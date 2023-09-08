#include <iostream>
#include <omp.h>


int main() {
    std::cout << "`reduction` 指定在并行区域结束时，对每个线程私有的一个或多个变量进行还原操作" << std::endl;

    int num = 10;
    std::cout << "并行外部开始处 num = " << num << "\n";  // 10

    #pragma omp parallel num_threads(4) reduction(+: num)  // 根据规则，这里每个线程的私有 num 将被初始化为 0
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                printf("第 1 个 section 开始处 num = %d\n", num);  // 0
                num = num + 5;
                printf("第 1 个 section 结束处 num = %d，由 tid [%d] 线程处理\n", num, omp_get_thread_num());  // 5
            }

            #pragma omp section
            {
                printf("第 2 个 section 开始处 num = %d\n", num);  // 0
                num = num + 11;
                printf("第 2 个 section 结束处 num = %d，由 tid [%d] 线程处理\n", num, omp_get_thread_num());  // 11
            }
        }
    }

    std::cout << "并行外部结束 num = " << num << "\n";  // 10+5+11=26
    return 0;
}
