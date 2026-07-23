#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_BLOCK_TIMEOUT 100000UL
#define SPI_DMA_TIMEOUT_MS 100U

typedef enum {
    SPI_BUS1 = 0, /* TFT/OLED 显示：PB3/PB2，SPI0，8 MHz */
    SPI_BUS2,     /* 外接 ICM20948：PB16/PB15/PB14，SPI1，4 MHz */
    SPI_BUS_COUNT
} SPI_Bus_t;

typedef void (*SPI_Callback_t)(
    SPI_Bus_t bus, void *ctx, BSP_Status_t status);

void BSP_SPI_Init(SPI_Bus_t bus);
void BSP_SPI_InitAll(void);
uint8_t BSP_SPI_TransferByte(SPI_Bus_t bus, uint8_t tx, uint8_t *rx);
BSP_Status_t BSP_SPI_Transfer(SPI_Bus_t bus,
                             const uint8_t *tx_buf,
                             uint8_t *rx_buf,
                             uint16_t len,
                             uint8_t dummy_tx);
uint8_t BSP_SPI_ReadWriteByte(SPI_Bus_t bus, uint8_t data);

/*
 * SPI_BUS1（显示 SPI0）使用 DMA_CH0/CH1 真正异步传输。
 * SPI_BUS2 当前没有异步调用者，若以后调用该接口则使用任务兼容后端。
 */
BSP_Status_t BSP_SPI_TransferAsync_DMA(SPI_Bus_t bus,
                                      uint8_t *tx_buf,
                                      uint8_t *rx_buf,
                                      uint16_t len,
                                      SPI_Callback_t cb,
                                      void *ctx);
uint8_t BSP_SPI_IsBusy(SPI_Bus_t bus);
BSP_Status_t BSP_SPI_WaitIdle(SPI_Bus_t bus);
void BSP_SPI_Task(SPI_Bus_t bus);
void BSP_SPI_TaskAll(void);
void BSP_SPI_DMA_RX_ISR(SPI_Bus_t bus);
void BSP_SPI_DMA_TX_ISR(SPI_Bus_t bus);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_SPI_H */
