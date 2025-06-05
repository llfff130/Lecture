#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000000000  // 数组大小 第二个1e9
double sum_with_reduction(double *array, int n) {
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += array[i];
    }
    return sum;
}

int main() {
    double *array = (double *)malloc(N * sizeof(double));
    if (array == NULL) {
        fprintf(stderr, "内存分配失败\n");
        return 1;
    }
    // 初始化数组（示例：1, 2, 3, ..., N）
    for (int i = 0; i < N; i++) {
        array[i] = i + 1;
    }
    // 计算理论上的正确和（用于验证）
    double expected_sum = (double)N * (N + 1) / 2;
     double start_time = omp_get_wtime();
    double sum1 = sum_with_reduction(array, N);
    double end_time = omp_get_wtime();
    printf("归约子句方法:\n");
    printf("  计算结果 = %.0f (期望值 = %.0f)\n", sum1, expected_sum);
    printf("  耗时 = %.6f 秒\n", end_time - start_time);
    free(array);
    return 0;
}
