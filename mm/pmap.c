// 按照知乎上的解释,""是优先在当前文件夹下寻找,而<>是在include文件夹下寻找
#include "mmu.h"
#include "pmap.h"
#include "printf.h"
#include "env.h"
#include "error.h"

/* 关于页表相关函数的约定
1. 以boot为前缀的函数名表示该函数是在系统启动、初始化的时候调用的
2. 不以boot为前缀的函数名表示该函数实在进程的生命周期中被调用的,用于操作进程的页表
对于页面置换函数进行了简化,一旦物理页框被全部分配,进行新的映射的时候并不会进行任何的页面置换
而是直接返回error,也就是-E_NO_MEM
*/

// 以下变量是要被mips_detect_memory函数初始化的变量
u_long maxpa;   // 最大物理地址+1,即物理地址范围为[0,maxpa-1]
u_long npage;   // 总物理页数(注意是物理页数不是虚拟页数)
u_long basemem; // 物理内存对应的字节数
u_long extmem;  // 扩展内存大小,本实验中不涉及,置0即可

Pde *boot_pgdir; // Pde的定义在mmu.h中,原类型是u_long

struct Page *pages;    // 定义的是链表数组
static u_long freemem; // 用于记录内存分配的位置

/* 关于内存分配的疑问:为什么pages需要显式分配内存但是项page_free_list却只需要直接定义即可?
这就涉及到了全局静态变量所分配的位置
编译链接后,内核文件中的data段(起始位置无法确定,具体见内存布局图)用于存储初始化的全局变量和静态变量;
bss段用于存储未初始化的全局变量和静态变量(程序运行结束自动释放,在程序执行之前会被系统自动清零(不知道是否需要由我们来实现?))
所以说全局变量不需要我们来进行手动的分配,它会根据是否初始化被自动被分配到data段和bss段
*/
static struct Page_list page_free_list; // 定义的是链表head

// 内存管理变量初始化函数
// 需要初始化maxpa,basemem,npage,extmem
void mips_detect_memory()
{
    maxpa = 0x4000000;
    basemem = 0x4000000;
    npage = basemem / BY2PG; // 16K个块
    extmem = 0;

    // 接下来输出的就是一些方便调试的信息了
    printf("Physical memory: %dK avaiable,"(int)(maxpa / 1024));
    printf("base = %dK, extended = %dK\n", (int)(basemem / 1024), (int)(extmem / 1024));
    // 初始化完成
}
/*
再放一遍虚拟地址和物理地址的映射规则
若虚拟地址位于0x80000000~0x9fffffff(kseg0),则将虚拟内存的最高位置0即可得到物理地址,通过cache访存,这一部分用于存放内核代码与数据结构
若虚拟地址位于0xa0000000~0xbfffffff(kseg1),则将虚拟地址的最高3位置0即可得到物理地址,不通过cache访存,这一部分用于映射外设
若虚拟地址位于0x00000000~0x7fffffff(kuseg),则需要通过TLB来获取物理地址,通过cache访存(本实验涉及的虚拟空间,都在低2GB虚拟空间中进行访存操作)
*/
// 该段代码的作用是分配n字节的空间并返回空间的起始虚拟地址,同时保证align可以整除初始虚拟地址
// 若clear为真则将对应空间的值清零,否则不清零
static void *alloc(u_int n, u_int align, int clear)
{
    // 下面对这些外部变量做详细解释
    // end变量的定义位于tools/scse0_3.lds:end=0x80400000(总结后)
    // end是虚拟地址,为kseg0,去除首位,得到对应的物理地址是0x400000
    // 所以我们管理的物理地址区间是[0x00400000,0x03ffffff]  (也就是说我们实际上只用了60MB的物理空间)
    // 当然如果算上内核的空间的话正好是64MB,对于物理地址为[0x00000000,0x03ffffff]
    // 这里观察include/mmu.h中的内存布局图,可以发现很多东西
    // 0x80000000-0x80010000存放的是中断异常相关的代码
    // 0x80010000-0x80400000存放的就是内核代码(全连上了,64MB物理空间)
    // 与之对应管理的虚拟内存区间是[0x80400000,0x83ffffff]
    // 喵喵喵,那么这样的话不是说我们管理的内存空间完全不需要考虑TLB映射的问题了吗???(全部位于kseg0中,直接将最高位置0即可得到物理地址)
    extern char end[];  // end[]=0x80400000
    u_long alloced_mem; // 初始地址

    // 如果是第一次分配内存空间的话,需要将freemem置为地址的起始点
    if (freemem == 0)
    {
        freemem = (u_long)end;
    }
    // 使得freemem向上取整能够被align整除(一般用于页对齐,页大小0x1000)
    freemem = ROUND(freemem, align);
    // 对齐后的freemem才是应该返回的初始地址
    alloced_mem = freemem;
    // 分配n个字节的空间(跨页分配没关系！！)
    freemem = freemem + n;
    // 如果分配的地址超出了最大物理地址,则抛出异常
    if (PADDR(freemem) >= maxpa)
    {
        panic("out of memory\n");
        return (void *)-E_NO_MEM;
    }

    // 如果clear=1的话,清空分配的内存块(初始化)
    if (clear)
    {
        bzero((void *)alloced_mem, n);
    }

    return (void *)alloced_mem;
}
/* 启动时的二级页表检索函数
返回以及一级页表基地址pgdir对应的页表结构中,va这个虚拟地址所在的二级页表项
如果create!=0则对应的二级页表不存在则会使用alloc函数分配一页物理内存用于存放
之所以使用alloc而不适用page_alloc的原因是因为这个函数是在内核启动过程中使用的
此时还未建立物理内存管理机制(此时还没有内存管理链表数组)
根据传入的虚拟地址,分配对应二级页表项的虚拟地址
 */
