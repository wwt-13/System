// 在研究pmap.c之前,必须深入了解链表宏的定义以及链表结构体的具体结构
#ifndef _SYS_QUEUE_H_
#define _SYS_QUEUE_H_
/* 文件概述
该文件定义了3中数据结构,链表、尾队列和循环队列(lab2只考虑尾链表)
如果要讲的更加贴合实际一点,实现的应该是“伪双向链表”
*/

/* 关于链表操作的具体规范
(此块内容等完全理解链表操作后再来进行完善)
1. 使用LIST_NEXT((elm),field)来获取elm指向下一个链表项的指针
2. 使用*(listelm)->field.le_prev来修改前链表项的le_next指向
3. 使用&LIST_NEXT((elm),field)来修改le_prev的指向
*/

// 伪双向链表数据结构宏定义

/*
创建了一个名为name链表的head结构体,包含一个指向type类型结构体的指针
这个指针指向了链表的首个元素(类似与数据结构中讲的head)
 */
#define LIST_HEAD(name, type)  \
    struct name                \
    {                          \
        struct type *lh_first; \
    }
// 将管理整个链表的head节点置为空
#define LIST_HEAD_INITIALIZER(head) \
    {                               \
        NULL                        \
    }
/* 链表项的数据类型
le_next:指向下一个链表项的指针
le_prev:指向 前一个链表项 指向下一个链表项的指针(它是伪双向链表实现的关键)
*/
#define LIST_ENTRY(type)       \
    struct                     \
    {                          \
        struct type *le_next;  \
        struct type **le_prev; \
    }

// 伪双向链表函数宏定义

// 判断链表是否为空(传入为管理链表的head)
#define LIST_EMPTY(head) ((head)->lh_first == NULL)
// 返回第一个链表项
#define LIST_FIRST(head) ((head)->lh_first)
// 将链表置为NULL
#define LIST_INIT(head)            \
    do                             \
    {                              \
        LIST_FIRST((head)) = NULL; \
    } while (0)
/* 关于do{}while(0)的使用问题
Linux和其它代码库里的宏都用do/while(0)来包围执行逻辑，因为它能确保宏的行为总是相同的，而不管在调用代码中使用了多少分号和大括号
具体解释地址:https://www.cnblogs.com/lanxuezaipiao/p/3535626.html
*/
// 结构体elm包含了名为field的数据,所以这里相当于返回当前链表项指向下一个链表项的指针
#define LIST_NEXT(elm, field) ((elm)->field.le_next)
/*
暂时还没看懂这函数在干啥
 */
#define LSIT_FOREACH(var, head, field) \
    for ((var) = LIST_FIRST((head));   \
         (var);                        \
         (var) = LIST_NEXT((var), field))
/* 链表后插函数
将elm插入到listelm之后
感觉还是我的写法更加清晰
*/
#define LIST_INSERT_AFTER(listelm, elm, field)                                     \
    do                                                                             \
    {                                                                              \
        LIST_NEXT((elm), field) = LIST_NEXT((listelm), field);                     \
        LIST_NEXT((listelm), field) = (elm);                                       \
        if (LIST_NEXT((elm), field) != NULL)                                       \
            LIST_NEXT((elm), field)->field.le_prev = &LIST_NEXT((elm), field);     \
        LIST_NEXT((listelm), field)->field.le_prev = &LIST_NEXT((listelm), field); \
    } while (0)

/* 链表前插函数
将elm插入到listelm的前面,field是链表项的数据类型
*/
#define LIST_INSERT_BEFORE(listelm, elm, field)              \
    do                                                       \
    {                                                        \
        (elm)->field.le_prev = (listelm)->field.le_prev;     \
        LIST_NEXT((elm), field) = (listelm);                 \
        *(listelm)->field.le_prev = (elm);                   \
        (listelm)->field.le_prev = &LIST_NEXT((elm), field); \
    }
/* 头部插入链表项函数
通用步骤
1. LIST_FIRST((head))=(elm)
2. (elm)->field.le_prev=&LIST_FIRST((head))
*/
#define LIST_INSERT_HEAD(head, elm, field)                                \
    do                                                                    \
    {                                                                     \
        if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != NULL)       \
            LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field); \
        LIST_FIRST((head)) = (elm);                                       \
        (elm)->field.le_prev = &LIST_FIRST((head));                       \
    } while (0)
/* 尾部插入链表项函数
 */
#define LIST_INSERT_TAIL(head, elm, field)                                             \
    do                                                                                 \
    {                                                                                  \
        if (LIST_FIRST((head)) != NULL)                                                \
        {                                                                              \
            LIST_NEXT((elm), field) = LIST_FIRST((head));                              \
            while (LIST_NEXT(LIST_NEXT((elm), field), field) != NULL)                  \
                (LIST_NEXT((elm), field) = LIST_NEXT(LIST_NEXT((elm), field), field)); \
            LIST_NEXT(LIST_NEXT((elm), field), field) = (elm);                         \
            (elm)->field.le_prev = &LIST_NEXT(LIST_NEXT((elm), field), field);         \
            LIST_NEXT((elm), field) = NULL;                                            \
        }                                                                              \
        else                                                                           \
        {                                                                              \
            LIST_FIRST((head)) = (elm);                                                \
            (elm)->field.le_prev = &LIST_FIRST((head));                                \
        }                                                                              \
    } while (0)
/* 删除链表项函数
 */
#define LIST_REMOVE(elm, field)                                            \
    do                                                                     \
    {                                                                      \
        if (LIST_NEXT((elm), field) != NULL)                               \
            LIST_NEXT((elm), field)->field.le_prev = (elm)->field.le_prev; \
        *(elm)->field.le_prev = LIST_NEXT((elm), field);                   \
    } while (0)

/* 尾部链表定义
这里的定义之后再来看看(lab2暂时不涉及)
 */
#define TAILQ_HEAD(name, type)  \
    struct name                 \
    {                           \
        struct type *tqh_first; \
        struct type **tqh_last; \
    }

#define TAILQ_ENTRY(type)       \
    struct                      \
    {                           \
        struct type *tqe_next;  \
        struct type **tqe_prev; \
    }

#endif