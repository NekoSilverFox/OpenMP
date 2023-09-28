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
#include <sstream>
#include <vector>
#include <fstream>
#include <mpi.h>

#define DEBUG                     1
#define GENERATE_NEW_MATRIX_VECTOR  1
const int RANK_ROOT             = 0;
const int RANDOM_SAND           = 5140903;
const char SOURCE_MATRIX_FILE[] = "source_matrix";
const char SOURCE_VECTOR_FILE[] = "source_vector";
const char SEPARATOR            = ' ';
const int MATRIX_MAX_NUM        = 10;
const int VECTOR_MAX_NUM        = 10;
const int M                     = 10;
const int N                     = 10;
const int TAG_LENGTH_MATRIX_FILE_NAME = 0;
const int TAG_MATRIX_FILE_NAME = 1;
const int TAG_LENGTH_VECTOR_FILE_NAME = 2;
const int TAG_VECTOR_FILE_NAME = 3;
const int TAG_NUM_VETOR_EVERY_RANK = 4;

/**
 * @brief Generat random matrix
 * 
 * @param file_name 
 * @param m num row of matrix
 * @param n num col of matrix
 * @return true Succuss generated
 * @return false Failed generated
 */
bool generateRadomMatrix2File(const char* file_name, const int m, const int n)
{
    std::fstream fout(file_name, std::ios::out);
    if (!fout)
    {
        std::cerr << "[ERROR] Can not open/create file " << file_name << "\n";
        return false;
    }

    srand(RANDOM_SAND);
    for (int row = 0; row < m; row++)
    {
        for (int col = 0; col < n; col++)
        {
            fout << rand() % MATRIX_MAX_NUM << ((col + 1 == m)? '\n' : SEPARATOR);
        }
    }
    fout.close();
    return true;
}


/**
 * @brief Generat random vector
 * 
 * @param file_name 
 * @param n 
 * @return true true Succuss generated
 * @return false false Failed generated
 */
bool generateRadomVector2File(const char* file_name, const int n)
{
    std::fstream fout(file_name, std::ios::out);
    if (!fout)
    {
        std::cerr << "[ERROR] Can not open/create file " << file_name << "\n";
        return false;
    }

    srand(RANDOM_SAND);
    for (int i = 0; i < n; i++)
    {
        fout << rand() % VECTOR_MAX_NUM << ((i + 1 == n)? "" : " ");
    }
    fout.close();
    return true;
}

/**
 * @brief Split string by chat c
 * 
 * @param str 
 * @param c 
 * @return std::vector<int> 
 */
std::vector<int> splitString(std::string str, const char c = SEPARATOR)
{
    std::istringstream iss(str);
    std::vector<int> res;
    std::string token;
    while (getline(iss, token, c))
    {
        res.push_back(std::stoi(token));
    }
    return res;
}


