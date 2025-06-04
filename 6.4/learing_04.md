# HUST-USYD 夏季学校并行编程实践课程 – 第4讲 #
## 大纲 ##
- **编程共享内存平台**
- **示例：矩阵乘法**
    - 矩阵划分
    - 数据局部性
- **OpenMP**
    - 执行模型
    - 一般代码结构
    - 基本omp构造和指令
        - 并行区域构造
            - 示例：Hello World
            - 实验1：Hello World
        - 工作共享构造：for指令
            - 示例：矩阵-向量乘法
            - 实验2：矩阵乘法
- **详细解释**

## 共享内存平台编程 ##
- 多个线程被创建并同时运行。一组全局变量被所有线程共享。线程也有自己的局部私有变量。线程通过共享变量隐式通信。线程通过共享变量上的同步显式协调。
- 共享内存机器通常规模较小（例如，大型机器约有100个核心）。
- 需要粗粒度并行化。
- 多线程技术用于共享内存编程：
    - Pthread：显式创建线程并同步线程执行。
    - OpenMP：使用编译器指令和子句——本课程将学习OpenMP。
- 数据划分和数据局部性：很重要，但数据分配给线程通常是隐式的。
- 显式同步：在共享内存机器中，全局数据被多个线程共享。保护对共享数据的访问，从而防止竞争条件施加顺序约束。
### 线程基础 ###
- 线程与进程：一个进程可以包含多个线程，这些线程共享相同的地址空间和进程状态，
- 线程同步问题：
### 竞争条件 ###
定义：当多个线程读写共享数据项时，如果没有同步机制，最终结果将不可预测，这就是竞争条件。
## 示例：矩阵乘法 ##
- **任务依赖图**：矩阵乘法的任务依赖图显示所有输出元素可以并发计算，但这是细粒度并行。
- **粗粒度并行**：对于共享内存机器，需要更粗粒度的并行。可以通过对输出矩阵C进行分区来实现，常见的分区方式包括1D循环分区、1D块分区和1D块循环分区。
```bash
1D循环划分：行以循环方式分配给线程，直到所有行都被分配完毕。
1D块划分:行首先被分组成块，然后每个块被分配给一个线程。
1D块循环划分：行首先被分组成块。块的数量远多于线程的数量。块以循环方式分配给线程。
```
## OpenMP“Open Multi-Processing” ##
```bash
#pragma omp构造[子句[子句…]]
#include <omp.h>
动机:开发一个并行编译器，自动将顺序程序转换为并行程序
功能：允许程序员将程序划分为串行区域和平行区域，而不是显式创建并发执行的线程。隐藏堆栈管理。提供一些同步构造。
```
### 一般代码结构(c/c++) ###
```c++
#include <omp.h>
main() {
    int var1, var2, var3;
    // 串行代码
    // 并行区域的开始。分叉一个线程团队。
    // 指定变量作用域
    #pragma omp parallel private(var1, var2) shared(var3)
    {
        // 并行区域由所有线程执行
        // 其他OpenMP指令
        // 运行时库调用
    } // 所有线程与主线程会合并解散
    // 恢复串行代码
}
```
### 指令、函数或子句 ###
| OpenMP指令、函数或子句                                  | 概念                 
| ----------------------------------------------- | ------------------- | 
|                                                 |                     |                           
| #pragma omp parallel                            | 并行区域、线程团队、结构化、跨线程 |               
|                                                 |                     |                       
| int omp\_get\_thread\_num()                     | 线程ID            |      
| int omp\_get\_num\_threads()                    | 线程数量               |      
|                                                 |                     |                        
| double omp\_get\_wtime()                        | 加速和阿姆达尔定律           |      
|                                                 |                     |                           
| setenv OMP\_NUM\_THREADS N                      | 内部控制变量 、设置默认值                      | 
|                                                 |                     |                           
| #pragma omp barrier                             | 同步和竞争条件       |               
| #pragma omp critical                            |                     |                           
|                                                 |                     |                        
| #pragma omp for                                 | 工作共享、并行循环、循环携带依赖    |        
| #pragma omp parallel for                        |                     |                            
|                                                 |                     |                           
| reduction(op:list)                              | 跨线程团队的值的归约          |                     
|                                                 |                     |                          
| schedule(dynamic\[,chunk])                      | 循环调度、循环开销和负载平衡      |            
| schedule(static\[,chunk])                       |                     |                            
|                                                 |                     |                           
| private(list), firstprivate(list), shared(list) | 数据环境                |                           
|                                                 |                     |                           
| nowait                                          | 禁用工作共享构造上的隐式屏障   |
|                                                 |                     |                          
| #pragma omp single                              | 带有单线程的工作共享          |                           
|                                                 |                     |                           
| #pragma omp task                                | 任务，包括任务的数据环境        |                        
| #pragma omp taskwait                            |                     |                          
- **并行区域构造**
```bash
当线程到达并行指令时，它创建一个线程团队并成为该团队的主控线程。
主控线程是该团队的成员，在该团队内其线程编号（或ID）为0。
从这个并行区域的开始，所有线程将执行区域内的相同代码。
在并行部分的末尾有一个隐式屏障。
只有主控线程会继续执行这一点之后的代码。
创建多少线程:实现默认值——通常是节点上的CPU或核心数量。
#pragma omp parallel num_threads(3)  设置线程数量
创建的线程编号从0（主控线程）到nthreads-1。
使用函数omp_get_thread_num()获取每个线程ID。
```
- **示例：Hello World**
```c
#include <omp.h>
#include <stdio.h>
int main() {
    // 并行区域，默认线程数量
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        printf("Hello World from thread = %d\n", tid);
    }
}
```
## 工作共享构造(for指令) ##
- for指令指定紧随其后的循环的迭代必须由线程团队并行执行。
- 默认情况下，循环控制索引i对每个线程都是“私有的”。
- for指令必须位于并行区域内。
- 线程会在此处等待，直到所有线程完成并行for循环，然后才继续执行循环之后的代码。
```c
// 顺序代码
for (i = 0; i < N; i++) {
    a[i] = a[i] + b[i];
}

// OpenMP并行区域
int id, i, Nthrds, istart, iend;
#pragma omp parallel
{
    id = omp_get_thread_num();
    Nthrds = omp_get_num_threads();
    istart = id * N / Nthrds;
    iend = (id + 1) * N / Nthrds;
    if (id == Nthrds - 1) iend = N;
    for (i = istart; i < iend; i++) {
        a[i] = a[i] + b[i];
    }
}

// 默认情况下，OpenMP并行区域和工作共享for构造
#pragma omp parallel for
for (i = 0; i < N; i++) {
    a[i] = a[i] + b[i];
}
```
## 详细解释 ##
1. 粗粒度并行化与细粒度并行化
    - 细粒度并行化：这种并行化方式将任务分解为非常小的子任务，每个子任务的执行时间相对较短。
    - 粗粒度并行化：与细粒度并行化不同，粗粒度并行化将任务分解为较大的子任务，每个子任务的执行时间相对较长。对于共享内存机器，粗粒度并行化更为合适，因为它减少了线程创建和管理的开销，同时减少了线程之间的通信和同步需求。
