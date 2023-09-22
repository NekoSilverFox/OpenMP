#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int size, rank;
    int res = -1;  // 如果不初始化，会报错
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int local_size = 8,  // 每个进程自己数组的大小
              global_size = size * local_size,  // 源数组的大小
              const_sum = 1;

    int global_data[global_size], local_data[local_size], count[local_size], local_sum[const_sum];

    /* 初始化原数组 */
    if (0 == rank)
        for (int i = 0; i < global_size; global_data[i] = i, i++);

    for (int i = 0; i < local_size; count[i] = 1, i++);

    MPI_Scatter(global_data, local_size, MPI_INT,
                local_data, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    printf("local data in rank %d is: ", rank);
    for (int i = 0; i < local_size; i++) printf("%d\t", local_data[i]);
    printf("\n");

    MPI_Reduce_scatter(local_data, local_sum, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    printf("\nlocal sum in rank %d is:", rank);
    for (int i = 0; i < const_sum; i++) printf("%d\t", local_sum[i]);
    printf("\n");

    MPI_Finalize();

}