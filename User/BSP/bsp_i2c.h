#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_BLOCK_TIMEOUT   300000UL
#define I2C_TX_COPY_BUF_LEN 32U
#define I2C_DMA_TIMEOUT_MS  100U

typedef enum {
    I2C_BUS1 = 0, /* PA0 SDA / PA1 SCL，I2C0，400 kHz */
    I2C_BUS_COUNT
} I2C_Bus_t;

typedef void (*I2C_Callback_t)(I2C_Bus_t bus, int result);

typedef struct {
    uint8_t state;
    uint8_t dev_addr;
    uint16_t tx_len;
    uint16_t tx_pos;
    uint16_t rx_len;
    uint8_t need_read;
    uint16_t sr1;
    uint16_t sr2;
    uint32_t start_tick;
    uint8_t error_source;
    uint8_t error_state;
    uint8_t error_dev_addr;
    uint16_t error_tx_pos;
    uint16_t error_tx_len;
    uint16_t error_rx_len;
    uint16_t error_sr1;
    uint16_t error_sr2;
    uint32_t error_count;

    /* 写后读组合事务的诊断快照和累计计数。 */
    uint8_t phase;
    uint8_t repeat_read_pending;
    uint8_t last_event;
    uint8_t last_iidx;
    uint8_t tx_first;
    uint8_t rx_first;
    uint16_t current_status;
    uint16_t line_state;
    uint16_t status_queue;
    uint16_t status_tx_done;
    uint16_t status_repeat_wait;
    uint16_t status_repeat_before;
    uint16_t status_repeat_after;
    uint16_t status_rx_done;
    uint32_t queue_count;
    uint32_t tx_done_count;
    uint32_t repeat_wait_count;
    uint32_t repeat_start_count;
    uint32_t rx_done_count;
    uint32_t nack_count;
    uint32_t arb_lost_count;
    uint32_t timeout_count;

    /* I2C_ERR_05/I2C_ERR_07联调所需的实时寄存器快照。 */
    uint32_t controller_config;
    uint32_t target_address;
    uint32_t controller_control;
    uint32_t fifo_status;
    uint32_t raw_interrupts;
    uint32_t enabled_interrupts;
    uint16_t dma_rx_remaining;
} BSP_I2C_Debug_t;

void BSP_I2C_Init(I2C_Bus_t bus);
void BSP_I2C_InitAll(void);
BSP_Status_t BSP_I2C_MasterWrite(
    I2C_Bus_t bus, uint8_t dev_addr, const uint8_t *data, uint16_t len);
BSP_Status_t BSP_I2C_MasterRead(
    I2C_Bus_t bus, uint8_t dev_addr, uint8_t *buf, uint16_t len);
BSP_Status_t BSP_I2C_MasterWriteRead(I2C_Bus_t bus, uint8_t dev_addr,
                                    const uint8_t *tx_data, uint16_t tx_len,
                                    uint8_t *rx_buf, uint16_t rx_len);
BSP_Status_t BSP_I2C_ScanBus(I2C_Bus_t bus, uint8_t *out_addr_list,
                             uint8_t max_count, uint8_t *out_found_count);

/*
 * 异步接口使用DMA_CH2/CH3和I2C0完成中断；
 * 写后读由RD_ON_TXEMPTY硬件重复START完成。
 */
BSP_Status_t BSP_I2C_MasterWrite_DMA_Async(
    I2C_Bus_t bus, uint8_t dev_addr,
    const uint8_t *tx_data, uint16_t tx_len, I2C_Callback_t callback);
BSP_Status_t BSP_I2C_MasterRead_DMA_Async(
    I2C_Bus_t bus, uint8_t dev_addr,
    uint8_t *rx_buf, uint16_t rx_len, I2C_Callback_t callback);
BSP_Status_t BSP_I2C_MasterWriteRead_DMA_Async(
    I2C_Bus_t bus, uint8_t dev_addr,
    const uint8_t *tx_data, uint16_t tx_len,
    uint8_t *rx_buf, uint16_t rx_len, I2C_Callback_t callback);

uint8_t BSP_I2C_IsBusy(I2C_Bus_t bus);
BSP_Status_t BSP_I2C_GetDebug(I2C_Bus_t bus, BSP_I2C_Debug_t *debug);
void BSP_I2C_Task(I2C_Bus_t bus);
void BSP_I2C_TaskAll(void);
void BSP_I2C_EV_ISR(I2C_Bus_t bus);
void BSP_I2C_ER_ISR(I2C_Bus_t bus);
void BSP_I2C_DMA_RX_ISR(I2C_Bus_t bus);
void BSP_I2C_DMA_TX_ISR(I2C_Bus_t bus);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_I2C_H */
