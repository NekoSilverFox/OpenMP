<div align="center">
<p>
 <img width="100px" src="https://raw.githubusercontent.com/NekoSilverFox/NekoSilverfox/403ab045b7d9adeaaf8186c451af7243f5d8f46d/icons/silverfox.svg" align="center" alt="NekoSilverfox" />
 <p align="center"><b><font size=6>OpenMP</font></b></p>
 <p align="center"><b>高性能计算 - OpenMP 并行编程技术</b></p>
</p>



[![License](https://img.shields.io/badge/license-MIT-brightgreen)](LICENSE)



<div align="left">
<!-- 顶部至此截止 -->


<!-- SPbSTU 报告起始 -->

[toc]



# 并行编程简介

对于超级计算机等来说，基本并行编程概念的**颗粒度由小到大**：

1. **指令集并行** - CPU 流水线
2. **共享存储式并行** - OpenMP、OpenCL、OpenAcc
3. **分布式并行** - MPI（）
4. 

## 指令集并行 - CPU 流水线

> 参考：
>
> https://zhuanlan.zhihu.com/p/120134376

​	指令集并行 (Instruction-Level Parallelism，**ILP**)，一种[并行计算](https://zh.wikipedia.org/wiki/平行計算)形式，在一个[程序](https://zh.wikipedia.org/wiki/程式)运行中，许多[指令](https://zh.wikipedia.org/wiki/指令集架構)操作，能在同时间进行。它也是一个测量值，用来计算在一个[程序](https://zh.wikipedia.org/wiki/程式)运算中，它有**多少个[指令](https://zh.wikipedia.org/wiki/指令集架構)能够在同时间运算，称为==指令级并行度==**。实现指令层级，可以用[硬件](https://zh.wikipedia.org/wiki/硬體)或[软件](https://zh.wikipedia.org/wiki/軟體)方式来实现。硬件方式有[超标量](https://zh.wikipedia.org/wiki/超純量)。



**超标量执行（superscalar execution）：**

​	超标量（superscalar）CPU 架构是指在一颗处理器内核中实行了**指令级并行**的一类并行运算。这种技术能够在相同的CPU主频下实现更高的CPU吞吐率（throughput）。处理器的内核中一般有多个执行单元（或称功能单元），如算术逻辑单元、位移单元、乘法器等等。未实现超标量体系结构时，CPU在每个时钟周期仅执行单条指令，因此仅有一个执行单元在工作，其它执行单元空闲。超标量体系结构的CPU在一个时钟周期可以同时分派（dispatching）多条指令在不同的执行单元中被执行，这就实现了指令级的并行。超标量体系结构可以视作MIMD（多指令多数据）。超标量体系结构的CPU一般也都实现了指令流水化。但是一般认为这二者是增强CPU性能的不同的技术。第一种采用了超标量技术的X86处理器是Pentium。



经典的流水线模型，一条指令有以下步骤；而**每个步骤的执行都可以认为 CPU 中需要对应的元器件**：

- 指令取址--Instruction Fetch（IF）
- 指令译码--Instruction Decode（ID）
- 指令执行--Instruction Execution（EX）
- 访存--Memory Access（MEM）
- 写回--Write-Back（WB）

五级流水线，多条指令同时在CPU各部件执行

指令级并行--汇编排流水的并行方式



**并行示例：**

![image-20230824004205735](doc/pic/image-20230824004205735.png)

假设 CPU 中 IF、ID、EX、MEM、WB 都只有一个对应元器件

- 在图中，红色部分是单周期处理器，也就是没有进行并行计算的。可以看到，对于一条指令需要完成所有操作（IF、ID、ED...）之后才能够再处理下一条指令，这造成了时钟周期很长和 CPU 元器件的闲置
- 而对于蓝色部分便为采用了并行技术的流水线处理器：在一条指令的某个操作（比如 IF）完成后，CPU 会立即对下一条指令进行 IF 操作，而不是等待当前指令所有操作（IF、ID、ED...）都完成后再进行下一条指令的处理。显然，使用了并行之后避免了 CPU 资源的闲置，且提升了对于多条指令的处理速度











































































































































