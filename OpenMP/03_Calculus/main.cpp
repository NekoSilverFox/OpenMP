#include <iostream>
#include <chrono>
#include <omp.h>

/**
 * 实现计算一个积分，具体题目看 README 中的章节 4.6.1
 */

const long NUM_STEPS = 100000000;  // 微分矩形的个数
const double STEP = 1.0 / NUM_STEPS;  // 微分矩形的宽 △x

/**
 * 用以往的串行计算来计算积分
 */
void serial()
{
    auto time_start = std::chrono::system_clock::now();
    /***********************************************************************/

    double sum_height = 0.0;
    double x;
    double res_area;

    for (int i = 0; i < NUM_STEPS; i++)
    {
        x = i * STEP + STEP / 2;  // 每个小矩形中心点的横坐标
        sum_height += 4.0/(1.0 + x * x);  // 累计每个小矩形的高
    }

    res_area = STEP * sum_height;

    /***********************************************************************/
    auto time_end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
    printf("积分的结果 res_area = %f，用时 %f 秒\n",
           res_area, double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den);
}

/**
 * 通过 OpenMP 并行的方式计算
 */
#define NUM_THREAD 8
void openmp_parallel()
{
    auto time_start = std::chrono::system_clock::now();

    /*********************/
    double sum_height = 0.0;
    double tid_sum_area[NUM_THREAD];  // 注意：这里使用了数组
    double res = 0.0;

    /***********************************************************************/
     omp_set_num_threads(NUM_THREAD);
     int ntds_global;

#pragma omp parallel
    {
        double x;
        int i;
        int tid = omp_get_thread_num();  // tid 线程的编号
        int ntds_private = omp_get_num_threads();  // 获取线程数量
        for (i = tid, tid_sum_area[tid] = 0.0; i < NUM_STEPS; i += ntds_private)  // 每个线程单独去累加 ntds_private 间隔处的矩形
        {
            x = i * STEP + STEP / 2;  // 每个小矩形中心点的横坐标
            tid_sum_area[tid] += 4.0/(1.0 + x * x);  // 累计每个小矩形的高
        }

        if (0 == tid) ntds_global = ntds_private; // 【重点】
    };

    /* 累加每个线程（tid）上的面积，从而求得最终结果 */
    for (int i = 0; i < ntds_global; ++i)
    {
        res += tid_sum_area[i] * STEP;
    }
    /***********************************************************************/
    auto time_end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
    printf("积分的结果 res = %f，用时 %f 秒\n",
           res, double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den);
}

int main() {
    std::cout << "通过串行计算来计算积分：" << std::endl;
    serial();

    std::cout << "\n通过 OpenMP 并行的方式计算积分：" << std::endl;
    openmp_parallel();

    return 0;
}
