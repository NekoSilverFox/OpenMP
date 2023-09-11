#include <stdio.h>
#include <math.h>
#include <omp.h>

#define N 1000

int main(int argc, char** argv)
{
    double b[N];
    double s = 0;

#pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        b[i] = i * sqrt(i);
        s += b[i];
    }

    printf("%f", s);
    return 0;
}
