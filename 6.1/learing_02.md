# 并行编程实践讲座笔记 - 第二讲

## 内存层次结构
- 计算机内存系统由寄存器、缓存、主存和辅助存储器组成层次结构
- 特点：
  - 上层：低延迟、高带宽、小容量
  - 下层：高延迟、低带宽、大容量
  ```bash
    寄存器访问：~1个时钟周期（<1ns）
    L1缓存访问：~3-4个时钟周期（~1ns）
    L2缓存访问：~10-20个时钟周期（~3-7ns）
    L3缓存访问：~40-75个时钟周期（~15-25ns）
    主内存访问：~100-300个时钟周期（~60-100ns）
  ```
- 程序通常表现出**高度局部性(Data Locality)：**
  - **空间局部性**：访问邻近之前访问过的数据
  - 时间局部性：重用之前访问过的数据项

## 计算强度
### 核心公式解析
$$
\text{实际时间} = f \times t_f + m \times t_m = f \times t_f \times \left(1 + \frac{t_m}{t_f} \times \frac{1}{q}\right) \
$$
其中：
- \( f \)：浮点运算次数 (FLOPs)
- \( t_f \)：每次浮点运算时间
- \( m \)：慢速内存访问次数
- \( t_m \)：每次慢速内存访问时间
- \($ q = \frac{f}{m} $\)：计算强度（每次内存访问对应的浮点运算数）**计算强度 = 算术操作次数 / 内存访问次数**


### 2. 机器平衡点公式
\[ \text{机器平衡} = \frac{t_m}{t_f} \]

现代处理器典型值：
| 平台 | t_m (cycles) | t_f (cycles) | 所需最小q |
|------|--------------|--------------|-----------|
| CPU  | 200          | 1            | ≥200      |
| GPU  | 400          | 4            | ≥100      |


## 矩阵乘法优化
**基本定义**
**给定两个矩阵 A(m×k) 和 B(k×n)，它们的乘积 C = A × B 是一个 m×n 的矩阵，其中:**
### 基础版本(ijk顺序)
```c
for (i=0; i<n; i++)
  for (j=0; j<n; j++)
    for (k=0; k<n; k++)
      C[i][j] += A[i][k] * B[k][j];
```
- 计算强度q≈2（对于大n）
- 这种实现简单明了，但性能较差，主要原因是没有充分利用缓存。特别是对矩阵B的访问是按列进行的，违反了内存的连续访问模式

### 优化版本(ikj顺序)
```c
for (i = 0; i < NRA; i++) {
    for (k = 0; k < NCA; k++) {
        for (j = 0; j < NCB; j++) {
            c[i][j] += a[i][k] * b[k][j];
        }
    }
}
```
- 利用连续内存访问（行主序存储）
- 计算强度提高到q≈2L（L为缓存行大小）
```bash
             ijk   ikj
100 100 1000:0.3 -> 0.2
1024 1024 1024:3.852935->3.320894
-lrt 链接函数
```
![test]()
### 性能对比  
不同循环顺序的性能对比如下（以ijk顺序为基准）：

