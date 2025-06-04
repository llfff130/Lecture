#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#define N 100
int main(){
	double A[N][N],B[N][N],C[N][N];
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			A[i][j]=i+j;
			B[i][j] = i - j;
		}
	}
	double start_time = omp_get_wtime(); // 记录开始时间
	#pragma omp parallel for
	for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
   	}
	double end_time = omp_get_wtime();
	printf("Matrix multiplication took %f seconds\n",end_time-start_time);
	return 0;
}
