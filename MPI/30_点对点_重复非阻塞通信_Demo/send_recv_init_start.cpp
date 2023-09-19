#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main() {
    int rank;
    int value;
    MPI_Request request;
    MPI_Status status;
    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0) {
        /* 1. 初始化通讯，但是此时并不会发送数据，只是初始化 */
        MPI_Send_init(&value, 1, MPI_INT, 1, 99, MPI_COMM_WORLD, &request);
        for(int i = 0; i < 10; i++) {
            value = i;
            /* 2. 开始发送数据 */
            MPI_Start(&request);

            /* 3. 完成通讯 */
            MPI_Wait(&request, &status);
        }

        /* 4. 释放查询对象 */
        MPI_Request_free(&request);
    }

    if(rank == 1) {
        MPI_Recv_init(&value, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, &request);
        for(int i = 0; i < 10; i++) {
            MPI_Start(&request);
            MPI_Wait(&request, &status);
            printf("value is %d\n", value);
        }
        MPI_Request_free(&request);
    }

    MPI_Finalize();
}