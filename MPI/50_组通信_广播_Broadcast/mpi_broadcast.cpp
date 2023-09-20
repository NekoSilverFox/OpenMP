#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int data = 0;
    if (rank == 0) data = 100;

    /* Broadcast 不会给自己（发送方）发送数据 */
    MPI_Bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("data = [%d] from rank [%d]\n", data, rank);

    MPI_Finalize();
}