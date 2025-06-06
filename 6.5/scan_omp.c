#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000000  // 数组大小
void parallel_scan(double *A, double *C, int n) {
    int num_threads = omp_get_max_threads();
    double *W = (double *)calloc(num_threads + 1, sizeof(double));  // 存储每个块的最后一个值

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int chunk_size = n / num_threads;
        int start = tid * chunk_size;
        int end = (tid == num_threads - 1) ? n : start + chunk_size;

        // 阶段1：计算局部扫描和，并存储最后一个值到 W[tid+1]
        C[start] = A[start];
        for (int i = start + 1; i < end; i++) {
            C[i] = C[i-1] + A[i];
        }
        W[tid + 1] = C[end - 1];  // 存储最后一个值

        // 阶段2：单线程计算 W 的扫描和
        #pragma omp barrier  // 确保所有线程完成阶段1
        #pragma omp single
        {
            for (int i = 1; i <= num_threads; i++) {
                W[i] += W[i-1];
            }
        }

        // 阶段3：每个线程将 W[tid] 加到局部扫描结果
        if (tid != 0) {  // 第一个块不需要调整
            for (int i = start; i < end; i++) {
                C[i] += W[tid];
            }
        }
    }

    free(W);
}
int main() {
    double *A = (double *)malloc(N * sizeof(double));
    double *C_parallel = (double *)malloc(N * sizeof(double));

    // 初始化数组（示例：1, 1, 1, ..., 1）
    for (int i = 0; i < N; i++) {
        A[i] = 1.0;
    }
   double start_time = omp_get_wtime();
    parallel_scan(A, C_parallel, N);
   double  end_time = omp_get_wtime();
    printf("并行扫描耗时: %.6f 秒\n", end_time - start_time);

    free(A);
    free(C_parallel);
    return 0;
}
