#ifndef _MMU_H_
#define _MMU_H_

/*
如下是官方原版注释:
This file contains:
Part1. MIPS definitions
Part2. Our conventions(惯例)
Part3. Our helper functions.
*/

/*
Part1. MIPS definitions
*/

#define BY2PG 4096              // 定义了页大小
#define PDMAP (4 * 1024 * 1024) //暂时不理解
#define PGSHIFT 12
#define PDSHIFT 22

/*
Part2. Our conventions
*/

/*内存布局图,非常重要
 o     4G ----------->  +----------------------------+------------0x100000000
 o                      |       ...                  |  kseg3
 o                      +----------------------------+------------0xe000 0000
 o                      |       ...                  |  kseg2
 o                      +----------------------------+------------0xc000 0000
 o                      |   Interrupts & Exception   |  kseg1
 o                      +----------------------------+------------0xa000 0000
 o                      |      Invalid memory        |   /|\
 o                      +----------------------------+----|-------Physics Memory Max
 o                      |       ...                  |  kseg0
 o  VPT,KSTACKTOP-----> +----------------------------+----|-------0x8040 0000-------end
 o                      |       Kernel Stack         |    | KSTKSIZE            /|\
 o                      +----------------------------+----|------                |
 o                      |       Kernel Text          |    |                    PDMAP
 o      KERNBASE -----> +----------------------------+----|-------0x8001 0000    |
 o                      |   Interrupts & Exception   |   \|/                    \|/
 o      ULIM     -----> +----------------------------+------------0x8000 0000-------
 o                      |         User VPT           |     PDMAP                /|\
 o      UVPT     -----> +----------------------------+------------0x7fc0 0000    |
 o                      |         PAGES              |     PDMAP                 |
 o      UPAGES   -----> +----------------------------+------------0x7f80 0000    |
 o                      |         ENVS               |     PDMAP                 |
 o  UTOP,UENVS   -----> +----------------------------+------------0x7f40 0000    |
 o  UXSTACKTOP -/       |     user exception stack   |     BY2PG                 |
 o                      +----------------------------+------------0x7f3f f000    |
 o                      |       Invalid memory       |     BY2PG                 |
 o      USTACKTOP ----> +----------------------------+------------0x7f3f e000    |
 o                      |     normal user stack      |     BY2PG                 |
 o                      +----------------------------+------------0x7f3f d000    |
 a                      |                            |                           |
 a                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                           |
 a                      .                            .                           |
 a                      .                            .                         kuseg
 a                      .                            .                           |
 a                      |~~~~~~~~~~~~~~~~~~~~~~~~~~~~|                           |
 a                      |                            |                           |
 o       UTEXT   -----> +----------------------------+                           |
 o                      |                            |     2 * PDMAP            \|/
 a     0 ------------>  +----------------------------+ -----------------------------
 o
*/
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

typedef u_long Pde;
typedef u_long Pte;

#endif
#endif