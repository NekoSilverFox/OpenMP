/** -*- coding: utf-8 -*-
 * @Time    : 2023/9/10 19:31
 * @Author  : 冰糖雪狸 | Мэн Цзянин
 * @Project : 50_OpenMP_lab6_string
 * @File    : main.cpp
 * @Software: CLion
 * @Github  ：https://github.com/NekoSilverFox
 * -----------------------
 **/

#include <iostream>
#include <omp.h>

/**
 * 4. Дана последовательность символов С = {с0...сn–1} и символ b. Создать OpenMP-приложение для определения количество вхождений символа b в строку C.
 * Количество символов и потоков являются входными параметра- ми программы, количество символов в строке может быть не кратно количеству потоков.
 */

int vSerialLab6(const std::string str, const char b)
{
    int res = 0;
    for (char i : str) if (i == b) ++res;
    return res;
}

int vParallelLab6(const std::string str, const char b, const int num_threads)
{
    int res = 0;

    #pragma omp parallel for num_threads(num_threads)
    for (char i : str)
    {
        if (i == b) ++res;
//        printf("%d thread is running\n", omp_get_thread_num());
    }

    return res;
}

int main(int argc, char** argv)
{
    /* Check parameters */
    if (3 != argc)
    {
        printf("[ERROR] The program does not have enough parameters:\n"
               "parameter 1 should be a string and parameter 2 number threads.");
        return -1;
    }

    std::string str = argv[1];
    int num_threads = atoi(argv[2]);
    if (num_threads > omp_get_max_threads()) num_threads = omp_get_max_threads();
    printf("Number threads: %d\n", num_threads);

    /* Serial version of lab6 */
    int res_serial = vSerialLab6(str, 'b');
    printf("Result of version serial: %d\n", res_serial);

    /* Parallel version of lab6 */
    int res_parallel = vParallelLab6(str, 'b', num_threads);
    printf("Result of version parallel: %d\n", res_parallel);

    return 0;
}
