#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int size, rank;
    int res = -1;  // 如果不初始化，会报错
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int data = rank;
    printf("data in rank %d is %d\n", rank, data);

    // MPI_Reduce(&data, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // 所有进程都要调用，并不是只有根进程。但只有根进程的结果是对的
    MPI_Allreduce(&data, &res, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);  // 所有进程都会获得最终的结果

    printf("Result reduce in rank %d is %d\n", rank, res);

    MPI_Finalize();
}