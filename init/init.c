// 暂时不知道有什么用
#include <asm/asm.h>
// 定义了各种内存管理函数
#include <pmap.h>
#include <env>
// 定义了printf函数
#include <printf.h>
#include <trap.h>

// mips_init()的实现位置
void mips_init()
{
    printf("init.c:\tmips_init() is called\n");

    // Lab2内存管理初始化函数
    // 实现位于mm/pmap.c中,作用是对一些内存管理相关的变量进行初始化
    mips_detect_memory();

    mips_vm_init();

    page_init();

    // 下面两个代码都是用于检测你所填写的代码是否正确的函数,与整体启动流程无关
    physical_memory_manage_check();
    page_check();

    panic("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

    while (1)
        ;

    panic("init.c:\tend of mips_init() reached!");
}

// 拷贝函数:将从src开始的len个字节拷贝到dest
void bcopy(const void *src, void *dest, size_t len)
{
    void *max; //记录拷贝极限地址
    max = dst + len;
    // 首先按照int=4字节进行拷贝
    while (dst + 3 < max)
    {
        *(int *)dst = *(int *)src;
        dst += 4;
        src += 4;
    }
    // 最后按照char=1字节拷贝剩余的0-3个字节
    while (dst < max)
    {
        *(char *)dst = *(char *)src;
        dst++;
        src++;
    }
}
// 置零函数:将从b指针指向开始的len个字节置为0
void bzero(void *b, size_t len)
{
    void *max; // 同样用于记录置零的极限地址
    max = b + len;
    //下面这句话用于调试输出
    printf("init.c:\tzero from %x to %x\n", (int)b, (int)max);
    //后面的实现机理和bcopy函数基本一致
    while (b + 3 < max)
    {
        *(int *)b = 0;
        b += 4;
    }
    while (b < max)
    {
        *(char *)b++ = 0;
    }
}