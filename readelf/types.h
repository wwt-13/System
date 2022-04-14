// 该头文件用于对各种参数进行宏定义

#ifndef _INC_TYPES_H_
#define _INC_TYPES_H_

#ifndef NULL
#define NULL ((void *)0)
#endif

// 接下来是对一系列关键字的重命名
typedef unsigned char u_int8_t;
typedef short int16_t;
typedef unsigned short u_int16_t;
typedef int int32_t;
typedef unsigned int u_int32_t;
typedef long long int64_t;
typedef unsigned long long u_int64_t;
// 寄存器大小32位
typedef int32_t register_t;

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int; // 这里再定义一遍是为了整齐?
typedef unsigned long u_long;

typedef u_int64_t u_quad_t; // 这里没看懂
typedef int64_t quad_t;
typedef quad_t *qaddr_t;

// typeof是GCC的扩展,作用是用某种已有东西的类型去定义新的变量类型
// typeof的使用是的MIN宏定义可以返回任何类型的最小值
#define MIN(_a, _b)             \
    ({                          \
        typeof(_a) __a = (_a);  \
        typeof(_b) __b = (_b);  \
        __a <= __b ? __a : __b; \
    })
// 原话解释是/* Static assert, for compile-time assertion checking */
// google翻译过来是用于编译时的断言检查,但是我没太理解该宏定义的基本用途,所以还是先放放
#define static_assert(c) \
    switch (c)           \
    case 0:              \
    case (c):
// 暂时不太理解
#define offsetof(type, member) ((size_t)(&((type *)0)->member))
// 看来还是位运算没学好,返回的是[a/n]*n,中括号代表向上取整,要求n必须是2的非负整数次幂
// 这个宏定义的意思就是找到最小的符合条件的初始虚拟地址,将中间未用到的地址空间全部放弃
#define ROUND(a, n) (((((u_long)(a)) + (n)-1)) & ~((n)-1)))
// 上面的ROUND是向上取整,这里相对的定义一个向下取整宏
#define ROUNDDOWM(a, n) (((u_long)(a)) & ~((n)-1))

//这里不知道是出bug了还是什么原因,貌似没检测到endif所以爆红
#endif