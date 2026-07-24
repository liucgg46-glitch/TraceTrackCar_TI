#ifndef __DRV_E220_H
#define __DRV_E220_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MSPM0G3519的E220透明传输适配层。
 *
 * UART_PORT_E220对应硬件UART4：
 *   PB10 -> E220 RXD；
 *   PB11 <- E220 TXD；
 *   PB28 <- E220 AUX。
 *
 * E220模块使用115200、8N1；透明传输模式下M0、M1由硬件可靠拉低。
 */
#ifndef DRV_E220_TX_QUEUE_DEPTH
#define DRV_E220_TX_QUEUE_DEPTH          16U
#endif

#ifndef DRV_E220_TX_FRAME_MAX_SIZE
#define DRV_E220_TX_FRAME_MAX_SIZE       512U
#endif

typedef struct {
    uint16_t queued_frames;
    uint32_t deferred_frames;
    uint32_t retried_frames;
    uint32_t queue_full_frames;
    uint32_t oversize_frames;
} Drv_E220_TxStats_t;

void Drv_E220_Init(void);
uint8_t Drv_E220_IsReady(void);
void Drv_E220_Task(void);
BSP_Status_t Drv_E220_GetTxStats(Drv_E220_TxStats_t *stats);
void Drv_E220_ClearTxStats(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_E220_H */
