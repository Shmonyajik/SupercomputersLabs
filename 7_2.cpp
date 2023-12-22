#include <vector>
#include <iostream>
#include <omp.h>

static int numThreads;

// Функция для обмена строк в матрице
void customSwap(std::vector<std::vector<double>>& matrix, int row1, int row2, int columns)
{
#pragma omp parallel for
    for (int i = 0; i < columns; i++)
    {
        double temp = matrix[row1][i];
        matrix[row1][i] = matrix[row2][i];
        matrix[row2][i] = temp;
    }
}

// Функция для вывода матрицы
void displayMatrix(std::vector<std::vector<double>>& matrix, int rows, int columns);

// Функция для вычисления ранга матрицы
int calculateMatrixRank(std::vector<std::vector<double>>& matrix, int size, int numThreads)
{
    int rank = size;

    // Установка числа потоков OpenMP
    if (numThreads > 0 && numThreads <= omp_get_max_threads())
        omp_set_num_threads(numThreads);

    for (int row = 0; row < rank; row++)
    {
        if (std::abs(matrix[row][row]) > 0.00001)
        {
            // Параллельная часть: обнуление элементов под и над текущим элементом
#pragma omp parallel for
            for (int col = 0; col < size; col++)
            {
                if (col != row)
                {
                    double multiplier = matrix[col][row] / matrix[row][row];
                    for (int i = 0; i < rank; i++)
                        matrix[col][i] -= multiplier * matrix[row][i];
                }
            }
        }
        else
        {
            // Если текущий элемент ноль, меняем строки местами или уменьшаем ранг
            bool reduce = true;

            for (int i = row + 1; i < size; i++)
            {
                if (std::abs(matrix[i][row]) > 0.00001)
                {
                    customSwap(matrix, row, i, rank);
                    reduce = false;
                    break;
                }
            }

            if (reduce)
            {
                rank--;
                // Убираем последний столбец (и строку) для уменьшения ранга
#pragma omp parallel for
                for (int i = 0; i < size; i++)
                    matrix[i][row] = matrix[i][rank];
            }
            row--;
        }

        displayMatrix(matrix, rank, rank);
        printf("\n");
    }
    return rank;
}

// Функция для вывода матрицы
void displayMatrix(std::vector<std::vector<double>>& matrix, int rows, int columns)
{
    std::cout << "Current Matrix:\n";
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
            std::cout << "  " << matrix[i][j];
        std::cout << "\n";
    }
}

int main()
{
    int size;
    std::cout << "Enter matrix size (N): ";
    std::cin >> size;

    std::cout << "\nEnter number of threads: ";
    std::cin >> numThreads;

    std::cout << "\nEnter matrix values:\n";
    std::vector<std::vector<double>> matrix(size, std::vector<double>(size));

    // Ввод значений матрицы
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            std::cin >> matrix[i][j];
        }
    }

    // Вычисление и вывод ранга матрицы
    std::cout << "Rank of the matrix is: " << calculateMatrixRank(matrix, size, numThreads) << std::endl;

    return 0;
}
