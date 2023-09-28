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

#define DEBUG                     0
#define GENERATE_NEW_MATRIX_VECTOR  1
const int RANK_ROOT             = 0;
const int RANDOM_SAND           = 5140903;
const char SOURCE_MATRIX_FILE[] = "source_matrix";
const char SOURCE_VECTOR_FILE[] = "source_vector";
const char RESULT_FILE[]        = "result";
const char SEPARATOR            = ' ';
const int MATRIX_MAX_NUM        = 10;
const int VECTOR_MAX_NUM        = 10;
const int M                     = 200;  // SET M HERE
const int N                     = 150;  // SET N HERE
const int TAG_RES               = 0;


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
            fout << rand() % MATRIX_MAX_NUM << ((col + 1 == n)? '\n' : SEPARATOR);
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
        std::cout << "Rank " << rank << " successfully randomized the matrix " << M << "x" << N << " to file `" << SOURCE_MATRIX_FILE << "`\n";

        /* Generating random vector */
        is_suc = generateRadomVector2File(SOURCE_VECTOR_FILE, N);
        if (!is_suc) return 1;
        std::cout << "Rank " << rank << " successfully randomized the vector " << N << " to file `" << SOURCE_VECTOR_FILE << "`\n";
        std::cout << "------------------------------------------------\n";
    }
#endif


    /* Assuming that ONLY the root process knows the name of the file and the other processes do not
     * so Broadcast source files name and Number of rows to be processed per process */
    int length_matrix_file_name;
    char* matrix_file_name;
    int length_vector_file_name;
    char* vector_file_name;
    int row_matrix = 0;
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

        /* Get file matrix row */
        std::fstream fin(matrix_file_name, std::ios::in);
        if (!fin)
        {
            std::cerr << "[ERROR] Can not open file " << matrix_file_name << "\n";
            return 1;
        }
        
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

    }

    /************* Broadcast source files name and Number of rows to be processed per process *************/
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
        MPI_Bcast(matrix_file_name, length_matrix_file_name, MPI_CHAR, RANK_ROOT, MPI_COMM_WORLD);
    }
    else
    {
        matrix_file_name = new char[length_matrix_file_name];
        MPI_Bcast(matrix_file_name, length_matrix_file_name, MPI_CHAR, RANK_ROOT, MPI_COMM_WORLD);
    }

    MPI_Bcast(num_col_every_rank, size, MPI_INT, RANK_ROOT, MPI_COMM_WORLD);

    /************************************ Get vector ************************************/
    std::string str_v;
    std::fstream finv(vector_file_name, std::ios::in);
    if (!vector_file_name)
    {
        std::cerr << "[ERROR] Can not open file " << matrix_file_name << "in rank " << rank << "\n";
        return 1;
    }
    getline(finv, str_v);
    std::vector<int> v_vector = splitString(str_v, SEPARATOR);
    finv.close();

    /************************** Move file ptr for local rank **************************/
    int i_ptr = 0;
    for (int i = 0; i < rank; i++)
    {
        i_ptr += num_col_every_rank[i];
    }
    std::fstream finm(matrix_file_name, std::ios::in);
    std::string cur_line;
    while (0 != i_ptr)
    {
        getline(finm, cur_line);
        --i_ptr;
    }

    /************************* Compute matrix's every col*vector *************************/
    std::vector<int> v_matrix_cur_line;
    int local_arr_res[num_col_every_rank[rank]];
    int tmp_sum;
    for (int i = 0; i < num_col_every_rank[rank]; i++)
    {
        tmp_sum = 0;
        getline(finm, cur_line);
        v_matrix_cur_line = splitString(cur_line, SEPARATOR);

        for (int j = 0; j < v_matrix_cur_line.size(); j++)
        {
            tmp_sum += (v_matrix_cur_line[j] * v_vector[j]);
        }
        local_arr_res[i] = tmp_sum;
    }
    finm.close();
    
    /************************* Send result arr to root *************************/
    int* arr_res;
    MPI_Request request;
    MPI_Status status;
    MPI_Isend(local_arr_res, num_col_every_rank[rank], MPI_INT, RANK_ROOT, TAG_RES, MPI_COMM_WORLD, &request);

    /***************************** Mix result arr *****************************/
    if (0 == rank)
    {
        int* res_rank_buf;
        int cur_i_arr_res = 0;
        arr_res = new int[row_matrix];
        for (int i = 0; i < size; i++)
        {
            res_rank_buf = new int[num_col_every_rank[i]];
            MPI_Irecv(res_rank_buf, num_col_every_rank[i], MPI_INT, i, TAG_RES, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

            for (int j = 0; j < num_col_every_rank[i]; j++)
            {
                arr_res[cur_i_arr_res + j] = res_rank_buf[j];
            }
            cur_i_arr_res += num_col_every_rank[i];
        }

        std::fstream fres(RESULT_FILE, std::ios::out);
        if (!fres)
        {
            std::cerr << "[ERROR] Can not open file " << matrix_file_name << "in rank " << rank << "\n";
            return 1;
        }
        
        std::cout << "\n\e[92m>>>>>>> Final result: ";
        for (int i = 0; i < row_matrix; i++)
        {
            std::cout << arr_res[i] << " ";
            fres << arr_res[i] << " ";
        }
        fres.close();
         std::cout << "\n########### Successfully wrote the result to the file `" << RESULT_FILE << "` ###########\e[0m\n";
    }


#if DEBUG
    std::cout << "=============================== rank " << rank << " ===============================\n"
              << "matrix_file_name = " << matrix_file_name << "\tlength_matrix_file_name = " << length_matrix_file_name << "\n"
              << "vector_file_name = " << vector_file_name << "\tlength_vector_file_name = " << length_vector_file_name << "\n";
        
    std::cout << "num_col_every_rank in rank " << rank << " is ";
    for (int i = 0; i < size; i++)
    {
        std::cout << num_col_every_rank[i] << " ";
    }

    std::cout << "\nv_vector in rank " << rank << " is ";
    for (auto i : v_vector) std::cout << i << " ";
    std::cout << "\tsize = " << v_vector.size();
#endif

    std::cout << ">> Local result product in rank " << rank << ": ";
    for (int i = 0; i < num_col_every_rank[rank]; i++)
    {
        std::cout << local_arr_res[i] << " ";
    }
    std::cout << "\n";

    MPI_Finalize();
}