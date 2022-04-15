#ifndef _ENV_H_
#define _ENV_H_

#include "types.h"
#include "queue.h"
#include "trap.h"
#include "mmu.h"

// env是Lab3部分的内容,此时只对env结构体分配的内存空间进行测试
struct Env
{
    struct Trapframe env_tf;
    LIST_ENTRY(ENV)
    env_link;
    u_int env_id;
    u_int env_parent_id;
    u_int env_status;
    Pde *env_pgdir;
    u_int env_cr3;

    // Lab4 IPC(暂时不清楚是啥)
    u_int env_ipc_value;
    u_int env_ipc_from;
    u_int env_ipc_recving;
    u_int env_ipc_dstva;
    u_int env_ipc_perm;

    // Lab4 fault handling(暂时不清楚是啥)
    u_int env_pgfault_handler;
    u_int env_xstacktop;

    // Lab6 scheduler counts
    u_int env_runs;
}