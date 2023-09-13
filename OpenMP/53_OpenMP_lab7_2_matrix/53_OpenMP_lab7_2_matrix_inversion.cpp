/** -*- coding: utf-8 -*-
 * @Time    : 2023/9/12 00:22
 * @Author  : 冰糖雪狸 | Мэн Цзянин 5140904/30202
 * @Project : 53_OpenMP_lab7_2_matrix_inversion
 * @File    : 53_OpenMP_lab7_2_matrix_inversion.cpp
 * @Software: CLion
 * @Github  ：https://github.com/NekoSilverFox
 * -----------------------
 *
 * Найти обратную матрицу для матрицы А. Входные данные: целое положительное число n, произвольная матрица А размерности n х n.
 * Решить задачу двумя способами:
 *  1) количество потоков является входным параметром, при этом размерность матриц может быть не кратна количеству потоков;
 *  2) количество потоков заранее неизвестно и не является параметром задачи.
 *
 **/

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
using namespace std;

const int RANDOM_MAX = 10;
static int num_threads = omp_get_max_threads();


/**
 * Print the matrix
 * @param matrix ptr of matrix
 * @param n n of matrix
 */
void printMatrix(const double* matrix, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%.3f  ", *(matrix + i*n + j));
        }
        printf("\n");
    }
}


/**
 * Generate n * n random matrices (Use OpenMP)
 * @param n
 * @return ptr of random matrices
 */
double* getRandomMatrix(const int n)
{
    double* matrix = new double[n*n];
    srand((int)time(0));

#pragma omp parallel for num_threads(::num_threads)
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrix[n*i + j] = rand() % RANDOM_MAX;
        }
    }

    return matrix;
}


/**
 * matrix transposition (Use OpenMP)
 * @param row row of matrix
 * @param col col of matrix
 * @param matrix ptr of matrix
 * @return result of matrix transposition
 */
double* matrixT(int row,int col, double* matrix)
{
    double* result = new double[row*col];

#pragma omp parallel for num_threads(::num_threads)
    for (int i = 0; i < row*col; i++)
    {
        int x = i + 1;
        int rowMat = 1;
        int colMat = 0;

        while (true)
        {
            if (x - col > 0)
            {
                x = x - col;
                rowMat++;
            }
            else
            {
                colMat = x;
                break;
            }
        }
        result[(colMat - 1)*row + rowMat - 1] = matrix[i];
    }

    return result;
}


/**
 * Determine the determinant of matrix (Use OpenMP)
 * @param n
 * @param matrix
 * @return det
 */
double det(int n, double* matrix)
{
    if (1 == n) return matrix[0];

    double sum = 0.0;
    double* sub_matrix = new double[(n - 1) * (n - 1)];

#pragma omp parallel for schedule(dynamic, n) reduction(+: sum)
    for (int row = 0; row < n; row++)
    {
        for (int sub_row_matrix = 0; sub_row_matrix < n - 1; sub_row_matrix++)
        {
            int mov = row > sub_row_matrix ? 0 : 1;
            for (int j = 0; j < n - 1; j++)
            {
                sub_matrix[sub_row_matrix*(n - 1) + j] = matrix[(sub_row_matrix + mov)*n + j + 1];
            }
        }
        int flag = (row % 2 == 0 ? 1 : -1);
        sum += flag* matrix[row*n] * det(n - 1, sub_matrix);
    }
    delete[] sub_matrix;
    
    return sum;
}


int main(int argc, char** argv)
{
    printf(">>> Лаб7.2 Мэн Цзянин 5140904/30202\n");

    /* Check parameters */
    if (2 != argc)
    {
        printf("[ERROR] The program does not have enough parameters: "
               "parameter 1 should be number threads.");
        return -1;
    }

    ::num_threads = atoi(argv[1]);
    if (num_threads > omp_get_max_threads()) num_threads = omp_get_max_threads();

    printf("Number threads: %d\n", num_threads);

    int n;
    printf("Please INPUT the dimension of the matrix `n` = ");
    std::cin >> n;
    if (n < 1)
    {
        printf("[ERROR] n should >= 1\n");
        return -1;
    }

    double* matrix_a = getRandomMatrix(n); // source matrix
    double* sub_matrix = new double[(n - 1)*(n - 1)];
    double* adjoint_matrix = new double[n*n];
    double* inverse_matrix = new double[n*n];

    /* Generate matrix */
    printf("\nSource random matrix A:\n");
    printMatrix(matrix_a, n);

    /* get det matrix */
    double det_matrix = det(n, matrix_a);
    if (0 == det_matrix)
    {
        printf("[ERROR] The matrix is not invertible!\n");
        return -1;
    }
    printf("\ndef matrix: %f\n", det_matrix);


    /* adjugate matrix T */
#pragma omp single
    {
        int i_adjoint_matrix = 0;
        for (int i_mat = 1; i_mat <= n; i_mat++)
        {
            for (int j_mat = 1; j_mat <= n; j_mat++)
            {
                int i_matrix = 0;

                /* cosine */
                for (int i = 0; i < n*n; i++)
                {
                    int x = i + 1;
                    int row = 1;
                    int col = 0;
                    while (true)
                    {
                        if (x - n > 0)
                        {
                            x = x - n;
                            row++;
                        }
                        else
                        {
                            col = x;
                            break;
                        }
                    }
                    if (row != i_mat && col != j_mat)
                    {
                        sub_matrix[i_matrix] = matrix_a[i];
                        i_matrix++;
                    }
                }
                adjoint_matrix[i_adjoint_matrix] = (i_adjoint_matrix % 2 == 0 ? 1 : -1)*det(n - 1, sub_matrix);//求伴随矩阵各元素
                i_adjoint_matrix++;
            }
        }
    }

    /* adjugate matrix */
    adjoint_matrix = matrixT(n, n, adjoint_matrix);
    printf("\nThe adjugate matrix of this matrix is:\n");
    printMatrix(adjoint_matrix, n);

#pragma omp parallel for
    for (int i = 0; i < n*n; i++)
    {
        inverse_matrix[i] = adjoint_matrix[i] / det_matrix;
    }

    printf("\n[RESULT] The inverse matrix is:\n");
    printMatrix(inverse_matrix, n);

    delete[] matrix_a;

    return 0;
}
