#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000000  // 数组大小

// 串行扫描算法
void sequential_scan(double *A, double *C, int n) {
    C[0] = A[0];
    for (int i = 1; i < n; i++) {
        C[i] = C[i-1] + A[i];
    }
}
int main() {
    double *A = (double *)malloc(N * sizeof(double));
    double *C_serial = (double *)malloc(N * sizeof(double));
    // 初始化数组（示例：1, 1, 1, ..., 1）
    for (int i = 0; i < N; i++) {
        A[i] = 1.0;
    }

    // 串行扫描
    double start_time = omp_get_wtime();
    sequential_scan(A, C_serial, N);
    double end_time = omp_get_wtime();
    printf("串行扫描耗时: %.6f 秒\n", end_time - start_time);
    free(A);
    free(C_serial);
    return 0;
}
