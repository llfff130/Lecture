#include<stdio.h>
#include<omp.h>
int main(){
	int num_threads=4;
	omp_set_num_threads(num_threads);
	#pragma omp parallel
	{
        // 获取当前线程的ID
        int tid = omp_get_thread_num();
        printf("Hello World from thread = %d\n", tid);
	}
	return 0;
}
