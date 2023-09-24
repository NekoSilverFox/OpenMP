/**
 * @file lab9_Mengjianing.cpp
 * @author MengJianing | Мэн Цзянин
 * @brief 
 * @version 0.1
 * @date 2023-09-22
 * 
 * @copyright Copyright (c) 2023 https://github.com/NekoSilverFox
 * 
 * 4. Криптономикон. В секретном институте No 127, находящемся где-то в Сибири, разработана новейшая система шифровки данных для малого бинеса.
 *    Система представляет собой ряд модулей, каждый из которых принимает на вход часть сообщения и шифрует его по определенной схеме.
 *    Какую именно часть текста шифрует каждый из модулей, задается специальным ключом, который меняется каждый день.
 *    Написать программу, моделирующую работу шифровальной системы. В качестве шифров использовать прямые подстановки
 *    (вид шифра, когда каждому символу исходного текста ставится в соответствие какойто символ шифрованного текста, причем всегда один и тот же).
 *    В качестве ключа использовать подстановку, нижняя строка которой задает номер модуля, текст делится на равные части,
 *    весь остаток получает тот модуль, который шифрует последнюю часть. Использовать метод передачи информации «точка-точка».
 */

#include <iostream>
#include <vector>
#include <mpi.h>
#include <ctime>

#define DEBUG               1
const char str_dic[]      = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ,.";
const int NUM_RANDOM_CHAR = strlen(str_dic); //26 + 26 + 10 + 3; // A~Z + a~z + 0~9 + space,.
const int ASCII_START     = 32;
const int ASCII_END       = 127;
const int TAG_MSG_LENGTH  = 0;
const int TAG_MSG         = 1;
const int RANK_ROOT       = 0;


/**
 * @brief Получить зашифрованный словарь случайных чисел по текущей дате
 * 
 * @param rank rank in MPI
 * @return std::string std::string Словарь сопоставлений, необходимых для шифрования строк
 */
std::string get_today_key_dic(const int rank)
{
    /* Получить текущую дату */
    time_t cur_time;
    time(&cur_time);
    
    tm* date = localtime(&cur_time);
    int year = 1900 + date->tm_year;
    int mon = 1 + date->tm_mon;
    int day = date->tm_mday;

    // std::cout << "Time now: " << day << "-" << mon << "-" << year << "\t in rank " << rank << "\n";

    /* Получить словарь случайных чисел, зашифрованных по текущему времени */
    srand(day * 1000000 + mon * 10000 + year + rank);
    int i = 0;
    char c;
    std::string key_dic = "";
    while (i < NUM_RANDOM_CHAR)
    {
        c = static_cast<char>(rand() % (ASCII_END - ASCII_START) + ASCII_START);
        if (key_dic.find(c) == std::string::npos)
        {
            key_dic += c;
            i++;
        }
    }

    return key_dic;
}

int main(int argv, char* argc[])
{
    char str_source[] = "ABCDE89o ,.";
    int size, rank;
    MPI_Status status;
    MPI_Init(&argv, &argc);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Словарь для шифрования каждого rank*/
    std::string key_dic = get_today_key_dic(rank);
    
    /* SEND строки для шифрования, метод передачи информации «точка-точка», root - rank 0 */
    std::string local_arr_char;
    int local_length;
    MPI_Request request;

    if (0 == rank)
    {
        std::cout << "======================= lab9 | Мэн Цзянин 5140904/30202 =======================\n"
                  << "MPI size = " << size << "\n"
                  << "str_source = " << str_source << "\tlength = " << strlen(str_source) << "\n"
                  << "================================================================================\n";

        if (0 == strlen(str_source) % size) 
        {
            local_length = strlen(str_source) / size;
            strncpy(&local_arr_char[0], str_source, local_length);

            for (int i = 1; i < size; i++)
            {
                MPI_Isend(&local_length, 1, MPI_INT, i, TAG_MSG_LENGTH, MPI_COMM_WORLD, &request);
                MPI_Isend(&str_source[local_length * i], local_length,  MPI_CHAR, i, TAG_MSG, MPI_COMM_WORLD, &request);
            }
        }
        else
        {
            local_length = strlen(str_source) / size;

            for (int i = 1; i < size; i++)
            {
                MPI_Isend(&local_length, 1, MPI_INT, i, TAG_MSG_LENGTH, MPI_COMM_WORLD, &request);
                MPI_Isend(&str_source[strlen(str_source) % size + local_length * i], local_length,  MPI_CHAR, i, TAG_MSG, MPI_COMM_WORLD, &request);
            }

            local_length += strlen(str_source) % size;
            strncpy(&local_arr_char[0], str_source, local_length);
        }
    }
    else
    {
        MPI_Irecv(&local_length, 1, MPI_INT, RANK_ROOT, TAG_MSG_LENGTH, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        MPI_Irecv(&local_arr_char[0], local_length, MPI_CHAR, RANK_ROOT, TAG_MSG, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
    }

#if DEBUG
    std::cout << "str_dic in rank " << rank << " = " << str_dic << "\tlength = " << strlen(str_dic) << "\n";
    std::cout << "key_dic in rank " << rank << " = " << key_dic << "\tlength = " << strlen(str_dic) << "\n";
    std::cout << "Local msg in rank " << rank << " is `" << local_arr_char << "` length is " << local_length << "\n\n";
#endif

    /* Шифрование строк с помощью картографических словарей */
    for (int i = 0; i < local_length; i++)
    {
        local_arr_char[i] = key_dic.at(key_dic.find(local_arr_char[i]));
    }

    /* Результаты возврата, метод передачи информации «точка-точка» */
    std::cout << "String after coding in rank " << rank << " is " << local_arr_char << "\tlength = " << strlen(local_arr_char) << "\n";
    


    MPI_Finalize();
}