int main(int argc, char* argv[])
{
    int size, rank;
    bool is_suc = false;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (M < size)
    {
        std::cerr << "[ERROR] Matrix col can not << number processes\n";
        return 1;
    }

    /* Generating random matrices and vector, Exclusion from calculations */
#if GENERATE_NEW_MATRIX_VECTOR
    if (0 == rank)
    {
        std::cout << "======================= lab10-11 | Мэн Цзянин 5140904/30202 =======================\n"
                  << "MPI size = " << size << "\n";

        /* Generating random matrices */
        is_suc = generateRadomMatrix2File(SOURCE_MATRIX_FILE, M, N);
        if (!is_suc) return 1;
        std::cout << "Rank " << rank << " successfully randomized the matrix " << M << "x" << N << " to file " << SOURCE_MATRIX_FILE << "\n";

        /* Generating random vector */
        is_suc = generateRadomVector2File(SOURCE_VECTOR_FILE, N);
        if (!is_suc) return 1;
        std::cout << "Rank " << rank << " successfully randomized the vector " << N << " to file " << SOURCE_VECTOR_FILE << "\n";
    }
#endif


    /* Assuming that ONLY the root process knows the name of the file and the other processes do not
     * so Broadcast source files name and Number of rows to be processed per process */
    int length_matrix_file_name;
    char* matrix_file_name;
    int length_vector_file_name;
    char* vector_file_name;
    int num_col_every_rank[size];   // Number of rows to be processed per process, index arr - rank
    if (0 == rank)
    {
        /* Get file names for per process */
        length_vector_file_name = strlen(SOURCE_VECTOR_FILE);
        vector_file_name = new char[length_vector_file_name];
        strcpy(vector_file_name, SOURCE_VECTOR_FILE);

        length_matrix_file_name = strlen(SOURCE_MATRIX_FILE);
        matrix_file_name = new char(length_matrix_file_name);
        strcpy(matrix_file_name, SOURCE_MATRIX_FILE);
        std::cout << "######################1 " << matrix_file_name << "\n";  //NO ERRPR

        /* Get file matrix row */
        std::fstream fin(matrix_file_name, std::ios::in);
        if (!fin)
        {
            std::cerr << "[ERROR] Can not open file " << matrix_file_name << "\n";
            return 1;
        }
        int row_matrix = 0;
        std::string tmp;
        while (getline(fin, tmp))
        {
            if (tmp.size() != 0) ++row_matrix;
        }
        fin.close();

        /* Spread the load on the processors, Распределение нагрузки на процессоры провести */
        int mean = row_matrix / size;
        int mod = row_matrix % size;
        for (int i = 0; i < size; i++)
        {
            num_col_every_rank[i] = mean;
            if (mod > 0)
            {
                num_col_every_rank[i] += 1;
                --mod;
            }
        }
        std::cout << "#######################2 " << matrix_file_name << "\n";

    }

    /* Broadcast source files name and Number of rows to be processed per process */
    MPI_Bcast(&length_vector_file_name, 1, MPI_INT, RANK_ROOT, MPI_COMM_WORLD);
    if (0 == rank)
    {
        MPI_Bcast(vector_file_name, length_vector_file_name, MPI_CHAR, RANK_ROOT, MPI_COMM_WORLD);
    }
    else
    {
        vector_file_name = new char[length_vector_file_name];
        MPI_Bcast(vector_file_name, length_vector_file_name, MPI_CHAR, RANK_ROOT, MPI_COMM_WORLD);
    }

    MPI_Bcast(&length_matrix_file_name, 1, MPI_INT, RANK_ROOT, MPI_COMM_WORLD);
    if (0 == rank)
    {
        std::cout << "#######################3 " << matrix_file_name << "\n";
        MPI_Bcast(matrix_file_name, length_matrix_file_name, MPI_CHAR, RANK_ROOT, MPI_COMM_WORLD);
    }
    else
    {
        matrix_file_name = new char[length_matrix_file_name];
        MPI_Bcast(matrix_file_name, length_matrix_file_name, MPI_CHAR, RANK_ROOT, MPI_COMM_WORLD);
    }

    MPI_Bcast(num_col_every_rank, size, MPI_INT, RANK_ROOT, MPI_COMM_WORLD);

    /* Get vector */
    std::string str_v;
    std::fstream fin(vector_file_name, std::ios::in);
    if (!vector_file_name)
    {
        std::cerr << "[ERROR] Can not open file " << matrix_file_name << "in rank " << rank << "\n";
    }
    getline(fin, str_v);
    std::vector<int> v_vector = splitString(str_v, SEPARATOR);
    fin.close();

        std::cout << "#######################4 " << matrix_file_name << "\n";

#if DEBUG
    std::cout << "=============================== rank " << rank << " ===============================\n"
              << "length_matrix_file_name = " << length_matrix_file_name << "\n"
              << "matrix_file_name = " << matrix_file_name << "\n\n"

              << "length_vector_file_name = " << length_vector_file_name << "\n"
              << "vector_file_name = " << vector_file_name  << "\n";
        
    std::cout << "num_col_every_rank in rank " << rank << " is ";
    for (int i = 0; i < size; i++)
    {
        std::cout << num_col_every_rank[i] << " ";
    }

    std::cout << "\nv_vector in rank " << rank << " is ";
    for (auto i : v_vector) std::cout << i << " ";
    std::cout << "\n";
#endif


    MPI_Finalize();
}