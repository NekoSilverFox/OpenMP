#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    static int max = 20;
    float send[10];
    float* buf_recv;

    /* 给每个进程发送的数据进行填充 */
    for (int i = 0; i < 10; i++) send[i] = i + 0.1 * rank;
    printf("\n============= send data in rank %d =============\n", rank);
    for (int i = 0; i < 10; i++) printf("%.1f ", send[i]);

    /* 以 0 号进程为根进程收集数据并分配所需内存空间 【大小为所有进程 send 数据缓存的大小之和】*/
    if (0 == rank) buf_recv = new float[sizeof(float) * 10 * size];

    /* 收集数据 */
    MPI_Gather(send, 10, MPI_FLOAT,
               buf_recv, 10, MPI_FLOAT,
               0, MPI_COMM_WORLD);

    /* 结果输出 */
    if (0 == rank)
    {
        printf("\n>>>> Recvived data result: \n");
        for (int i = 0; i < size * 10; i++) printf("%.1f ", buf_recv[i]);
    }
    printf("\n");

    MPI_Finalize();
}