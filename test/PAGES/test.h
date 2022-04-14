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