OUTPUT_ARCH(mips)
ENTRY(_start)
/* 暂时还看不太懂 */
SECTIONS
{
    .=0x80010000;
    /*.text段
    通过对生成的操作系统内核进行分析
    .text段的起始地址为0x80010000
    .text段用于保存可执行文件的操作指令
    下面这段代码的含义是将所有输入文件的.int,.text,.fini都放入vm_linux的.text段中(应该?)
    */
    .text:{
        *(.int)
        *(.text)
        *(.fini)
    }
    /* .data段
    个人认为:段的大小与程序所写内容息息相关(不同Lab的段大小根据代码量的不同各不相同)
    .data段用于保存以及初始化的全局变量和静态变量
    下面这段代码表示vm_linux的.data段用于存放所有程序的.data段
    */
    .data:{
        *(.data)
    }
    /* .bss段
    .bss段用于保存未初始化的全局变量和静态变量
    下面这段代码表示vm_linux的.bss段用于存放所有程序的.bss段
    */
    .bss:{
        *(.bss)
    }
    .sdata:{
        *(.sdata)
    }
    .=0x80400000;
    end=.;
}