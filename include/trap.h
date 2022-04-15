// 这里只定义一个结构体Trapframe用于测试
struct Trapframe
{
    u_long regs[32];

    u_long cp0_status;
    u_long hi;
    u_long lo;
    u_long cp0_badvaddr;
    u_long cp0_cause;
    u_long cp0_epc;
    u_long pc;
};