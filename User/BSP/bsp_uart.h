#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UART_RX_BUF_SIZE 256U
#define UART_TX_BUF_SIZE 512U

/*
 * 保持旧工程端口语义：
 * UART_PORT1 = E220（PB10/PB11，硬件 UART4）
 * UART_PORT2 = K210（PB4/PB5，硬件 UART1）
 */
typedef enum {
    UART_PORT1 = 0,
    UART_PORT2,
    UART_PORT_COUNT
} UART_Port_t;

typedef struct {
    uint16_t rx_overflow;
    uint16_t tx_drop;
    uint16_t rx_count;
    uint16_t tx_count;
} UART_Stats_t;

typedef uint8_t (*BSP_UART_TxReadyFn_t)(void);
typedef BSP_Status_t (*BSP_UART_TxDeferredFn_t)(
    const uint8_t *data, uint16_t len);

void BSP_UART_Init(UART_Port_t port);
void BSP_UART_InitAll(void);
void BSP_UART_SetTxReadyGuard(
    UART_Port_t port, BSP_UART_TxReadyFn_t guard);
void BSP_UART_SetTxDeferredHandler(
    UART_Port_t port, BSP_UART_TxDeferredFn_t handler);

uint16_t BSP_UART_Write(
    UART_Port_t port, const uint8_t *data, uint16_t len);
BSP_Status_t BSP_UART_WriteFrame(
    UART_Port_t port, const uint8_t *data, uint16_t len);
BSP_Status_t BSP_UART_WriteFrameNow(
    UART_Port_t port, const uint8_t *data, uint16_t len);
BSP_Status_t BSP_UART_SendData_NonBlocking(
    UART_Port_t port, const uint8_t *data, uint16_t len);

uint16_t BSP_UART_Read(UART_Port_t port, uint8_t *buf, uint16_t len);
uint8_t BSP_UART_GetChar(UART_Port_t port, uint8_t *ch);
uint16_t BSP_UART_Available(UART_Port_t port);
uint8_t BSP_UART_IsTxBusy(UART_Port_t port);
uint16_t BSP_UART_TxFree(UART_Port_t port);
void BSP_UART_FlushRx(UART_Port_t port);
void BSP_UART_FlushTx(UART_Port_t port);
BSP_Status_t BSP_UART_GetStats(UART_Port_t port, UART_Stats_t *stats);
void BSP_UART_ClearStats(UART_Port_t port);
void BSP_UART_Task(UART_Port_t port);
void BSP_UART_TaskAll(void);

/* 兼容旧 BSP 的公共 ISR 名称；MSPM0 不再使用独立 DMA Stream ISR。 */
void BSP_UART_USART_ISR(UART_Port_t port);
void BSP_UART_DMA_TX_ISR(UART_Port_t port);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_UART_H */
