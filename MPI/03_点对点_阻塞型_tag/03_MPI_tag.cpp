#include <iostream>
#include <mpi.h>


int main(int argc, char* argv[])
{
    /* init */
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float data;
    MPI_Status recv_status;

    if (0 == rank)
    {
        /* MPI_ANY_SOURCE, MPI_ANY_TAG 能接收来自任何任何进程的任意 TAG 的数据 */
        MPI_Recv(&data, 1, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_status);
        std::cout << "Receive data [" << data << "] from rank [" << recv_status.MPI_SOURCE << "]\n";
    }
    else
    {
        data = 0.1 * rank;
        MPI_Send(&data, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }


    MPI_Finalize();
    return 0;
}