|循环顺序	|相对性能	|空间局部性|
|-------|-------------|--------|
|ijk	|1.0x	|中|
|ikj	|~1.5-2x	|高|
|jik	|~0.9x	|中|
|jki	|~0.9x	|中|
|kij	|~1.3x	|高|
|kji	|~0.8x	|低|
## 分块技术 (Blocking) ##
- 将矩阵划分为b×b的子块
- 计算强度q≈b（块大小）
- 性能随块大小增加而提高（只要3b² < 快速内存大小）
```c
//// BLOCK_SIZE是块大小（通常选择适合缓存大小的值，如32或64）
 for (int bi = 0; bi < N; bi += BLOCK_SIZE)
        for (int bj = 0; bj < N; bj += BLOCK_SIZE)
            for (int bk = 0; bk < N; bk += BLOCK_SIZE)
                for (int i = bi; i < bi + BLOCK_SIZE && i < N; i++)
                    for (int j = bj; j < bj + BLOCK_SIZE && j < N; j++) {
                        double sum = c[i][j];
                        for (int k = bk; k < bk + BLOCK_SIZE && k < N; k++)
                                sum += a[i][k] * b[k][j];
                        c[i][j] = sum;
                    }
```
## 循环展开 (Loop Unrolling) ##
例如，将这样的循环
```c
for (i = 0; i < N; i++)
    a[i] = b[i] + c[i];
```
展开为：
```c
for (i = 0; i < N; i += 4) {
    a[i] = b[i] + c[i];
    a[i+1] = b[i+1] + c[i+1];
    a[i+2] = b[i+2] + c[i+2];
    a[i+3] = b[i+3] + c[i+3];
}
```
应用于矩阵乘法(**+4**)
- 循环展开的最佳展开因子取决于硬件平台、编译器优化能力和具体问题，通常为2、4或8。
```c
for (i=0; i<n; i+=4){
  y0 = y[i]; y1 = y[i+1]; y2 = y[i+2]; y3 = y[i+3];
  for (k=0; k<n; k++) {
    x0 = x[k];
    y0 += A[i][k]*x0; y1 += A[i+1][k]*x0;
    y2 += A[i+2][k]*x0; y3 += A[i+3][k]*x0;
  }
  y[i]=y0; y[i+1]=y1; y[i+2]=y2; y[i+3]=y3;
}
```
## 实验 ##
### 任务2：矩阵-向量乘法的循环展开优化
```c
//外层循环i的循环展开（Unrolling Factor = 4）
for (i = 0; i < NRA - 3; i += 4) {
    for (k = 0; k < NCA; k++) {
      b[i] += a[i][k] * x[k];
      b[i+1] += a[i+1][k] * x[k];
      b[i+2] += a[i+2][k] * x[k];
      b[i+3] += a[i+3][k] * x[k];
    }
  }

  // Handle the remaining rows
  for (; i < NRA; i++) {
    for (k = 0; k < NCA; k++) {
      b[i] += a[i][k] * x[k];
    }
  }

  //内层循环k的循环展开（Unrolling Factor = 4）
  for (i = 0; i < NRA - 3; i += 4) {
    // Loop unrolling for inner loop k with factor 4
    for (k = 0; k < NCA - 3; k += 4) {
      b[i] += a[i][k] * x[k] + a[i][k+1] * x[k+1] + a[i][k+2] * x[k+2] + a[i][k+3] * x[k+3];
      b[i+1] += a[i+1][k] * x[k] + a[i+1][k+1] * x[k+1] + a[i+1][k+2] * x[k+2] + a[i+1][k+3] * x[k+3];
      b[i+2] += a[i+2][k] * x[k] + a[i+2][k+1] * x[k+1] + a[i+2][k+2] * x[k+2] + a[i+2][k+3] * x[k+3];
      b[i+3] += a[i+3][k] * x[k] + a[i+3][k+1] * x[k+1] + a[i+3][k+2] * x[k+2] + a[i+3][k+3] * x[k+3];
    }

    // Handle the remaining columns for the current 4 rows
    for (; k < NCA; k++) {
      b[i] += a[i][k] * x[k];
      b[i+1] += a[i+1][k] * x[k];
      b[i+2] += a[i+2][k] * x[k];
      b[i+3] += a[i+3][k] * x[k];
    }
  }
  for (; i < NRA; i++) 
    for (k = 0; k < NCA; k++) {
      b[i] += a[i][k] * x[k];
    }

```
- 编译：gcc -O3 matvec_0.c -o matvec_original
- 运行：./matvec_original 1024 1024
### 实验3 ##
性能比较柱状图
| 矩阵大小 | 循环展开优化运行时间 |  未优化运行时间|
|----------|----------------|----------------------|
| 100x100  | 0.000002   | 0.000002       |
| 200x200  | 0.000007   | 0.000008  |
| 500x500  | 0.000044   | 0.000060       |
| 1000x1000| 0.000188   |  0.000340        |