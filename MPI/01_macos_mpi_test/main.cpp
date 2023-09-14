#include <iostream>
#include "mpi.h"

/**
 * 如果要编译 MPI 程序，不能使用 gcc/clang
 * 
 *  编译：mpicc XXX.cpp -o EXE
 *  运行：mpirun -np 处理器数量 EXE
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */

int main(int argc, char **argv)
{
    // Initialize the MPI environment 初始化 MPI 环境
    MPI_Init(&argc, &argv);
  
    // Get the number of processes ssociated with the communicator 获取与cpu相关的进程数
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the calling process 获取调用进程的级别
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor 获取处理器名称
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    printf("Hello world from process %s with rank %d out of %d processors\n", processor_name, world_rank, world_size);

    // Finalize: Any resources allocated for MPI can be freed  可以释放为 MPI 分配的任何资源
    MPI_Finalize();
}