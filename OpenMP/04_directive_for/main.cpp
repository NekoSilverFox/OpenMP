#include <iostream>
#include <omp.h>

const int NUM_THREADS = 4;

int main() {
    std::cout << "===== directive for 处理数组，后一个数等于前一个数的两倍=====" << std::endl;

    int arr[20];
    arr[0] = 1;

    /* 串行计算 */
    for (int i = 0; i < 20 - 1; i++)
    {
        arr[i + 1] = arr[i] * 2;
    }

    std::cout << "串行结果：\n";
    for (int i : arr)
    {
        std::cout << i << " ";  // 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288
    }

    /* 并行计算 */
    int arr2[20];
    arr2[0] = 1;
#pragma omp parallel for num_thread(NUM_THREADS)
    for (int i = 0; i < 20 - 1; ++i)
    {
        arr2[i + 1] = arr2[i] * 2;  // 一个或者更多个迭代结果依赖于其他迭代的循环，一般不能被正确的并行化
    }
    std::cout << "\n并行结果：\n";
    for (int i : arr2)
    {
        std::cout << i << " ";  // 1 2 4 8 16 32 64 128 256 512 0 0 0 0 0 0 0 0 0 0
    }

    return 0;
}
