/** -*- coding: utf-8 -*-
 * @Time    : 2023/9/10 20:42
 * @Author  : 冰糖雪狸 | Мэн Цзянин
 * @Project : 52_OpenMP_lab7_1_itc
 * @File    : main.cpp
 * @Software: CLion
 * @Github  ：https://github.com/NekoSilverFox
 * -----------------------
 **/

#include <stdio.h>
#include <math.h>
#include <omp.h>

#define N 1000

int main(int argc, char** argv)
{
    double b[N];
    double s = 0;

    /* Устранение проблем "гонки" при обеспечении параллельной производительности */
 #pragma omp parallel for reduction(+: s)
    for (int i = 0; i < N; i++)
    {
        b[i] = i * sqrt(i);
        s += b[i];
    }

    printf("%f", s);  // correct - 12633303.179735
    return 0;
}
