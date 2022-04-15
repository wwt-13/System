#define LIST_HEAD(name, type)  \
    struct name                \
    {                          \
        struct type *lh_first; \
    }
#define LIST_HEAD_INITIALIZER(head) \
    {                               \
        NULL                        \
    }
#define LIST_ENTRY(type)       \
    struct                     \
    {                          \
        struct type *le_next;  \
        struct type **le_prev; \
    }
LIST_HEAD(Page_list, Page);
typedef LIST_ENTRY(Page) Page_LIST_entry_t;
struct Page
{
    Page_LIST_entry_t pp_link;

    unsigned short pp_ref; // 该页被alloc的次数
};

typedef unsigned int u_int;
typedef unsigned long u_long;
typedef u_long Pde;

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

typedef struct Env
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
};