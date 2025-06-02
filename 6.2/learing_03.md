# HUST-USYD 并行编程实践暑期学校 —— 第三讲

## 大纲
- 回顾作业1
- 加速比和效率
- 阿姆达尔定律和开销
- 一般设计流程
- 与机器无关的部分
  - 分区，通信/同步
  - 任务依赖图
  - 数学结合律
- 与机器有关的部分
  - 分配和负载平衡
- 实验室练习：带部分主元的高斯消元

## 加速比和效率
- **并行计算的目标**：高性能 —— 高速度
- **加速比**：并行算法相对于顺序算法的性能提升，定义为 ($ S = \frac{T_s}{T_p} $)，其中 ($ T_s $) 是单处理器计算时间，\( $T_p $\) 是 \( p \) 个处理器的计算时间。
- **效率**：并行算法中处理器的有用利用率，定义为 \($ E = \frac{S}{p} = \frac{T_s}{pT_p} $\)。

`使用 \( p \) 个处理器解决问题时的期望加速比`  
- **期望**：使用 \( p \) 个处理器解决问题时，我们期望得到 \( p \) 倍的加速比，即高效率。
- **现实**：可能不是这样！    

`在大多数并行程序中，除了计算本身外，还会引入额外的开销，这些开销包括：`
- **进程/线程通信或同步**：不同处理器或线程之间需要通信和同步，这会消耗时间和资源。
- **负载不平衡**：可用处理器或线程之间的负载分配不均，导致某些处理器或线程空闲，而其他处理器或线程过载。
- **额外工作**：为管理计算和增加并行性而引入的额外工作，如任务分配、数据分发等。
- **其他开销**：如内存访问延迟、上下文切换等。  

`开销的量化`
- **总开销**：\( $T_o = pT_p - T_s $\)，其中 
- **效率**：\($ E = \frac{T_s}{pT_p} = \frac{T_s}{T_s + T_o} = \frac{1}{1 + \frac{T_o}{T_s}} $\)
- **效率范围**：\( $0 \leq E \leq 1 $\)
- **开销影响**：当 \( $T_o $\) 较大时，\( E \) 会很小，表明并行程序的效率较低。

## 阿姆达尔定律和开销
### 阿姆达尔定律 ###
- 预测多处理器并行计算的理论加速比，表明串行部分限制了潜在加速比。
- 假设解决一个问题的总操作量可以分为两部分：`β:纯粹的串行部分    1−β :完全可并行化的`
    - 并行时间的计算     
    在并行计算中，使用 \( p \) 个处理器的并行时间 \( $T_p $\) 可以表示为：  
            \[ $T_p = \beta T_s + \frac{(1 - \beta) T_s}{p} $\]  
    - 加速比的计算  
    \[ $S = \frac{T_s}{T_p} = \frac{T_s}{\beta T_s + \frac{(1 - \beta) T_s}{p}} $\]

- **当 \( p \) 非常大时的加速比**  
当 \( p \) 非常大时，加速比 \( S \) 可以近似为：
\[$ S \approx \frac{1}{\beta} $\]
- **限制因素**：当 \( p \) 非常大时，加速比 \( S \) 的上限由 \( $\beta $\) 决定。

- **开销**：并行程序中除了计算外的额外工作，如通信、同步、负载不平衡等，会降低效率。

## 一般设计流程
### 与机器无关的部分
- **分区**：将大任务分解为多个小任务，以便并发执行。
  - **任务分区**：先分解计算，再关联数据。
  - **数据分区**：先分解数据，再关联计算。
- **通信/同步**：协调并发任务的执行，管理数据传输。以高效的方式组织通信可能具有挑战性
- **任务依赖图**：展示任务间的依赖关系，帮助识别并行结构。
- **数学结合律**：利用数学性质增加并行程度，如求和的结合律。

### 与机器有关的部分
- **分配和负载平衡**：根据机器特性分配任务，最小化开销并平衡工作负载。

## 实验室练习：带部分主元的高斯消元
### 高斯消元
- **目标**：通过行操作将矩阵转换为上三角矩阵。
- **算法**：
  ```markdown
  for i = 1 to n-1
      for j = i+1 to n
          tmp = A(j,i)
          for k = i to n
              A(j,k) = A(j,k) - (tmp/A(i,i)) * A(i,k)
  ```
