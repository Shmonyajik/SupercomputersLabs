#include <mpi.h>
#include <iostream>
#include <ctime>

void myScatter(int* send, int Scount, void* recv, int Rcount, int size, int rank)
{
    MPI_Status status;

    if (rank == 0)
    {
        for (int i = 1; i < size; i++)
        {
            MPI_Send(send + i, Scount, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(recv, Rcount, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int* data = new int[size];
    int received = 0;

    if (rank == 0)
    {
        for (int i = 0; i < size; i++)
        {
            data[i] = i;
        }
    }

    // Измерение времени для вашей реализации myScatter
    double start_time_myScatter = MPI_Wtime();

    // Ваша реализация myScatter
    myScatter(data, 1, &received, 1, size, rank);

    // Измерение времени для вашей реализации myScatter
    double end_time_myScatter = MPI_Wtime();

    // Измерение времени для MPI_Scatter
    double start_time_MPI_Scatter = MPI_Wtime();

    // Реализация с использованием MPI_Scatter
    MPI_Scatter(data, 1, MPI_INT, &received, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Измерение времени для MPI_Scatter
    double end_time_MPI_Scatter = MPI_Wtime();

    if (rank == 0)
    {
        std::cout << "Time taken by myScatter: " << (end_time_myScatter - start_time_myScatter) * 1000 << " ms\n";
        std::cout << "Time taken by MPI_Scatter: " << (end_time_MPI_Scatter - start_time_MPI_Scatter) * 1000 << " ms\n";
    }

    std::cout << "Process " << rank << " received " << received << "\n";

    MPI_Finalize();
    delete[] data;

    return 0;
}