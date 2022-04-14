# 关于include

- 总和:`gcc main.c -o main -Wall`

## 预处理

预处理指令`gcc main.c -E -o main.i`

好家伙，足足预处理出来接近900行代码
此时就获取到了一个main.i的预编译文件,预编译如果简单理解的话就是替换那些预处理命令，
对于c语言来说就是那些以"#"开头的指令，比如`#include`,`#ifndef`,`#define`等,它会把
include的文件填充进来，确定`#ifdef`之类的地方是否需要编译，以及替换宏定义等等，对于这里
写的main.c文件,则上面的程序预编译会把stdio.h文件填充进来，得到main.i

## 编译

> 编译,但不包括汇编
> 编译指令:`gcc main.i -S -o main.S`

经过编译得到了汇编程序,大致内容见main.S

可以看出这是一个汇编代码，其中有各种段的初始化,以及main函数栈的初始化
并且在23行可以看到`call puts`,这是对应的printf函数

## 汇编

> 在经过编译将C程序翻译成汇编后,距离机器码只有一步之遥,也就是将汇编指令翻译成机器码
> 此时可以使用gcc命令进行汇编:`gcc main.S -c -o main.o`
> 同时为了是的代码可读,还需要对汇编代码进行反汇编:`objdump -DSx main.o > main_res.S`
但是我本地无法查看反汇编代码,所以还是需要去jump server虚拟机上通过objump指令反汇编

此时call puts这句话没了,被替换成了下面这两句话
```x64
  1f:   e8 00 00 00 00          callq  24 <main+0x24>
                        20: R_X86_64_PLT32      printf-0x4
```
这不是一个运行时的有效地址，目前程序还不知道要去哪找puts这个函数
因为当前这个程序还没说明puts这个函数在哪里，可以康康Symbol Table中明确说明了printf这个符号还未定义
`0000000000000000         *UND*  0000000000000000 printf`
所以这里挖个坑还需要从别的地方找

汇编器生成的未经过链接的目标文件，它实际上不知道数据和代码要在内存的什么位置上运行
它更不知道那些本地引用的外部定义的全局变量和函数的位置
**所以汇编器只管挖好这些坑，让链接器后面来填**

那么从别的地方找到一个“外部”的函数定义并组合进来就是链接需要做的事情了

所以下一步我们需要做的就是将目标文件`main.o`和其他目标文件进行链接

## 链接

> 其实可以直接执行`gcc main.o -o main`直接得到可执行程序的,但是这里不这么做

因为gcc为我们省略了很多默认的选项，比如说libc库，为了弄清楚链接究竟做了些什么，我们需要使用ld命令来进行链接操作‘

可以发现，只对一个单独的main.o目标文件进行链接时，发生报错(显示未定义__main和printf)
本机报错
```shell
main.o:main.c:(.text+0x10): undefined reference to `__main'
main.o:main.c:(.text+0x1c): undefined reference to `puts'
```
课程机报错(找不到_start符号和printf)
```shell
ld: warning: cannot find entry symbol _start; defaulting to 0000000000401000
ld: main.o: in function `main':
main.c:(.text+0x20): undefined reference to `printf'
```

现在再尝试链接一下libc标准库(printf位于libc标准库中)
`ld -o main /user/lib32/libc.so main.o -dynamic-linker /lib32/ld-linux.so.2`
(但是我链接失败了,不知道为啥...)

反汇编可得`callq 401010 <puts@plt>`，得到了一个有效的地址，执行的时候就能根据这个找到printf的入口

printf函数的实现在libc链接库目标文件中，在源文件中的`#include<stdio.h>`的作用仅仅是在预编译的时候得到printf的声明
，让程序知道要到外面去找这个符号,但是这个文件在哪里,**.h文件是不知道的**，这个需要链接的时候的时候让连接器去所有的目标文件里找这个符号，找到之后把它链接进来，所以对于我们当前的系统来说我们要把main.o和/usr/lib32/libc.so一起链接
让链接器从libc.so中找到printf的实现

理解了为啥课程中的各个文件只需要`#include`函数的定义就行,因为最终的Makefile文件将所有实现都链接到一起了，
符号表中已经包括了`#include`中实现的所有函数

# 总结

> 是否可以这样理解呢？
> 编译给每个程序单独生成了一堆符号表,挖了一堆坑
> 链接则遍历所有程序的符号表,并为所有程序填好坑