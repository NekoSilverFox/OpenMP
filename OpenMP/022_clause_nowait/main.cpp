#include <iostream>
#include <unistd.h>
#include <omp.h>

int main() {
    std::cout << "nowait 的使用" << std::endl;

#pragma omp parallel num_threads(4)
    {
        int tid = omp_get_thread_num();

#pragma omp for /*nowait*/
        for(int i = 0; i < 10; i++)
        {
            sleep(2);
            printf("Hello from thread tid [%d], i = [%d]\n", tid, i);
        }  // 如果不使用 nowait，则所用线程都将再此等待至最后一个线程结束

#pragma omp single nowait
        {
            printf("thread tid [%d] process single\n", tid);
            sleep(3);
        };

        printf("thread tid [%d] finish\n");
    }
    return 0;
}