static Pte *boot_pgdir_walk(Pde *pgdir, u_long va, int create)
{
    // pgdir是一级页表的基地址
    Pde *pgdir_entryp = pgdir + PDX(va); // 获取va对应的一级页表项
    Pte *pgtable, *pgtable_entry;
    if ((*pgdir_entryp & PTE_V) == 0) // 估计是判断该一级页表项对应的二级页表是否存在
    {
        if (create)
        {
            // 一级页目录表项中存储的是物理地址
            *pgdir_entryp = PADDR(alloc(BY2PG, BY2PG, 1));
            // 因为分配到的地址一定能被BY2PG整除,所以末12位可以用来表示标志位
            *pgdir_entryp = (*pgdir_entryp) | PTE_V | PTE_R; // 设置PTE_R代表可写
        }
        else
        {
            return 0;
        }
    }
    // 清除添加的标志位对地址造成的影响并将其转换为虚拟地址(理解了)
    pgtable = (Pte *)KADDR(PTE_ADDR(*pgdir_entryp));
    pgtable_entry = pgtable + PTX(va);
    return pgtable_entry; // 返回二级页表项的虚拟地址
}
/* 启动时的区间映射函数
作用是将一级页表基地址pgdir对应的二级页表结构做区间地址映射
将虚拟地址区间[va,va+size-1]映射到物理地址区间[pa,pa+size-1]
因为是按页映射,要求size必须是页面大小的整数倍
同时将相关页表项的权限设置为perm
(就是将va开始的size(需要经过对BY2PG向上取整)大小的空间映射到物理地址中去)
*/
void boot_map_segment(Pde *pgdir, u_long va, u_long size, u_long pa, int perm)
{
    int i;
    Pte *pgtable_entry;
    for (int i = 0, size = ROUND(size, BY2PG); i < size; i += BY2PG)
    {
        // 获得二级页表项对应的虚拟空间地址
        pgtable_entry = boot_pgdir_walk(pgdir, va + i, 1);
        *pgtable_entry = PTE_ADDR(pa + i) | perm | PTE_V;
    }
}
/*官方定义
设置二级页表
*/
void mips_vm_init()
{
    // 关于end的解释见alloc()
    extern char end[];
    // 最初的定义在boot/start.S中
    /*
    .global mCONTEXT
    mCONTEXT:
        .word 0
    经过查询得知:.global是使得符号对链接器可见,是的mCONTEXT变为整个工程可见的全局变量
    .word 0:为该变量分配4字节的存储空间,并用0对存储空间进行初始化
    所以说mCONTEXT就是一个int(0)？(暂且认为是这样吧)
    */
    extern int mCONTEXT;
    extern struct Env *envs;

    // Pde=u_long,但是不理解为什么要u_long,明明虚拟地址最多32位u_int肯定够用
    Pde *pgdir;
    u_int n;

    // 为一级页表分配一页内存空间(并将空间清空)
    pgdir = alloc(BY2PG, BY2PG, 1); // pgdir = 0x80400000
    printf("to memory %x for struct page directory.\n", freemem);
    mCONTEXT = (int)pgdir; // 仍然不清楚干啥用

    // boot_pgdir是一级页表的初始虚拟地址
    boot_pgdir = pgdir;

    /* 为物理内存管理所需要用到的Page结构体按页分配内存
    (这里最好等完全理解了pages的结构再来看:定义位于include/pmap.h中)
    1. 为管理内存的链表数组分配空间
    经过测试(test/PAGES),得到struct Page的大小是24字节,64MB的物理内存,
    页面大小4KB,总共需要分配16K个链表,16K*24B=384KB 384KB/4KB=96
    所以正好分配了96个页面
    */
    // 可以保证所有链表都是连续分配的
    pages = (struct Page *)alloc(npage * sizeof(struct Page), BY2PG, 1);
    printf("to memory %x for struct Pages.\n", freemem);
    // n是实际分配的内存空间大小
    n = ROUND(npage * sizeof(struct Page), BY2PG);
    // boot_map_segemnt就是将
    // PADDR(pages)=pages的物理地址,PADDR的作用是获取kseg0段对应的物理地址
    boot_map_segment(pgdir, UPAGES, n, PADDR(pages), PTE_R); // 不懂这一步究竟是什么目的

    /* 为进程管理所用到的Env结构体按照页分配内存
    设NENV是最大进程数,NENV个Env结构体的大小为n,
    一页的大小为m,由上述函数分析可知分配的大小为[n/m]*m,同时将分配的空间映射到上述的内核页表中,对应的起始虚拟地址为UENVS.
    1. 经过测验,struct Env的大小为392个字节,NENV大小为1K,由此得分配的内存空间大小为392KB,
    页大小为4KB,所以分配页数为392/4=98页(正好分配,不需要进行ROUND)
    */
    envs = (struct Env *)alloc(NENV * sizeof(struct Env), BY2PG, 1);
    n = ROUND(NENV * sizeof(struct Env), BY2PG);
    boot_map_segment(pgdir, UENVS, n, PADDR(envs), PTE_R);

    printf("pmap.c\t mips vm init success\n");
}

