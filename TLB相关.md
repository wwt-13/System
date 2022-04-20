# TLB相关知识

## Why use TLB

> 页表一般都很大，而且存放在内存中,所以处理器引入MMU后，读取指令、数据都需要访问两次内存
> 首先通过查询页表得到物理地址，然后访问该物理地址读取指令、数据
> 所以，为了减少因为MMU导致的处理器性能下降，引入了TLB
> TLB中存储了当前最可能被访问到的页表项，其内容是部分页表项的一个副本(简单来说就是页表的cache
> 只有在TLB无法完成地址翻译任务时，才会到内存中查询页表，这样就减少了页表查询导致的处理器性能下降

> TLB组成
> TLB的项由两部分组成：标识和数据，标识中存放的是虚地址的一部分,而数据部分中存放的是物理页号、存储保护信息一级其他的一些辅助信息

> TLB硬件结构

内存管理相关的CP0寄存器

1. BadVaddr(0):用于保存引发地址异常的虚拟地址
2. EntryHi(10),EntryLo(2):**所有**读写TLB的操作都要通过这两个寄存器
   他们分别对应到TLB的Key和Data,但并不是TLB本身(<a href="#Entry">详细介绍见下</a>)
3. Index(0):TLB读写相关使用的寄存器
4. Random(1):随机填写TLB表项时需要用到的寄存器

- 每个TLB表项都有64位,其中高32位是key,第32位是Data
- 

EntryHi,EntryLo的<span name="Entry">位结构如下</span>

- Key(EntryHi)
  - VPN:Virtual Page Number
    - 当**TLB缺失**时(CPU发出虚拟地址,欲在TLB查找物理地址但是未查到),EntryHi中的VPN自动(硬件实现)填充为对应虚拟地址的虚页号
    - 当需要填充或检索TLB表项的时候,软件需要将VPN段填充为对应的虚拟地址
  - ASID:Address Space IDentifier
    - 用于区分不同的地址空间。查找 TLB 表项时，除了需要提供 VPN，还需要提供 ASID（同一虚拟地址在不同的地址空间中通常映射到不同的物理地址）
- Data(EntryLo)
  - PFN:Physical Frame Number
    - 软件通过填写PFN,随后使用TLB指令,才将此时的Key与Data写入TLB中
  - N:Non-cachable
    - 当该位置高时，后续的物理地址访存将不通过 cache
  - D:Dirty
    - 事实上是可写位。当该位置高时，该地址可写；否则任何写操作都将引发 TLB 异常
  - V:Valid
    - 如果该位为低，则任何访问该地址的操作都将引发 TLB 异常
  - G:Global

TLB构建的映射:`< VPN, ASID >→< PFN, N, D, V, G >`

TLB相关指令

- tlbr:以 Index 寄存器中的值为索引,*读出 TLB 中对应的表项*到 EntryHi 与 EntryLo
- tlbwi:以 Index 寄存器中的值为索引,将此时 EntryHi 与 EntryLo 的值写到*索引指定*的 TLB 表项中
- tlbwr:将 EntryHi 与 EntryLo 的数据*随机*写到一个 TLB 表项中（此处使用 Random 寄存器来“随机”指定表项，Random 寄存器本质上是一个不停运行的循环计数器）
- tlbp:*根据 EntryHi 中的 Key（包含 VPN 与 ASID），查找 TLB 中与之对应的表项*，并将表项的索引存入 Index 寄存器（若*未找到匹配项，则 Index 最高位被置 1*）

软件操作TLB的流程

软件必须经过CP0寄存器们与TLB交互,因此软件操作TLB总是分为两步

1. 填写CP0寄存器
2. 使用TLB相关指令

