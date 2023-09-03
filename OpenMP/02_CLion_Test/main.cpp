#include <iostream>
#include <omp.h>

int main()
{
    std::cout << "----------- Hello, World! -----------" << std::endl;

#pragma omp parallel
    {
        // cout 不是原子操作，所以多线程会乱流。cout不是线程安全的，要靠自己去线程同步，比较麻烦 。
//        std::cout << "Hello world from thread [" << omp_get_thread_num() << "], " << "number of threads [" << omp_get_num_threads() << "]\n";


        // printf()函数是原子操作的，所以不会乱流。所以打印输出要使用 printf
        printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
    }

    return 0;
}
