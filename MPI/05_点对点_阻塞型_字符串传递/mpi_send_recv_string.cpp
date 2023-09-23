/**
 * @file mpi_send_recv_string.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-23
 * 
 * @copyright Copyright (c) 2023
 * 
 * 通过 MPI 传递 string/char* 字符串
 * 
 */

#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int size, rank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char* arr_char;
    int length;

    if (0 == rank)
    {
        arr_char = "TEST";
        length = strlen(arr_char) + 1;  // +1 是因为 strlen 没有吧 '\0' 计入

        std::cout << "Rank 0 发送进程中，arr_char = " << arr_char << " length = " << length << "\n";

        MPI_Send(&length, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(arr_char, length, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(&length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        arr_char = new char[length];  // 必须要提前分配内存，不然会报错
        MPI_Recv(arr_char, length, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);

        std::cout << "Rank 1 接收进程中，arr_char = " << arr_char << " length = " << length << "\n";
    }

    MPI_Finalize();
}