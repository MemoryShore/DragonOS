#pragma once

#define __force __attribute__((force))

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#ifndef barrier
// 内存屏障
#define barrier() __asm__ __volatile__("" :: \
                                           : "memory");
#endif

// 编译器属性

// 当函数的返回值未被使用时，编译器抛出警告信息
#define __must_check __attribute__((__warn_unused_result__))

typedef uint8_t __attribute__((__may_alias__)) __u8_alias_t;
typedef uint16_t __attribute__((__may_alias__)) __u16_alias_t;
typedef uint32_t __attribute__((__may_alias__)) __u32_alias_t;
typedef uint64_t __attribute__((__may_alias__)) __u64_alias_t;

/**
 * @brief 从src读取数据到dst，该过程避免编译器优化。
 * 
 * @param dst 目标地址指针
 * @param src 源地址指针
 * @param size 要读取的数据大小（建议1、2、4、8字节，若不满足要求，则采用memcpy读取。）
 */
static __always_inline void __read_once_size(void *dst, const volatile void *src, int size)
{
    switch (size)
    {
    case 1:
        *(__u8_alias_t *)dst = *(volatile __u8_alias_t *)src;
        break;
    case 2:
        *(__u16_alias_t *)dst = *(volatile __u16_alias_t *)src;
        break;
    case 4:
        *(__u32_alias_t *)dst = *(volatile __u32_alias_t *)src;
        break;
    case 8:
        *(__u64_alias_t *)dst = *(volatile __u64_alias_t *)src;
        break;
    default:
        barrier();
        __builtin_memcpy((void *)dst, (const void *)src, size);
        barrier();
        break;
    }
}

/**
 * @brief 把src处的数据到dst，该过程避免编译器优化。
 * 
 * @param dst 目标地址指针
 * @param src 源地址指针
 * @param size 要写入的数据大小（建议1、2、4、8字节，若不满足要求，则采用memcpy传输。）
 */
static __always_inline void __write_once_size(volatile void *dst, void *src, int size)
{
    switch (size)
    {
    case 1:
        *(volatile __u8_alias_t *)dst = *(__u8_alias_t *)src;
        break;
    case 2:
        *(volatile __u16_alias_t *)dst = *(__u16_alias_t *)src;
        break;
    case 4:
        *(volatile __u32_alias_t *)dst = *(__u32_alias_t *)src;
        break;
    case 8:
        *(volatile __u64_alias_t *)dst = *(__u64_alias_t *)src;
        break;
    default:
        barrier();
        __builtin_memcpy((void *)dst, (const void *)src, size);
        barrier();
        break;
    }
}

/**
 * 这两个宏能够避免编译器重排序、合并涉及到的读写操作，从而避免由于编译器优化导致的多线程读写顺序错误。
 * 通过将有顺序要求的两个读/写操作放置在READ_ONCE()和WRITE_ONCE()之中，能够让编译器知道这些操作具有顺序要求。
 *
 * 这两个宏同样适用于Union或struct。如果要访问的数据大小不是1、2、4、8字节，则会使用memcpy来处理。
 *
 * 这两个宏的主要使用场景：
 * 1.两个进程或者中断处理函数之间的信息交流与沟通
 * 2.确保编译器不会折叠、旋转或以其他方式对代码进行优化，从而破坏数据访问顺序。
 *
 * 这两个宏的union __u内的__c用作这个union的地址的指针
 * 
 * 关于READ_ONCE和WRITE_ONCE的简单说明，请转到：https://bbs.dragonos.org/forum.php?mod=viewthread&tid=24
 */

/**
 * @brief 读取变量x (避免编译器优化)
 */
#define READ_ONCE(x)                                \
    ({                                              \
        union                                       \
        {                                           \
            typeof(x) __val;                        \
            char __c[1];                            \
        } __u = {.__c = {0}};                       \
        __read_once_size(__u.__c, &(x), sizeof(x)); \
        __u.__val;                                  \
    })

/**
 * @brief 将val写入变量x (避免编译器优化)
 */
#define WRITE_ONCE(x, val)                           \
    ({                                               \
        union                                        \
        {                                            \
            typeof(x) __val;                         \
            char __c[1];                             \
        } __u = {.val = (val)};                      \
        __write_once_size(&(x), __u.__c, sizeof(x)); \
        __u.__val;                                   \
    })
