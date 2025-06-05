#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000000000  // 数组大小
double sum_with_manual_reduction(double *array, int n) {
    double sum = 0.0;
    #pragma omp parallel
    {
        double local_sum = 0.0;
        #pragma omp for nowait
        for (int i = 0; i < n; i++) {
            local_sum += array[i];
        }
        // 使用临界区保护全局累加
        #pragma omp critical
        sum += local_sum;
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
    double sum1 = sum_with_manual_reduction(array, N);
    double end_time = omp_get_wtime();
    printf("归约子句方法:\n");
    printf("  计算结果 = %.0f (期望值 = %.0f)\n", sum1, expected_sum);
    printf("  耗时 = %.6f 秒\n", end_time - start_time);
    free(array);
    return 0;
}