2. 同步机制与竞争条件:为了防止竞争条件，需要使用同步机制。可以使用互斥锁（mutex）或其他同步原语来保护共享资源的访问。
```c
#pragma omp critical
{
    count++;
}
```
3. istart 和 iend 的划分
```bash
istart = id * N / Nthrds;
iend = (id + 1) * N / Nthrds;
if (id == Nthrds - 1) iend = N;
```
- 一个长度为N的任务划分为多个线程来执行。id 是当前线程的编号，Nthrds 是线程总数。
```bash
例如，如果有4个线程（Nthrds = 4），任务总数为100（N = 100）：
线程0：istart = 0, iend = 25，处理索引0到24的任务。
线程1：istart = 25, iend = 50，处理索引25到49的任务。
线程2：istart = 50, iend = 75，处理索引50到74的任务。
线程3：istart = 75, iend = 100，处理索引75到99的任务。
```

4. 循环携带依赖
```bash
int i, j, A[MAX];
#pragma omp parallel for
for (i = 0; i < MAX; i++) {
    j += 2;
    A[i] = big(j);
}
```
- 在这个例子中，j 是一个累加变量，每次循环都会更新它的值。由于j的值在每次迭代中都会改变，因此存在循环携带依赖。这意味着后续迭代的计算依赖于前一次迭代的结果，因此不能并行化。
- 为了避免这种依赖，可以将j的计算改为不依赖于前一次迭代的结果：int j = 5 + 2 * (i + 1);