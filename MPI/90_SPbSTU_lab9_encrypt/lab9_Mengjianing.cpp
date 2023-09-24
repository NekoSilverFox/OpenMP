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
const int TAG_KEY         = 0;

/**
 * @brief Получить зашифрованный словарь случайных чисел по текущей дате
 * 
 * @return std::string Словарь сопоставлений, необходимых для шифрования строк
 */
std::string get_today_key_dic()
{
    /* Получить текущую дату */
    time_t cur_time;
    time(&cur_time);
    
    tm* date = localtime(&cur_time);
    int year = 1900 + date->tm_year;
    int mon = 1 + date->tm_mon;
    int day = date->tm_mday;
    std::cout << "Time now: " << day << "-" << mon << "-" << year << "\n";
    
    /* Получить словарь случайных чисел, зашифрованных по текущему времени */
    srand(day * 1000000 + mon * 10000 + year);
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
    int size, rank;
    MPI_Status status;
    MPI_Init(&argv, &argc);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Boardcast Словарь для шифрования */
    char* key_dic = new char[NUM_RANDOM_CHAR];
    if (0 == rank)
    {
        std::cout << "===================================== lab9 =====================================\n";
        get_today_key_dic().copy(key_dic, NUM_RANDOM_CHAR);
        std::cout << "MPI size = " << size << "\n"
                  << "str_dic = " << str_dic << "\tlength = " << strlen(str_dic) << "\n"
                  << "key_dic = " << key_dic << "\tlength = " << strlen(str_dic) << "\n"
                  << "================================================================================\n";
    }
    MPI_Bcast(key_dic, NUM_RANDOM_CHAR, MPI_CHAR, 0, MPI_COMM_WORLD);
    std::cout << "key_dic in rank " << rank << " = " << key_dic << "\tlength = " << strlen(str_dic) << "\n";
    
#if DEBUG
    MPI_Barrier(MPI_COMM_WORLD);
#endif

    /* Передача строки для шифрования, метод передачи информации «точка-точка», root - rank 0 */
    char str_source[] = "ABCDE89o ,.";
    char* arr_local_char;
    int local_length;
    if (0 == rank)
    {
        if (0 == str_source.size() % size) 
        {
            local_length = 0;
        }
    }


    // std::cout << (int)11/3 << "\t" << 11 % 3 << "\n";

    MPI_Finalize();
}