### 带部分主元的高斯消元
- **问题**：当对角线元素 \( A(i,i) \) 很小时，可能导致数值不稳定。
- **解决方案**：部分主元法，交换行使 \( A(i,i) \) 尽可能大。
  ```markdown
  for i = 1 to n-1
      find and record k where |A(k,i)| = max_{i ≤ j ≤ n} |A(j,i)|
      if |A(k,i)| = 0
          exit with a warning that A is singular, or nearly so
      elseif k ≠ i
          swap rows i and k of A
      end if
      A(i+1:n,i) = A(i+1:n,i) / A(i,i)
      A(i+1:n,i+1:n) = A(i+1:n , i+1:n ) - A(i+1:n , i) * A(i , i+1:n)
  ```

## 实验室练习：循环展开
### 循环展开的目的
- 减少循环控制的开销，提高性能。

### 循环展开的实现
- **选择变量**：可以选择对 \(i\) 或 \(j\) 进行循环展开。
- **展开因子**：假设展开因子为 4。
- **代码示例**：
  ```markdown
  // 对 i 进行循环展开
  for (i = 0; i < n - 1; i += 4)
    {
        for (int ii = 0; ii < 4 && i + ii < n - 1; ii++)
        {
            // find and record k where |a(k,i+ii)|=max|a(j,i+ii)|
            amax = a[i + ii][i + ii];
            indk = i + ii;
            for (k = i + ii + 1; k < n; k++)
            {
                if (fabs(a[k][i + ii]) > fabs(amax))
                {
                    amax = a[k][i + ii];
                    indk = k;
                }
            }

            // exit with a warning that a is singular
            if (amax == 0)
            {
                printf("matrix is singular!\n");
                exit(1);
            }
            else if (indk != i + ii) // swap row i+ii and row k
            {
                for (j = 0; j < n; j++)
                {
                    c = a[i + ii][j];
                    a[i + ii][j] = a[indk][j];
                    a[indk][j] = c;
                }
            }

            // store multiplier in place of A(k,i+ii)
            for (k = i + ii + 1; k < n; k++)
            {
                a[k][i + ii] = a[k][i + ii] / a[i + ii][i + ii];
            }

            // subtract multiple of row a(i+ii,:) to zero out a(j,i+ii)
            for (k = i + ii + 1; k < n; k++)
            {
                c = a[k][i + ii];
                for (j = i + ii + 1; j < n; j++)
                {
                    a[k][j] -= c * a[i + ii][j];
                }
            }
        }
    }

  // 处理剩余的列
  for (i = n - (n % 4); i < n - 1; i++) {
    // find and record k where |a(k,i)|=max|a(j,i)|
        amax = a[i][i];
        indk = i;
        for (k = i + 1; k < n; k++)
        {
            if (fabs(a[k][i]) > fabs(amax))
            {
                amax = a[k][i];
                indk = k;
            }
        }

        // exit with a warning that a is singular
        if (amax == 0)
        {
            printf("matrix is singular!\n");
            exit(1);
        }
        else if (indk != i) // swap row i and row k
        {
            for (j = 0; j < n; j++)
            {
                c = a[i][j];
                a[i][j] = a[indk][j];
                a[indk][j] = c;
            }
        }

        // store multiplier in place of A(k,i)
        for (k = i + 1; k < n; k++)
        {
            a[k][i] = a[k][i] / a[i][i];
        }

        // subtract multiple of row a(i,:) to zero out a(j,i)
        for (k = i + 1; k < n; k++)
        {
            c = a[k][i];
            for (j = i + 1; j < n; j++)
            {
                a[k][j] -= c * a[i][j];
            }
        }
  }
  ```
- 编译：gcc -O3 gepp_0.c -o gepp
- 运行：./gepp 100
## 总结
- **加速比和效率**：理解并行算法的性能提升和处理器利用率。
- **阿姆达尔定律**：识别并行程序中的串行瓶颈。
- **设计流程**：从问题规范到并行算法的转换，包括分区、通信/同步、任务依赖图和数学结合律。
- **高斯消元**：通过行操作将矩阵转换为上三角矩阵，解决线性方程组。
- **循环展开**：优化循环性能，减少循环控制开销。
