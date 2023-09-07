#include <iostream>
#include <omp.h>
#include <unistd.h>

int main()
{
    std::cout << "===== clause private =====" << std::endl;

    double a[1000], b[1000], c[1000], d[1000];
    double tmp = 2.333333;
    for (int i = 0; i < 1000; ++i)
    {
        a[i] = i;
        b[i] = 1000 - i;
    }

    /* 每个线程都有与此变量**同名**的变量（==注意：==并不是深拷贝一份，而是一个未初始化的新同名变量，是只变量名一样而已） */
#pragma omp parallel num_threads(8) private(tmp)
    {
        printf("tid %d 的线程中变量 tmp 的值：%d\n", omp_get_thread_num(), tmp);  // 每个线程中 tmp 都是不确定的
    }

#pragma omp parallel for num_threads(8)
//#pragma omp parallel for num_threads(8) private(for)  // 将变量进行私有化
    for (int i = 0; i < 1000; i++)
    {
        tmp = a[i] / b[i];
        usleep(1);
        c[i] = tmp * tmp;
    }

    for (int i = 0; i < 1000; i++)
    {
        tmp = a[i] / b[i];
        usleep(1);
        d[i] = tmp * tmp;
        if (c[i] != d[i]) std::cout << "出错啦！！！\n";  //
    }


    return 0;
}
