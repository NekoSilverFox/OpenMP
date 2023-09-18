#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
    MPI_Request request;
    MPI_Status status;
    int size, rank, data;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* 程序主题算法：进程rank 向 进程rank+1 发送数据 */
    if (rank > 0)
    {
        MPI_Irecv(&data, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        std::cout << "Rank [" << rank << "] has received data ["  << data << "] from rank [" << rank - 1 << "]\n";
    }

    std::cout << "Hello from rank [" << rank << "]\n";
    data = rank;
    if (rank < size - 1)
    {
        MPI_Isend(&data, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
    }

    MPI_Finalize();
}