/* 用于初始化Pages结构体以及空闲链表
1. 利用链表宏初始化page_free_list
2. 将mips_vm_init()中用到的空间对应的物理页面控制块的引用次数置1
3. 将剩余的物理页面的引用次数置为0，并将对应的内存控制块插入page_free_list中
*/
void page_init(void)
{
    // 1
    if (!LIST_EMPTY(&page_free_list))
        LIST_INIT(&page_free_list);
    // 这一步不可少(对于我的初始化方法而言)
    freemem = ROUND(freemem, BY2PG);
    // 2(根据freemem可得目前分配的物理页号)
    for (int i = 0; i < PPN(PADDR(freemem)); i++)
    {
        pages[i]->pp_ref = 1;
    }
    // 3
    for (int i = PPN(PADDR(freemem)); i < npage; i++)
    {
        pages[i]->pp_ref = 0;
        LIST_INSERT_HEAD(&page_free_list, &pages[i], pp_link);
    }
}
/* page_alloc函数
1. 将page_free_list空闲链表头部内存控制块对应的物理页面分配出去
2. 将其从空闲链表中移除(标记pp_ref=1+清空对应的物理页面)
3. 将pp指向该链表项
*/
int page_alloc(struct Page **pp)
{
    if (LIST_EMPTY(&page_free_list))
    {
        return -E_NO_MEM;
    }
    struct Page *tmp = LIST_FIRST(&page_free_list);
    LIST_REMOVE(tmp, pp_link);
    // 这里不能写tmp->pp_ref的是在别的地方会写
    // tmp->pp_ref = 1;             // 标记为被分配(但是login没写，不知道为啥)
    bzero(page2kva(tmp), BY2PG); // 将对应的物理页面清空
    *pp = tmp;
    return 0;
}
/* 内存引用释放函数
1. 判断pp指向的内存控制块的引用次数是否为0,为0则将对应的内存控制块插入到page_free_list中
 */
