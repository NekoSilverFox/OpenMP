#include <iostream>
#include <mpi.h>

/**
 * MPI 散播：
 *  通过一个进程向同一个通信域的所有进程发送数据
 * 
 * 特点：
 *  分段发送，比如 int[100]，每次发送 n 个数据，向每个进程发送的区间为 int[rank*N] ~ int[rank*N + N]
 *  发送段数，该通信域进程总数
 */

const int N = 10;

int main(int argc, char* argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* 创建消息发送和接收缓冲区 */
    int* buf_send = new int[size * N * sizeof(int)];
    int* buf_recv = new int[N * sizeof(int)];

    /* 0 号进程为发送缓存填充数据  */
    if (0 == rank)
    {
        for (int i = 0; i < size * N; i++)
        {
            buf_send[i] = i;
        }
    }

    /* 散播 scatter */
    MPI_Scatter(buf_send, N, MPI_INT,
                buf_recv, N, MPI_INT,
                0, MPI_COMM_WORLD);

    /* 结果输出 */
    printf("\n============ rank %d ============\n", rank);
    for (int i = 0; i < N; i++)
    {
        printf("recv[%d]=%d\t", i, buf_recv[i]);
    }
    printf("\n");

    MPI_Finalize();
}