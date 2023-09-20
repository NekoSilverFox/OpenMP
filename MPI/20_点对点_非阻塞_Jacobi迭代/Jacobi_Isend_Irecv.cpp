#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int m = 18;
    int n = 18;
    int a[m][n];
    int b[m][n];
    int i, j, k;
    for(i = 0; i < m; i++) {
        for(j = 0; j < n; j++) {
            a[i][j] =  rand() / (RAND_MAX + 1.0) * 10 * (i + j) ;
        }
    }

    int size, rank;
    MPI_Init(NULL, NULL);
    MPI_Status status[4];
    MPI_Request request[4];
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // 每个进程计算的行数，为了简单这里假设正好可以除尽
    int gap = (m - 2) / size;
    int start = gap * rank + 1;
    int end = gap * (rank + 1);
    int bound;
    int left, right;
    
    // 迭代10次，计算时忽略了 0，n-1 行 和 0，n-1 列
    for(k = 0; k < 10; k++) {

        // 计算边界的值
        bound = start;
        for(j = 1; j < m -1; j++) {
            b[bound][j] = 0.25 * (a[bound-1][j] + a[bound+1][j] + a[bound][j+1] + a[bound][j-1]);     
        }
        bound = end;
        for(j = 1; j < m -1; j++) {
            b[bound][j] = 0.25 * (a[bound-1][j] + a[bound+1][j] + a[bound][j+1] + a[bound][j-1]);     
        }

        // 使用虚拟进程
        left = rank - 1;
        if(left < 0) {
            left = MPI_PROC_NULL;
        }

        right = rank + 1;
        if(right > size - 1) {
            right = MPI_PROC_NULL;
        }

        MPI_Isend(&b[start][0], n, MPI_INT, left, 99, MPI_COMM_WORLD, &request[0]);
        MPI_Isend(&b[end][0], n, MPI_INT, right, 99, MPI_COMM_WORLD, &request[1]);
        MPI_Irecv(&a[start - 1][0], n, MPI_INT, left, 99, MPI_COMM_WORLD, &request[2]);
        MPI_Irecv(&a[end+1][0], n, MPI_INT, right, 99, MPI_COMM_WORLD, &request[3]);

        // 计算剩余的部分
        for(i = start+1; i < end; i++) {
            for(j = 1; j < m -1; j++) {
                b[i][j] = 0.25 * (a[i-1][j] + a[i+1][j] + a[i][j+1] + a[i][j-1]);     
            }
        }
        for(i = start ; i <= end; i++) {
            for(j = 1; j < n - 1; j++) {
                a[i][j] = b[i][j];
            }
        }
        MPI_Waitall(4, request, status);
    }

    // 这里按照顺序输出结果
    for(k = 0; k< size; k++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if(rank == k) {
            for(i = start; i <= end; i++) {
                for(j = 1; j < n-1; j++) {
                    // printf("a[%d][%d] is %-4d ", i, j, a[i][j]);
                    printf("%-4d", a[i][j]);
                }
                printf("\n");
            }
        }
    }
    MPI_Finalize();
}