// 测试宏_PMAP_H_是否被定义
// 该宏用于解决重复定义的问题
// 当头文件第一次被包含的时候,它被正常处理,符号_PMAP_H_被定义为1
// 如果头文件被再次包含，通过条件编译，它的内容被忽略
#ifndef _PMAP_H_
// 如果没被定义，再定义宏_PMAP_H_
#define _PMAP_H_

#include "types.h"
#include "queue.h"
#include "mmu.h"
#include "printf.h"

/* 定义链表结构体 */

// 定义Page_list结构体
// 参考LIST_HEAD的定义可将下方代码等价转换为
// struct Page_list{
//     struct Page *lh_first
// };
LIST_HEAD(Page_list, Page);

// 参考LIST_ENTRY的定义可将下方代码等价转换为
// struct{
//     struct Page *le_next;
//     struct Page **le_prev;
// };
typedef LIST_ENTRY(Page) Page_LIST_entry_t;

// 同理,可将链表项的定义等价转化为
// struct Page{
//     struct{
//         struct Page *le_next;
//         struct Page **le_prev;
//     }pp_link;
//     u_short pp_ref;
// };
struct Page
{
    Page_LIST_entry_t pp_link;

    u_short pp_ref; // 该页被alloc的次数
};
// 利用extern关键字引用外部已经定义的全局变量
extern struct Page *pages; // pages的定义位于pmap.c

// static inline详解
// static
// c语言中，函数在默认情况下是global的(可以被其他文件使用extern访问)
// 而static关键字是其变为静态的，访问范围被限制到声明它们的文件
// 因此，如果我们想要限制对于函数的访问，就把它们声明为static
// 并且，因为static会将变量和方法对于其他文件隐藏，所以利用static可以在不同文件存在同名的static函数
// inline
// inline是向编译器建议，将被inline修饰的函数以内联的方式嵌入到调用这个函数的地方
// 什么是内联？
// 一般来说,调用一个函数的流程如下:
// 1. 保存调用该函数的命令地址
// 2. 程序流跳转到所调用的函数并执行该函数
// 3. 最后跳转回之前所保存的命令地址、
// 这样的跳转对于需要经常调用的小函数来说，大大降低了程序的运行效率，所以C99新增了内联函数(inline function)
// 关键字inline推荐编译器，任何地方只要调用该文件的内联函数，就直接把该函数的机器码插入到调用它的地方，是的程序的执行更有效率(空间换时间)
// 关于static和inline的具体细节还需要进一步的理解，以后有时间再来细看

// 返回当前pp指向页在页数组中的下标(和宏定义PPN的区别是可以直接传入虚拟地址)
static inline u_long page2ppn(struct Page *pp)
{
    return pp - pages;
}
// 获取pp链表项对应页面相对于起始页面的偏移量(如果起始页面的地址为0,那么获取的就是物理地址?)
static inline u_long page2pa(struct Page *pp)
{
    // PGSHIFT的定义在mmu.h中,`#define PGSHIFT 12`
    // 所以此函数的作用是将pp指针指向链表项在数组中的下标左移12位
    // 左移12位,刚好等于的是页的大小4K,所以本函数的目的是获取pp链表项对应页面相对于起始页面的偏移量(确实获取的是偏移量而不是物理地址)
    return page2ppn(pp) << PGSHIFT;
}
// 关于虚拟地址到物理地址的映射关系
// 在实验中,虚拟地址映射到物理地址,然后通过物理地址来进行访存,与我们实验相关的映射和寻址规则如下
// 虚拟空间大小4GB(0x100000000),物理空间大小64MB(0x4000000)
// 若虚拟地址位于0x80000000~0x9fffffff(kseg0),则将虚拟内存的最高位置0即可得到物理地址,通过cache访存,这一部分用于存放内核代码与数据结构
// 若虚拟地址位于0xa0000000~0xbfffffff(kseg1),则将虚拟地址的最高3位置0即可得到物理地址,不通过cache访存,这一部分用于映射外设
// 若虚拟地址位于0x00000000~0x7fffffff(kuseg),则需要通过TLB来获取物理地址,通过cache访存(本实验涉及的虚拟空间,都在低2GB虚拟空间中进行访存操作)

// 总结:pa2page需要传入物理地址,返回管理该物理地址对应页面的链表项地址
static inline struct Page *pa2page(u_long *pa)
{
    // 如果物理页号大于最大物理页号
    if (PPN(pa) >= npage)
        panic("pa2page called with invalid pa");
    return &pages[PPN(pa)];
}
// 将传入的链表项虚拟地址(kseg0)转换为对应虚拟地址
static inline u_long page2kva(struct Page *pp)
{
    return KADDR(page2pa(pp));
}
static inline u_long va2pa(Pde *pgdir, u_long va)
{
    Pte *p; // 等价于u_long *p;

    pgdir = &pgdir[PDX(va)];
    if (!(*pgdir & PTE_V))
        return ~0;
    p = (Pte *)KADDR(PTE_ADDR(*pgdir));
    if (!(p[PTX(va)] & PTE_V))
        return ~0;
    return PTE_ADDR(p[PTX(va)]);
}
// 接下来是内存管理的函数定义,实现在mm/pmap.c中
void mips_detect_memory();

void mips_vm_init();

void mips_init(); // 实现在init/init.c中,调用了一些函数
void page_init();
void page_check();
void physical_memory_manage_check();
int page_alloc(struct Page **pp);
void page_free(struct Page **pp);
void page_decref(struct Page **pp);
int pgdir_walk(Pde *pgdir, u_long va, int create, Pte **ppte);
int page_insert(Pde *pgdir, struct Page *pp, u_long va, u_int perm);
struct Page *page_lookup(Pde *pgdir, u_long va, Pte **ppte);
void page_remove(Pde *pgdir, u_long va);
void tlb_invalidate(Pde *pgdir, u_long va);

// 从外部源文件引入pages
extern struct Page *pages;

#endif