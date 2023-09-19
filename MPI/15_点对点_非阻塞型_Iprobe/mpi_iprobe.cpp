#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main() {
    int rank;
    MPI_Request request;
    MPI_Status status;
    int value;
    int hava_message = 0;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0) {
        value = 10;
        MPI_Isend(&value, 1, MPI_INT, 1, 99, MPI_COMM_WORLD, &request);
        MPI_Request_free(&request);  // 直接释放掉该对象所占的资源，在该案例中没等接受就直接释放了资源 (非阻塞型如果不使用 wait，一定要释放)

    } else if(rank == 1) {
        for(int i = 0; i < 5; i++) {
            MPI_Iprobe(0, 99, MPI_COMM_WORLD, &hava_message, &status);  // 在不实际接收消息的情况下检查消息是否到达
            printf("hava_message is %d\n", hava_message);
        }        

        if(hava_message) {
            int receive_count = 0;
            MPI_Get_count(&status, MPI_INT, &receive_count);
            printf("receive_count is %d\n", receive_count);
        }
    }

    MPI_Finalize();
}