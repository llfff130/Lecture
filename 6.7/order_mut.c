#include <stdio.h>
#include <stdlib.h>
#include<omp.h>
#define N 100
#define M 200

void matrix_multiply(double A[N][M], double C[N][N]) {
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < N; i++) {
        for (int j = i; j < N; j++) {  // 只计算上三角部分
            double sum = 0.0;
            for (int k = 0; k < M; k++) {
                sum += A[i][k] * A[j][k];
            }
            C[i][j] = sum;
            C[j][i] = sum;  // 利用对称性填充下三角部分
        }
    }
}

int main() {
    double A[N][M];
    double C[N][N] = {0};

    // 初始化矩阵 A
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            A[i][j] = (double)rand() / RAND_MAX;
        }
    }

    // 计算矩阵乘法
    matrix_multiply(A, C);

    // 打印结果矩阵 C（部分）
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%.2f ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
