/**
 * @file lab12_MengJianing.cpp
 * @author MengJianing | Мэн Цзянин
 * @brief 
 * @version 0.1
 * @date 2023-09-25
 * 
 * @copyright Copyright (c) 2023 https://github.com/NekoSilverFox
 * 
 * 4. Задача о вычислении произведения матрицы на вектор-2.
 *    Дана A – матрица размерностью m строк на n столбцов и дан вектор x размерностью n.
 *    Исходные данные хранятся в файлах на диске, подготовленные для каждого процессора
 *    (написать последовательную программу подготовки данных, входным параметром которой является число процессоров).
 *    Написать программу для p процессоров (2<=p<<m,n, p%n, p%m != 0), вычисляющую произведение Ax,
 *    как линейную комбинацию n векторов. Распределение нагрузки на процессоры провести наиболее удобным способом.
 * 
 */

#include <iostream>
#include <fstream>
#include <mpi.h>

const int RANK_ROOT             = 0;
const int TAG_MSG               = 0;
const int RANDOM_SAND           = 5140904;
const char SOURCE_MATRIX_FILE[] = "source_matrix.txt";
const int MATRIX_MAX_NUM        = 10;
const int M                     = 10;
const int N                     = 10;  

/**
 * @brief Generat random matrix
 * 
 * @param file_name 
 * @param m num row of matrix
 * @param n col row of matrix
 * @return true Succuss generated
 * @return false Failed generated
 */
bool generateRadomMatrix2File(const char* file_name, const int m, const int n)
{
    std::fstream fout(SOURCE_MATRIX_FILE, std::ios::out);
    if (!fout)
    {
        std::cerr << "[ERROR] Can not open/create file " << SOURCE_MATRIX_FILE << "\n";
        return false;
    }

    srand(RANDOM_SAND);
    for (int row = 0; row < m; row++)
    {
        for (int col = 0; col < n; col++)
        {
            fout << rand() % MATRIX_MAX_NUM << " ";
        }
        fout << "\n";
    }
    fout.close();
    return true;
}

int main(int argc, char* argv[])
{
    int size, rank;
    bool isSuc = false;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (0 == rank)
    {
        std::cout << "======================= lab10-11 | Мэн Цзянин 5140904/30202 =======================\n"
                  << "MPI size = " << size << "\n";

        /* Generating random matrices */
        isSuc = generateRadomMatrix2File(SOURCE_MATRIX_FILE, M, N);
        std::cout << "Rank " << rank << " successfully randomized the matrix " << M << "x" << N << " to file " << SOURCE_MATRIX_FILE << "\n";
    }


    MPI_Finalize();
}