#ifndef _MMU_H_
#define _MMU_H_

#include "types.h"
/*
如下是官方原版注释:
This file contains(该文件包含了):
Part1. MIPS definitions
Part2. Our conventions(惯例)
Part3. Our helper functions.
*/

/*
Part1. MIPS definitions
*/

#define BY2PG 4096              // 定义了页大小
#define PDMAP (4 * 1024 * 1024) // (每个页目录映射的字节大小？)
#define PGSHIFT 12
#define PDX(va) ((((u_long)(va)) >> 22) & 0x03ff) // 将虚拟地址的高10位&0x03ff(其实就是为了获取虚拟地址的高10位,获取虚拟地址的页目录索引)
#define PTX(va) ((((u_long)(va)) >> 12) & 0x03ff) // 获取虚拟地址的中间10位(虚拟地址的页表索引)
#define PTE_ADDR(pte) ((u_long)(pte) & ~0xfff)    // 清空低12位

// 官方:page number field of address
// 按道理va传入的应该是物理地址,然后获取的是物理页数,这样pa2page函数就能说得通了
#define PPN(va) (((u_long)(va)) >> 12)
// 和PPN等效
#define VPN(va) PPN(va)

/* 页表项标志位
一级页表项(页目录项)
*/
#define PTE_G 0x0100       // Global bit:第9位
#define PTE_V 0x0200       // Valid bit:第10位
#define PTE_R 0x0400       // Dirty bit:第11位'1'意味着可写,对于该页表项对应页面的写操作都必须在页面置换的时候写回内存
#define PTE_D 0x0002       // filesystem cached is dirty(没懂)
#define PTE_COW 0x0001     // Copy on Write
#define PTE_UC 0x0800      // 12位:unCached
#define PTE_LIBRARY 0x0004 // share memory
/*
Part2. Our conventions
*/

// 内存布局图见根目录下的那个

#define KERNBASE 0x80010000 // 内核起始地址

#define VPT (ULIM + PDMAP)      // 暂时不太理解
#define KSTACKTOP (VPT - 0x100) // 暂时不太理解
#define KSTKSIZE (8 * BY2PG)    // 貌似是栈大小？？
#define ULIM 0x80000000         // 操作系统的起始地址

#define UVPT (ULIM - PDMAP)    // 0x7fc0 0000
#define UPAGES (UVPT - PDMAP)  // 0x7f80 0000
#define UENVS (UPAGES - PDMAP) // 0x7f40 0000
// 这里还有一些宏,暂时不管

/*
Part3. Our helper functions
*/
#include "types.h"
// 实现位于init.c
void bcopy(const void *, void *, size_t);
// 实现位于init.c
void bzero(void *, size_t);

// 暂时不理解
extern char bootstacktop[], bootstack[];

// 还有一个奇怪的问题没有解决:看到很多地方,头文件中都只有函数的定义而没有函数的实现,那为什么使用的时候只需要include"头文件"就能使用了呢(明明头文件里只有相关函数的定义)?
/*下面是这个问题的具体解答
知乎问答链接:https://www.zhihu.com/question/389126944
具体解释见test/INCLUDE/关于include.md
*/
// 定义物理页的大小
extern u_long npage;

typedef u_long Pde; // 用于表示一级页表项
typedef u_long Pte; // 用于表示二级页表项:page table entry

/* 关于volatile变量
定义为volatile的变量就是对编译器声明该变量可能会被意想不到的改变
编译器不回去假设这个变量的值
编译器在用到volatile变量的时候必须从内存中读取,而不能使用保存在寄存器中的值
下面是volatile变量的几个例子:
1.并行设备的硬件寄存器（如：状态寄存器）；
2.一个中断服务子程序中会访问到的非自动变量（Non-automatic variables）；
3.多线程应用中的共享变量；
暂时还没看到这两个变量在程序中出现，暂时不管
*/
extern volatile Pte *vpt[];
extern volatile Pde *vpd[];

// 将内核(kseg0)的虚拟地址翻译为物理地址(去除首位即可)
// 很妙的方法,直接将地址和操作系统起始地址相减即可
// 如果虚拟地址不在kseg0范围内,则报错
#define PADDR(kva)                                           \
    ({                                                       \
        u_long a = (u_long)(kva);                            \
        if (a < ULIM)                                        \
            panic("PADDR called with invalid kva %08lx", a); \
        a - ULIM;                                            \
    })

/* 将物理地址转换为内核虚拟地址(kesg0)
首先将根据物理地址获取物理块下标(判断是否超出内存空间)
然后将物理地址与操作系统起始地址相加即可
*/
#define KADDR(pa)                                                    \
    ({                                                               \
        u_long ppn = PPN(pa);                                        \
        if (ppn >= npage)                                            \
            panic("KADDR called with invalid pa %08lx", (u_long)pa); \
        (pa) + ULIM;                                                 \
    })

#endif