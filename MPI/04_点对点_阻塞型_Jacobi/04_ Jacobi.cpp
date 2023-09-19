/**
 * 雅阁比矩阵迭代
 *  矩阵四周为 8，中间为 0
 */

#include <iostream>
#include <mpi.h>

#define N 8  // 矩阵大小为 8x8
#define SIZE N/4  // 采用 4 个进程，每个进程的数组大小
#define T 2

void printRows(int rank, float arr2d[][N])
{
    MPI_Status status;

}


int main(int argc, char* argv[])
{
    int rank, size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* 本地矩阵大小,0 和 3 行是 buffer 层 */
    float rank_arr2d[SIZE + 2][N], tmp_rank_arr2d[SIZE + 2][N], tmp[SIZE][N], c[N][N];

    /* 给初始矩阵赋值，中间为 0，四周为 8 */
    for (int i = 0; i < SIZE + 2; i++)
    {
        for (int j = 0; i < SIZE + 2; j++)
        {
            rank_arr2d[i][j] = tmp_rank_arr2d[i][j] = 0.0;
        }
    }
    if (0 == rank) for (int i = 0; i < N; i++) rank_arr2d[1][i] = 8.0;
    else if (3 == rank) for (int i = 0; i < N; i++) rank_arr2d[SIZE][i] = 8.0;
    for (int i = 1; i < SIZE + 1; i++) rank_arr2d[i][0] = rank_arr2d[i][SIZE - 1] = 8.0;
    
    /* Jacobi interation 部分 */
    for (int step = 0; step < T; step++)
    {
        /* 从下方进程接收数据 */
        if (rank < 3) MPI_Recv(&rank_arr2d[SIZE + 1][0], N, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, &status);

        /* 向 上方 进程发送数据 */
        if (rank > 0) MPI_Send(&rank_arr2d[1][0], N, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD);

        /* 向 下方 进程发送数据 */
        if (rank < 3)  MPI_Send(&rank_arr2d[SIZE][0], N, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
        
        /* 从上方进程接收数据 */
        if (rank > 0) MPI_Recv(&rank_arr2d[0][0], N, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, &status);

        /* 计算，只关注中间数据的迭代操作 */
        int r_begin = (0 == rank)? 2 : 1;
        int r_end = (3 == rank)? SIZE - 1 : SIZE;
        for (int i = r_begin; i <= r_end; i++)
        {
            for (int j = 1; j < N-1; j++)
            {
                tmp_rank_arr2d[i][j] =  0.25*(rank_arr2d[i-1][j] + rank_arr2d[i+1][j] + rank_arr2d[i][j+1] + rank_arr2d[i][j-1]);
            }
        }

        /* 更新 */
        for (int i = r_begin; i <= r_end; i++)
        {
            for (int j = 1; j < N - 1; j++)
            {
                rank_arr2d[i][j] = tmp_rank_arr2d[i][j];
            }
        }

    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 1; i < SIZE + 1; i++)
    {
        for (int j = 0; j < N; j++)
        {
            tmp[i - 1][j] = rank_arr2d[i][j];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(tmp, 16, MPI_FLOAT, c, 16, MPI_FLOAT, 0, MPI_COMM_WORLD);

    /* 输出结果 */
    if (0 == rank)
    {
        std::cout << "Result: \n";
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                printf("%.3f\t", c[i][j]);
            }
            std::cout << std::endl;
        }
    }

    MPI_Finalize();
}