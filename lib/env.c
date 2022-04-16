#include <mmu.h>
#include <error.h>
#include <env.h>
#include <pmap.h>
#include <printf.h>

// 暂时不清楚这些干啥用(但是在tlb_invalidate函数中会用到)
struct Env *envs = NULL;
struct Env *curenv = NULL;