void page_free(struct Page *pp)
{
    if (pp->pp_ref > 0)
    {
        return;
    }
    else if (pp->pp_ref == 0)
    { // 插入空闲链表
        LIST_INSERT_HEAD(&page_free_list, pp, pp_link);
        return;
    }
    else
    {
        panic("pp_ref is less than 0\n");
    }
}
/* boot_pgdir_walk的非启动版本
区别如下
1. 前者在内核启动的过程中,不允许失败,后者则是普通运行过程,空间不够允许失败,返回失败码
2. 要将原本的返回值放到ppte的所指的空间上
*/
int pgdir_walk(Pde *pgdir, u_long va, int create, Pte **ppte)
{
    Pde *pgdir_entry = pgdir + PDX(va);
    Pte *pgtable;

    if ((*pgdir_entry & PTE_V) == 0) // 对应二级页表不存在
    {
        struct Page *tmp; // 需要分配对应的内存管理链表管理内存
        if (create)
        {
            if (page_alloc(&tmp) == -E_NO_MEM) // 无法分配内存
                return -E_NO_MEM;
            tmp->pp_ref++;
            *pgdir_entry = (page2pa(tmp)) | PTE_V | PTE_R;
        }
        else
        {
            *ppte = 0;
            return 0;
            // 这里不是很懂为啥要返回0？
        }
    }
    pgtable = (Pte *)KADDR(PTE_ADDR(*pgdir_entry));
    *ppte = pgtable + PTX(va); // 返回的是对应二级页表项虚拟地址
    return 0;
}
/* 增加地址映射函数
将va这一虚拟地址映射到内存控制块pp对应的物理页面
并将页表项权限设置为perm
*/
int page_insert(Pde *pgdir, struct Page *pp, u_long va, u_int perm)
{
    Pte *pgtable_entry;
    pgdir_walk(pgdir, va, 0, &pgtable_entry);                // 判断是否已经对对应的物理页面建立二级页表
    if (pgtable_entry != 0 && (*pgtable_entry & PTE_V) != 0) // 代表以创建二级页表
    {
        // 判断二级页表对应页面是否被pp管理
        if (pa2page(*pgtable_entry) != pp)
        { // 如果管理的不是pp，则将va对应的二级页表移除
            // 猜测是将管理va对应的二级页表的页面移除
            page_remove(pgdir, va);
        }
        else
        { // tlb中存储各种页表项
            // 貌似是标记更新tlb
            tlb_invalidate(pgdir, va);
            *pgtable_entry = (page2pa(pp) | perm | PTE_V);
            return 0;
        }
    }
    tlb_invalidate(pgdir, va);

    if (pgdir_walk(pgdir, va, 1, &pgtable_entry) != 0)
    {
        return -E_NO_MEM;
    }
    *pgtable_entry = page2pa(pp) | perm | PTE_V;
    pp->pp_ref++;
    return 0;
}
/* 寻找映射的物理地址函数
返回va这一虚拟地址映射对应的物理页面对应的内存控制块，同时将ppte指向的空间设为对应的二级页表项地址
*/
struct Page *page_lookup(Pde *pgdir, u_long va, Pte **ppte)
{
    struct Page *ppage;
    Pte *pte;
    // 获取二级页表项存放的物理地址
    pgdir_walk(pgdir, va, 0, &pte);
    if (pte == 0 || (*pte & PTE_V == 0))
    {
        return 0;
    }
    // 获取物理页面对应的内存控制块
    ppage = pa2page(*pte);
    if (ppte) // 这一步判断没理解
    {
        *ppte = pte;
    }
    return ppage;
}
// 用于减少页面应用次数的函数,如果引用次数归零,则自动将链表项归入空闲链表集合中
void page_decref(struct Page *pp)
{
    if (--pp->pp_ref == 0)
    {
        page_free(pp);
    }
}
/* 取消地址映射函数
删除一级页表pgdir对应的二级页表中va这一虚拟地址对物理地址的映射(需要减少物理页面的引用次数)
*/
void page_remove(Pde *pgdir, u_long va)
{
    Pte *pgtable_entry;
    struct Page *ppage;

    ppage = page_lookup(pgdir, va, &pgtable_entry);
    if (ppage == 0)
    {
        return
    }
    else
    {
        page_decref(ppage);
    }
    // 将对应的二级页表项置空
    *pgtable_entry = 0;
    tlb_invalidate(pgdir, va);
    return;
}
// 可以实现删除特定虚拟地址的映射
void tlb_invalidate(Pde *pgdir, u_long va)
{
    if (curenv)
    {
        tlb_out(PTE_ADDR(va) | GET_ENV_ASID(curenv->env_id));
    }
    else
    {
        tlb_out(PTE_ADDR(va));
    }
}
/* 被动地址映射函数
暂时没看懂....
*/
void pageout(int va, int context)
{
}