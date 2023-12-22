#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <mpi.h>

#define MATRIX_SIZE 5

int main(int argc, char* argv[]) {
    MPI_Init(NULL, NULL);

    int processRank, totalProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses);

    int rowsPerProcess = MATRIX_SIZE / totalProcesses;
    std::vector<int> matrixA(MATRIX_SIZE * MATRIX_SIZE);
    std::vector<int> matrixB(MATRIX_SIZE * MATRIX_SIZE);
    std::vector<int> matrixC(MATRIX_SIZE * MATRIX_SIZE);

    std::vector<int> bufferB(MATRIX_SIZE * rowsPerProcess);
    std::vector<int> partialResult(MATRIX_SIZE * rowsPerProcess);

    double startTime, stopTime;

    if (processRank == 0) {
        // Генерация и сохранение матриц A и B в файлы
        std::ofstream fileA("C:/C++/Lab6/tmpFiles/a.txt");
        std::ofstream fileB("C:/C++/Lab6/tmpFiles/b.txt");

        // Заполнение матриц A и B случайными числами и запись их в файлы
        for (int i = 0; i < MATRIX_SIZE; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                matrixA[i * MATRIX_SIZE + j] = rand() % 1000 - 16;
                matrixB[i * MATRIX_SIZE + j] = rand() % 1000 - 16;

                fileA << matrixA[i * MATRIX_SIZE + j] << " ";
                fileB << matrixB[i * MATRIX_SIZE + j] << " ";
            }
            fileA << '\n';
            fileB << '\n';
        }

        fileA.close();
        fileB.close();

        startTime = MPI_Wtime();

        // Распределение данных матрицы B
        for (int i = 1; i < totalProcesses; i++) {
            MPI_Send(matrixB.data() + i * rowsPerProcess * MATRIX_SIZE, MATRIX_SIZE * rowsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Распределение данных матрицы A
        for (int i = 1; i < totalProcesses; i++) {
            MPI_Send(matrixA.data() + (i - 1) * rowsPerProcess * MATRIX_SIZE, MATRIX_SIZE * rowsPerProcess, MPI_INT, i, 1, MPI_COMM_WORLD);
        }

        // Вычисление части матрицы C на процессе 0
        for (int i = 0; i < rowsPerProcess; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                int temp = 0;
                for (int k = 0; k < MATRIX_SIZE; k++) {
                    temp += matrixA[i * MATRIX_SIZE + k] * matrixB[k * MATRIX_SIZE + j];
                }
                matrixC[i * MATRIX_SIZE + j] = temp;
            }
        }

        // Сбор результатов от других процессов
        for (int processIndex = 1; processIndex < totalProcesses; processIndex++) {
            MPI_Recv(partialResult.data(), MATRIX_SIZE * rowsPerProcess, MPI_INT, processIndex, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (int i = 0; i < rowsPerProcess; i++) {
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    matrixC[(processIndex * rowsPerProcess + i) * MATRIX_SIZE + j] = partialResult[i * MATRIX_SIZE + j];
                }
            }
        }

        stopTime = MPI_Wtime();

        // Вывод результата
        std::ofstream outputFile("C:/C++/Lab6/tmpFiles/res.txt");
        for (int i = 0; i < MATRIX_SIZE; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                outputFile << matrixC[i * MATRIX_SIZE + j] << " ";
            }
            outputFile << '\n';
        }
        outputFile.close();

        std::cout << "Calculation completed in: " << stopTime - startTime << "s\n";
    }
    else {
        // Получение данных матрицы B
        MPI_Recv(bufferB.data(), MATRIX_SIZE * rowsPerProcess, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Получение данных матрицы A
        MPI_Recv(matrixA.data(), MATRIX_SIZE * rowsPerProcess, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Вычисление части матрицы C на других процессах
        for (int i = 0; i < rowsPerProcess; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                int temp = 0;
                for (int k = 0; k < MATRIX_SIZE; k++) {
                    temp += matrixA[i * MATRIX_SIZE + k] * bufferB[k * MATRIX_SIZE + j];
                }
                partialResult[i * MATRIX_SIZE + j] = temp;
            }
        }

        // Отправка результата процессу 0
        MPI_Send(partialResult.data(), MATRIX_SIZE * rowsPerProcess, MPI_INT, 0, 3, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
