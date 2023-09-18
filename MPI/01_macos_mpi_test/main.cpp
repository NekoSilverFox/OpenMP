#include <iostream>
#include "mpi.h"

/**
 * 如果要编译 MPI 程序，不能使用 gcc/clang
 * 
 *  编译：mpicc XXX.cpp -o EXE
 *  运行：mpirun -np 处理器数量 EXE
 */

int main(int argc, char **argv)
{
    /* 1. Initialize the MPI environment 初始化 MPI 环境，用于接受命令行的参数 */
    MPI_Init(&argc, &argv);
  
    /* 2. Get the number of processes ssociated with the communicator 获取通信域总的进程数 */
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    /* 3. Get the rank of the calling process 获取当前的进程 ID  */
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    /* 4. Get the name of the processor 获取处理器名称 */
    char processor_name[MPI_MAX_PROCESSOR_NAME];  // MPI_MAX_PROCESSOR_NAME = 256
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    printf("Hello world from process %s with rank %d out of %d processors\n", processor_name, world_rank, world_size);

    /* 获取当前系统时间 */
    double time = MPI_Wtime();
    std::cout << "Func MPI_Wtime(): " << time << "\n";

    /* 5. Finalize: Any resources allocated for MPI can be freed  释放/结束为 MPI 分配的任何资源，一般放在最后一行，如果不添加这个程序不会终止 */
    MPI_Finalize();
}