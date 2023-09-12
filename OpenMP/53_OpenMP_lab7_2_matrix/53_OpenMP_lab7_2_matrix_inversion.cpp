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

    int mov = 0;
    double sum = 0.0;
    double* sub_matrix = new double[(n - 1) * (n - 1)];

    for (int row = 0; row < n; row++)
    {
        for (int sub_row_matrix = 0; sub_row_matrix < n - 1; sub_row_matrix++)
        {
            mov = row > sub_row_matrix ? 0 : 1; //subMat中小于Matrow的行，同行赋值，等于的错过，大于的加一
            for (int j = 0; j<n - 1; j++)  //从Mat的第二列赋值到第n列
            {
                sub_matrix[sub_row_matrix*(n - 1) + j] = matrix[(sub_row_matrix + mov)*n + j + 1];
            }
        }
        int flag = (row % 2 == 0 ? 1 : -1);  //因为列数为0，所以行数是偶数时候，代数余子式为1.
        sum += flag* matrix[row*n] * det(n - 1, sub_matrix);//Mat第一列各元素与其代数余子式积的和即为行列式
    }
    delete[]sub_matrix;
    
    return sum;
}


int main()
{
    printf(">>> Лаб7.2 Мэн Цзянин 5140904/30202\n");

    int n = 3;
//    std::cout << "n = ";
//    std::cin >> n;


    double* matrix_a = getRandomMatrix(n); // source
    double* sub_matrix = new double[(n - 1)*(n - 1)];
    double* adjoint_matrix = new double[n*n];
    double* inverse_matrix = new double[n*n];

    /* Generate matrix */
    printf("Source random matrix A:\n");
    printMatrix(matrix_a, n);

    /* get det matrix */
    double det_matrix = det(n, matrix_a);
    if (0 == det_matrix)
    {
        printf("[ERROR] The matrix is not invertible!\n");
        return -1;
    }
    printf("\ndef matrix: %f\n", det_matrix);


    int i_adjoint_matrix = 0;  // index
#pragma omp parallel for num_threads(::num_threads)
    for (int i_mat = 1; i_mat <= n; i_mat++)
    {
        for (int j_mat = 1; j_mat <= n; j_mat++)
        {
            int i_matrix = 0;

            /* cosine */
            for (int i = 0; i < n*n; i++)
            {
                int x = i+1;
                int row= 1;
                int col= 0;
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
    
    
    adjoint_matrix = matrixT(n, n, adjoint_matrix);//转置
    cout << "该矩阵的伴随矩阵为" << endl;
    int adMat_index = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << adjoint_matrix[adMat_index] <<"\t";
            adMat_index++;
        }
        cout << endl;
    }
    for (int i = 0; i < n*n;i++)
        inverse_matrix[i] = adjoint_matrix[i] / det_matrix;

    cout << "该矩阵的逆矩阵为" << endl;
    int inverseMat_index = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << inverse_matrix[inverseMat_index]<<"\t";
            inverseMat_index++;
        }
        cout << endl;
    }

    delete[]matrix_a;



    return 0;
}
