#include <stdio.h>
#include <mpi.h>
using namespace std;

/**
 * @brief 0 号进程发送一个数据到 1 号进程
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */

int main(int argc, char* argv[])
{
    int world_rank, world_size;
    MPI_Status status;

    MPI_Init(&argc, &argv);  // 初始化
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);  // 通信域中总进程数
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);  // 当前进程 ID

    if (0 == world_rank)
    {
        int send_data = 23333;
        MPI_Send(&send_data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("【总进程数：%d】%d 号进程发送数据 %d 到 1 号进程\n", world_size, world_rank, send_data);
    }
    else if (1 == world_rank)
    {
        int recv_data = 0;
        printf("【总进程数：%d】%d 号进程接收数据前 recv_data = %d\n", world_size, world_rank, recv_data);
        MPI_Recv(&recv_data, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("【总进程数：%d】%d 号进程接收数据后 recv_data = %d\n", world_size, world_rank, recv_data);
    }
    

    MPI_Finalize();
}