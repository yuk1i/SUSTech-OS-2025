# 多处理器编程

> 多处理器编程，从入门到放弃。

使用 `make` 编译所有源文件。

本实验包主要参照 https://jyywiki.cn/OS/2025/lect5.md 设计，我们推荐你观看 jyy 的网课 https://www.bilibili.com/video/BV1bGdpYbE2r 。

## 放弃1：单线程思维

在我们（开发人员）的视角下，程序总是顺序执行的。所以，我们会非常自然地写出 `alipay.c` 中的错误例子：在判断完 `money >= 0` 后，money 被其他线程设置为 0 了。

> code: alipay.c alipay2.c

## 放弃2：编译器也是单线程思维

> code: sum1.c

当编译器对代码进行优化时，它总是以单线程的模型对代码进行优化。

运行 `sum1_O0`, `sum1_O1`, `sum1_O2`，它们是用不同的优化等级编译相同的源文件 `sum1.c`，观察结果。

我们发现，`sum1` 在 `-O1` 和 `-O2` 下行为是不一致的。

编译器会按照**单线程模型**对代码进行优化。在编译器眼里，sum 只会在这个 for 循环内修改，既然是 for(N次) sum++，那么：

- O1：循环展开一下，`tmp = sum; tmp += N; for(N次) do_nothing(); sum = tmp;`
- O2：既然循环什么都没有干，那不如直接把循环删了，`sum += N`

O1:

```asm
0000000000001190 <T_sum>:
    1190:       48 8b 15 c9 2e 00 00    mov    rdx,QWORD PTR [rip+0x2ec9]           # rdx = [sum]
    1197:       48 8d 42 01             lea    rax,[rdx+0x1]                        # rax = rdx + 1
    119b:       48 81 c2 01 e1 f5 05    add    rdx,0x5f5e101                        # rdx += 0x5f5e101 (100000001)
    11a2:       66 66 2e 0f 1f 84 00    data16 cs nop WORD PTR [rax+rax*1+0x0]      # do nothing
    11a9:       00 00 00 00 
    11ad:       0f 1f 00                nop    DWORD PTR [rax]                      # do nothing
    11b0:       48 89 c1                mov    rcx,rax                              # loop begins, rcx = rax
    11b3:       48 83 c0 01             add    rax,0x1
    11b7:       48 39 d0                cmp    rax,rdx
    11ba:       75 f4                   jne    11b0 <T_sum+0x20>                    # loop: jump if not equal
    11bc:       48 89 0d 9d 2e 00 00    mov    QWORD PTR [rip+0x2e9d],rcx           # [sum] = rcx
    11c3:       c3                      ret
```

O2:

```asm
00000000000011d0 <T_sum>:
    11d0:       48 81 05 85 2e 00 00    add    QWORD PTR [rip+0x2e85],0x5f5e100        # [sum] += 0x5f5e100 (100000000)
    11d7:       00 e1 f5 05 
    11db:       c3                      ret
    11dc:       0f 1f 40 00             nop    DWORD PTR [rax+0x0]
```

此外，还有一种我们常写的bug：

> code: wait.c

```c
int flag = 0;

void T1() {
    usleep(100);
    flag = 1;
}

void T2() {
    while(flag == 0);
    printf("T2 got it.\n");
}
```

编译器是单线程生物，在 `T2` 中，编译器不会认为 `flag` 会被修改，因为在它眼里所有代码都是在单线程下执行的。

所以，编译器就能把这一个 `while(flag == 0)` 优化为：

```c
    if (flag == 0) while(1);
```

这也是为什么 `wait` 会卡死。

### Solution: 

1. `volatile` 关键字修饰变量：编译器不要对 Load/Store 优化，编译器在每次读写该变量时真的会产生汇编指令对其进行读写。
2. `asm volatile("":::"memory")`：告诉编译器这一行后，世界发生了变化，之前所有的假设全部不再成立了。

## 放弃3：CPU 乱序执行、Memory Ordering

在 `sum1.c` 中，如果直接使用 `sum++`，gcc (-O0) 会生成三条指令：

```assembly
    1335:       48 8b 05 24 2d 00 00    mov    rax,QWORD PTR [rip+0x2d24]       # rax = [sum]
    133c:       48 83 c0 01             add    rax,0x1                          # rax += 1
    1340:       48 89 05 19 2d 00 00    mov    QWORD PTR [rip+0x2d19],rax       # [sum] = rax
```

已知会发生 data race，`%rax` 里面的值可能是错的！

在 `sum2.c` 中，我们使用 `单条指令` `incq` 来对 `sum` 这个地址进行自增。

运行 `sum2`，我们发现结果还是错的。但是在 `sum3.c` 中，我们将两个线程绑定到同一个核心 (`bindtocpu`)，结果就正确了。

这是因为 CPU 在执行单条指令 `incq`，在底层电路上还是分三步操作：`load inc store`。

### CPU 的奇怪行为2: Load-Store 观测顺序，Memory Ordering

> code: ab.c

```c
int x = 0;
int y = 0;

void T1() {
    x = 1;
    print(y);
}

void T2() {
    y = 1;
    print(x);
}
```

理论上来说，不论按照什么执行顺序，都不可能出现 `0 0` 的结果。但是，它确实会发生。

```shell
$ gcc -O2 ab.c && ./a.out | head -c 1000000 | sort | uniq -c
    875 0 0 
 174215 0 1 
  24898 1 0 
     12 1 1 
```

现代 CPU 会采用乱序执行，以减少流水线停顿 (stall)。

T1: Store(X), Load(y) 这两步操作是没有依赖关系的！（回顾计组课上的流水线冲突：RAR，RAW，WAW）
所以，Store 还未完成时（或者因为缓存缺失而等待），Load 操作已经发出了。

**总结**

在多处理器编程上，我们要额外考虑三种情况：

1. 人是一种单线程生物。单线程思维不再一定正确了，共享变量有可能在任何时刻被别人更改。
2. 编译器也是一种单线程生物，编译器在做优化时假定只有当前在运行的代码会修改变量状态。
3. CPU 是一种复杂生物，它在内部会对指令进行拆分、乱序执行。我们对共享变量的观察、修改顺序，不一定是其他线程看到的顺序。

**Takeaway**：不要自己写聪明的同步算法，大概率是错的。
