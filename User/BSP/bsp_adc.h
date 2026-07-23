#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 74HC4051 模拟输出：PA25 / ADC0 通道 2。 */
#define BSP_ADC_CONVERSION_TIMEOUT 100000UL

typedef enum {
    BSP_ADC_CH1 = 0,
    BSP_ADC_CH_COUNT
} BSP_ADC_Ch_t;

void BSP_ADC_Init(void);
uint16_t BSP_ADC_GetRaw(BSP_ADC_Ch_t ch);
BSP_Status_t BSP_ADC_GetRawArray(
    uint16_t *out_buf, uint8_t max_count, uint8_t *out_count);
void BSP_ADC_Start(void);
void BSP_ADC_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ADC_H */
