/**
 * @file lab10_11_MengJianing.cpp
 * @author MengJianing | Мэн Цзянин
 * @brief 
 * @version 0.1
 * @date 2023-09-25
 * 
 * @copyright Copyright (c) 2023  https://github.com/NekoSilverFox
 * 
 * 
 * 4. Реализовать сбор значений с n процессов с помощью двухточечных обменов (каждый процесс посылает одно значение).
 *    Эффективность реализации сравнить с функцией MPI_Gather()
 */

#include <iostream>
#include <iomanip>
#include <mpi.h>
#include <chrono>


const int RANK_ROOT       = 0;
const int TAG_MSG         = 0;


int main(int argc, char* argv[])
{
    int size, rank;
    double t_p2p, t_gather;
    std::chrono::system_clock::time_point t_start, t_end;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double local_data = rank + 0.1;
    double* buf_recv;

    /* Test time uesd MPI function Send and Recv */
    MPI_Send(&local_data, 1, MPI_DOUBLE, RANK_ROOT, TAG_MSG, MPI_COMM_WORLD);
    if (0 == rank)
    {
        std::cout << "======================= lab10-11 | Мэн Цзянин 5140904/30202 =======================\n"
                  << "MPI size = " << size << "\n";

        buf_recv = new double[size];
        t_start = std::chrono::system_clock::now();
        for (int i = 0; i < size; i++)
        {
            MPI_Recv(&buf_recv[i], 1, MPI_DOUBLE, i, TAG_MSG, MPI_COMM_WORLD, &status);
        }
        t_end = std::chrono::system_clock::now();

        // for (int i = 0; i < size; i++) std::cout << buf_recv[i] << " ";
        const std::chrono::duration<double, std::milli> t_ms = t_end - t_start;
        t_p2p = t_ms.count();
        std::cout << "\n\n----------------- Test MPI_Send and MPI_Recv -----------------\n"
                  << std::setw(10) << "Size" << std::setw(20) << "Total time" << std::setw(20) << "Average time\n"
                  << std::setw(10) << size << std::setw(17) << t_ms.count() << " ms" << std::setw(16) << t_ms.count() / size << " ms\n\n";
    }


    MPI_Barrier(MPI_COMM_WORLD);

    /* Test time uesd MPI function Gather */
    if (0 == rank)
    {
        buf_recv = new double[size];
        t_start = std::chrono::system_clock::now();
    }
    MPI_Gather(&local_data, 1, MPI_DOUBLE, buf_recv, 1, MPI_DOUBLE, RANK_ROOT, MPI_COMM_WORLD);
    if (0 == rank)
    {
        t_end = std::chrono::system_clock::now();
        const std::chrono::duration<double, std::milli> t_ms = t_end - t_start;
        t_gather = t_ms.count();
        std::cout << "\n----------------------- Test MPI_Gather -----------------------\n"
                  << std::setw(10) << "Size" << std::setw(20) << "Total time" << std::setw(20) << "Average time\n"
                  << std::setw(10) << size << std::setw(17) << t_ms.count() << " ms" << std::setw(16) << t_ms.count() / size << " ms\n\n";
        // for (int i = 0; i < size; i++) std::cout << buf_recv[i] << " ";
        std::cout << "\n###### MPI_Gather " << t_p2p / t_gather << " times faster than MPI point to point MPI_Send-MPI_Recv\n\n";
    }

    MPI_Finalize();
}