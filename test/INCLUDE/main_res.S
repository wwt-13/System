main.o:     file format elf64-x86-64
main.o
architecture: i386:x86-64, flags 0x00000011:
HAS_RELOC, HAS_SYMS
start address 0x0000000000000000

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         0000002b  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000000  0000000000000000  0000000000000000  0000006b  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  0000000000000000  0000000000000000  0000006b  2**0
                  ALLOC
  3 .rodata       0000000f  0000000000000000  0000000000000000  0000006b  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      0000002b  0000000000000000  0000000000000000  0000007a  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000a5  2**0
                  CONTENTS, READONLY
  6 .note.gnu.property 00000020  0000000000000000  0000000000000000  000000a8  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 .eh_frame     00000038  0000000000000000  0000000000000000  000000c8  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
SYMBOL TABLE:
0000000000000000 l    df *ABS*  0000000000000000 main.c
0000000000000000 l    d  .text  0000000000000000 .text
0000000000000000 l    d  .data  0000000000000000 .data
0000000000000000 l    d  .bss   0000000000000000 .bss
0000000000000000 l    d  .rodata        0000000000000000 .rodata
0000000000000000 l    d  .note.GNU-stack        0000000000000000 .note.GNU-stack
0000000000000000 l    d  .note.gnu.property     0000000000000000 .note.gnu.property
0000000000000000 l    d  .eh_frame      0000000000000000 .eh_frame
0000000000000000 l    d  .comment       0000000000000000 .comment
0000000000000000 g     F .text  000000000000002b main
0000000000000000         *UND*  0000000000000000 _GLOBAL_OFFSET_TABLE_
0000000000000000         *UND*  0000000000000000 printf



Disassembly of section .text:

0000000000000000 <main>:
   0:   f3 0f 1e fa             endbr64 
   4:   55                      push   %rbp
   5:   48 89 e5                mov    %rsp,%rbp
   8:   48 83 ec 10             sub    $0x10,%rsp
   c:   89 7d fc                mov    %edi,-0x4(%rbp)
   f:   48 89 75 f0             mov    %rsi,-0x10(%rbp)
  13:   48 8d 3d 00 00 00 00    lea    0x0(%rip),%rdi        # 1a <main+0x1a>
                        16: R_X86_64_PC32       .rodata-0x4
  1a:   b8 00 00 00 00          mov    $0x0,%eax
  1f:   e8 00 00 00 00          callq  24 <main+0x24>
                        20: R_X86_64_PLT32      printf-0x4
  24:   b8 00 00 00 00          mov    $0x0,%eax
  29:   c9                      leaveq 
  2a:   c3                      retq   

Disassembly of section .rodata:

0000000000000000 <.rodata>:
   0:   54                      push   %rsp
   1:   68 69 73 20 69          pushq  $0x69207369
   6:   73 20                   jae    28 <main+0x28>
   8:   61                      (bad)  
   9:   20 74 65 73             and    %dh,0x73(%rbp,%riz,2)
   d:   74 00                   je     f <main+0xf>

Disassembly of section .comment:

0000000000000000 <.comment>:
   0:   00 47 43                add    %al,0x43(%rdi)
   3:   43 3a 20                rex.XB cmp (%r8),%spl
   6:   28 55 62                sub    %dl,0x62(%rbp)
   9:   75 6e                   jne    79 <main+0x79>
   b:   74 75                   je     82 <main+0x82>
   d:   20 39                   and    %bh,(%rcx)
   f:   2e 33 2e                xor    %cs:(%rsi),%ebp
  12:   30 2d 31 37 75 62       xor    %ch,0x62753731(%rip)        # 62753749 <main+0x62753749>
  18:   75 6e                   jne    88 <main+0x88>
  1a:   74 75                   je     91 <main+0x91>
  1c:   31 7e 32                xor    %edi,0x32(%rsi)
  1f:   30 2e                   xor    %ch,(%rsi)
  21:   30 34 29                xor    %dh,(%rcx,%rbp,1)
  24:   20 39                   and    %bh,(%rcx)
  26:   2e 33 2e                xor    %cs:(%rsi),%ebp
  29:   30 00                   xor    %al,(%rax)

Disassembly of section .note.gnu.property:

0000000000000000 <.note.gnu.property>:
   0:   04 00                   add    $0x0,%al
   2:   00 00                   add    %al,(%rax)
   4:   10 00                   adc    %al,(%rax)
   6:   00 00                   add    %al,(%rax)
   8:   05 00 00 00 47          add    $0x47000000,%eax
   d:   4e 55                   rex.WRX push %rbp
   f:   00 02                   add    %al,(%rdx)
  11:   00 00                   add    %al,(%rax)
  13:   c0 04 00 00             rolb   $0x0,(%rax,%rax,1)
  17:   00 03                   add    %al,(%rbx)
  19:   00 00                   add    %al,(%rax)
  1b:   00 00                   add    %al,(%rax)
  1d:   00 00                   add    %al,(%rax)
        ...

Disassembly of section .eh_frame:

0000000000000000 <.eh_frame>:
   0:   14 00                   adc    $0x0,%al
   2:   00 00                   add    %al,(%rax)
   4:   00 00                   add    %al,(%rax)
   6:   00 00                   add    %al,(%rax)
   8:   01 7a 52                add    %edi,0x52(%rdx)
   b:   00 01                   add    %al,(%rcx)
   d:   78 10                   js     1f <.eh_frame+0x1f>
   f:   01 1b                   add    %ebx,(%rbx)
  11:   0c 07                   or     $0x7,%al
  13:   08 90 01 00 00 1c       or     %dl,0x1c000001(%rax)
  19:   00 00                   add    %al,(%rax)
  1b:   00 1c 00                add    %bl,(%rax,%rax,1)
  1e:   00 00                   add    %al,(%rax)
  20:   00 00                   add    %al,(%rax)
                        20: R_X86_64_PC32       .text
  22:   00 00                   add    %al,(%rax)
  24:   2b 00                   sub    (%rax),%eax
  26:   00 00                   add    %al,(%rax)
  28:   00 45 0e                add    %al,0xe(%rbp)
  2b:   10 86 02 43 0d 06       adc    %al,0x60d4302(%rsi)
  31:   62                      (bad)  
  32:   0c 07                   or     $0x7,%al
  34:   08 00                   or     %al,(%rax)
        ...