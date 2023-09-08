#include <iostream>
#include <omp.h>

int main() {
    std::cout << "`sections` 将接下来的代码块包含将被并行执行的**`section`块**。`sections`在封闭代码的指定部分中，由线程组进行分配任务" << std::endl;

    #pragma omp parallel num_threads(3)
    {
        /* 每个 section块 都会由线程独立并行执行，如果一个线程执行速度足够快那么他可能会执行多个 section 块 */
        #pragma omp sections
        {
            #pragma omp section
            {
                int num_threads = omp_get_num_threads();
                int tid = omp_get_thread_num();
                printf("Hello from thread id [%d] of number threads [%d] in section block 1\n", tid, num_threads);
            }

            #pragma omp section
            {
                int num_threads = omp_get_num_threads();
                int tid = omp_get_thread_num();
                printf("Hello from thread id [%d] of number threads [%d] in section block 2\n", tid, num_threads);
            }

            #pragma omp section
            {
                int num_threads = omp_get_num_threads();
                int tid = omp_get_thread_num();
                printf("Hello from thread id [%d] of number threads [%d] in section block 3\n", tid, num_threads);
            }

            #pragma omp section
            {
                int num_threads = omp_get_num_threads();
                int tid = omp_get_thread_num();
                printf("Hello from thread id [%d] of number threads [%d] in section block 4\n", tid, num_threads);
            }
        }

    }


    return 0;
}
