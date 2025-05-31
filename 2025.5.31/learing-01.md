# 并行编程实践 - 第一讲笔记

## 课程目标
1. 掌握并行计算的基本概念  
2. 学习并行算法设计与实现方法  
3. 为未来深入学习和应用并行计算奠定基础  

## 并行计算概述
### 定义
- 使用多核计算机、集群或其他并行/分布式系统高速解决复杂计算问题  
- 性能单位：FLOPS（浮点运算/秒）  
  - 等级：Giga (10^9)、Tera (10^12)、Peta (10^15)、Exa (10^18)  
  - 单CPU峰值FLOPS计算公式：  
    $$
    \text{FLOPS} = \text{cores} \times \frac{\text{cycles}}{\text{second}} \times \frac{\text{floating-point operations}}{\text{cycle}}
    $$

### 核心问题
- 任务划分与分配  
- 进程/线程间协作  
- 性能评估  

## 并行计算机架构
### 1. 多核系统
- 特点：共享内存、小规模（约100核）  
- 示例：  
  ```plaintext
  [处理器] ↔ [片上缓存] ↔ [共享缓存] ↔ [全局内存]
  ```

### 2. 集群
- 特点：分布式内存、高度可扩展（如超级计算机）  
- 示例：全球最快超算"Frontier"  
  - 9,472节点，每节点含64核CPU + 4 GPU  
  - 总计606,208 CPU核心 + 37,888 GPU  

### 3. GPU加速器
- 架构：  
  - 流多处理器（SM） + 共享内存 + 张量核心（机器学习专用）  
  - 示例：NVIDIA RTX 4090  
    - 16,384核心 + 512张量核心 + 72MB L2缓存  

---

## 互联网络
### 关键类型
- **静态网络**：固定链路（如全连接网络，复杂度O(p²)）  
- **动态网络**：按需连接（如Crossbar交换机）  

### Frontier的Slingshot网络
- 拓扑：`Dragonfly（蜻蜓拓扑）`  
  - **优势**：低延迟（直径=3跳）、低成本、高可扩展性  
  - 层级：  
    1. 组内全连接（电链路）  
    2. 组间全连接（光链路）  

---

## 计算机分类（Flynn分类法）
| 指令流 \ 数据流 | 单数据流       | 多数据流        |
|------------------|----------------|-----------------|
| **单指令流**     | SISD（单核）   | SIMD（GPU/TPU） |
| **多指令流**     | MISD（罕见）   | MIMD（多核/集群）|

---

## 并行编程模型
1. **共享内存**：OpenMP  
2. **分布式内存**：MPI  
3. **SIMD**：CUDA（GPU编程，本课程仅简介）  

---

## 实验练习
- 平台：EduCoder（头歌）  
- 任务：  
  - **OpenMP**：编译运行`omp_hw.c` 
    ```c
    #include<stdio.h>
    #include<omp.h>
    int main(){
        #pragma omp parallel()
        {
        int thread_id=omp_get_thread_num();
        int num_threads=omp_get_num_threads();
        printf("Hello,openmp!from thread %d of total %d threads\n",thread_id,num_threads);
        }
        return 0;
    }
    ``` 
    ```bash
    gcc -fopenmp -o omp_hw omp_hw.c  
    ./omp_hw
    ```
  - **MPI**：编译运行`mpi_hw.c`  
    ```c
    #include<stdio.h>
    #include<mpi.h>
    int main(int argc,char** argv){
        int rank,size;
        MPI_Init(&argc,&argv);
         // 获取当前进程的rank（编号）
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
         // 获取总进程数
        MPI_Comm_size(MPI_COMM_WORLD, &size);
         printf("Hello from MPI process %d of %d\n", rank, size);
         MPI_Finalize();
         return 0;
    }
    ```
    ```bash
    mpicc -o mpi_hw mpi_hw.c  
    mpirun -np 4 mpi_hw  # 启动4进程
    ```
