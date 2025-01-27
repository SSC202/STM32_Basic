#ifndef __MALLOC_H
#define __MALLOC_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#ifndef NULL
#define NULL 0
#endif

// 定义三个内存池
#define SRAMIN   0 // 内部内存池
#define SRAMEX   1 // 外部内存池(SDRAM)
#define SRAMCCM  2 // CCM内存池(此部分SRAM仅仅CPU可以访问!!!)

#define SRAMBANK 3 // 定义支持的SRAM块数.

// mem1内存参数设定.mem1完全处于内部SRAM里面.
#define MEM1_BLOCK_SIZE       64                              // 内存块大小为64字节
#define MEM1_MAX_SIZE         160 * 1024                      // 最大管理内存 160K
#define MEM1_ALLOC_TABLE_SIZE MEM1_MAX_SIZE / MEM1_BLOCK_SIZE // 内存表大小

// mem2内存参数设定.mem2的内存池处于外部SDRAM里面
#define MEM2_BLOCK_SIZE       64                              // 内存块大小为64字节
#define MEM2_MAX_SIZE         28912 * 1024                    // 最大管理内存28912K
#define MEM2_ALLOC_TABLE_SIZE MEM2_MAX_SIZE / MEM2_BLOCK_SIZE // 内存表大小

// mem3内存参数设定.mem3处于CCM,用于管理CCM(特别注意,这部分SRAM,仅CPU可以访问!!)
#define MEM3_BLOCK_SIZE       64                              // 内存块大小为64字节
#define MEM3_MAX_SIZE         60 * 1024                       // 最大管理内存60K
#define MEM3_ALLOC_TABLE_SIZE MEM3_MAX_SIZE / MEM3_BLOCK_SIZE // 内存表大小

// 内存管理控制器
struct _m_mallco_dev {
    void (*init)(uint8_t);        // 初始化
    uint16_t (*perused)(uint8_t); // 内存使用率
    uint8_t *membase[SRAMBANK];   // 内存池 管理SRAMBANK个区域的内存
    uint32_t *memmap[SRAMBANK];   // 内存管理状态表
    uint8_t memrdy[SRAMBANK];     // 内存管理是否就绪
};
extern struct _m_mallco_dev mallco_dev; 

void mymemset(void *s, uint8_t c, uint32_t count);   // 设置内存
void mymemcpy(void *des, void *src, uint32_t n);     // 复制内存
void my_mem_init(uint8_t memx);                      // 内存管理初始化函数(外/内部调用)
uint16_t my_mem_perused(uint8_t memx);               // 获得内存使用率(外/内部调用)

void myfree(uint8_t memx, void *ptr);                    // 内存释放(外部调用)
void *mymalloc(uint8_t memx, uint32_t size);             // 内存分配(外部调用)
void *myrealloc(uint8_t memx, void *ptr, uint32_t size); // 重新分配内存(外部调用)

#ifdef __cplusplus
}
#endif
#endif