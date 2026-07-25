#include "bsp_i2c.h"
#include <string.h>

typedef enum {
    I2C_ASYNC_NONE = 0,
    I2C_ASYNC_WRITE,
    I2C_ASYNC_READ,
    I2C_ASYNC_WRITE_READ
} I2C_AsyncOp_t;

typedef enum {
    I2C_PHASE_IDLE = 0,
    I2C_PHASE_TX,
    I2C_PHASE_RX
} I2C_AsyncPhase_t;

typedef struct {
    volatile uint8_t busy;
    volatile uint8_t completion_pending;
    volatile BSP_Status_t completion_status;
    volatile I2C_AsyncPhase_t phase;
    I2C_AsyncOp_t op;
    uint8_t dev_addr;
    uint8_t tx_copy[I2C_TX_COPY_BUF_LEN];
    uint16_t tx_len;
    uint8_t *rx_buf;
    uint16_t rx_len;
    I2C_Callback_t callback;
    BSP_I2C_Debug_t debug;
} I2C_Runtime_t;

static I2C_Runtime_t s_i2c_rt[I2C_BUS_COUNT];

static void I2C_Abort(void)
{
    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
    DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);
    DL_I2C_resetControllerTransfer(I2C_SENSOR_INST);
    DL_I2C_flushControllerTXFIFO(I2C_SENSOR_INST);
    DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);
}

static BSP_Status_t I2C_WaitStatus(uint32_t mask, uint8_t wait_set)
{
    uint32_t timeout = I2C_BLOCK_TIMEOUT;

    while (timeout > 0U) {
        uint32_t hw_status = DL_I2C_getControllerStatus(I2C_SENSOR_INST);
        uint8_t is_set = ((hw_status & mask) != 0U) ? 1U : 0U;

        if ((hw_status & DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) {
            I2C_Abort();
            return BSP_ERROR;
        }
        if (is_set == wait_set) {
            return BSP_OK;
        }
        timeout--;
    }
    I2C_Abort();
    return BSP_TIMEOUT;
}

static BSP_Status_t I2C_WritePart(uint8_t dev_addr,
                                  const uint8_t *data,
                                  uint16_t len,
                                  uint8_t send_stop)
{
    uint16_t sent;
    BSP_Status_t status;

    if ((data == 0) || (len == 0U) || (dev_addr > 0x7FU)) {
        return BSP_PARAM;
    }

    DL_I2C_flushControllerTXFIFO(I2C_SENSOR_INST);
    sent = (uint16_t)DL_I2C_fillControllerTXFIFO(
        I2C_SENSOR_INST, data, len);
    DL_I2C_startControllerTransferAdvanced(I2C_SENSOR_INST, dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_TX, len,
        DL_I2C_CONTROLLER_START_ENABLE,
        (send_stop != 0U) ? DL_I2C_CONTROLLER_STOP_ENABLE :
                            DL_I2C_CONTROLLER_STOP_DISABLE,
        DL_I2C_CONTROLLER_ACK_ENABLE);

    while (sent < len) {
        uint32_t timeout = I2C_BLOCK_TIMEOUT;
        while (DL_I2C_isControllerTXFIFOFull(I2C_SENSOR_INST) &&
               (timeout > 0U)) {
            timeout--;
        }
        if (timeout == 0U) {
            I2C_Abort();
            return BSP_TIMEOUT;
        }
        sent += (uint16_t)DL_I2C_fillControllerTXFIFO(
            I2C_SENSOR_INST, &data[sent], (uint16_t)(len - sent));
    }

    status = I2C_WaitStatus(DL_I2C_CONTROLLER_STATUS_BUSY, 0U);
    if ((status == BSP_OK) && (send_stop != 0U)) {
        status = I2C_WaitStatus(DL_I2C_CONTROLLER_STATUS_BUSY_BUS, 0U);
    }
    return status;
}

static BSP_Status_t I2C_ReadPart(uint8_t dev_addr,
                                 uint8_t *buf,
                                 uint16_t len)
{
    uint16_t received;
    BSP_Status_t status;

    if ((buf == 0) || (len == 0U) || (dev_addr > 0x7FU)) {
        return BSP_PARAM;
    }

    DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);
    DL_I2C_startControllerTransferAdvanced(I2C_SENSOR_INST, dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_RX, len,
        DL_I2C_CONTROLLER_START_ENABLE,
        DL_I2C_CONTROLLER_STOP_ENABLE,
        DL_I2C_CONTROLLER_ACK_ENABLE);

    received = 0U;
    while (received < len) {
        uint32_t timeout = I2C_BLOCK_TIMEOUT;
        while (DL_I2C_isControllerRXFIFOEmpty(I2C_SENSOR_INST) &&
               (timeout > 0U)) {
            if ((DL_I2C_getControllerStatus(I2C_SENSOR_INST) &
                 DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) {
                I2C_Abort();
                return BSP_ERROR;
            }
            timeout--;
        }
        if (timeout == 0U) {
            I2C_Abort();
            return BSP_TIMEOUT;
        }
        buf[received] = DL_I2C_receiveControllerData(I2C_SENSOR_INST);
        received++;
    }

    status = I2C_WaitStatus(DL_I2C_CONTROLLER_STATUS_BUSY, 0U);
    if (status == BSP_OK) {
        status = I2C_WaitStatus(DL_I2C_CONTROLLER_STATUS_BUSY_BUS, 0U);
    }
    return status;
}

void BSP_I2C_Init(I2C_Bus_t bus)
{
    if (bus < I2C_BUS_COUNT) {
        memset(&s_i2c_rt[bus], 0, sizeof(s_i2c_rt[bus]));
        DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
        DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);
        NVIC_ClearPendingIRQ(I2C_SENSOR_INST_INT_IRQN);
        NVIC_EnableIRQ(I2C_SENSOR_INST_INT_IRQN);
    }
}

void BSP_I2C_InitAll(void)
{
    BSP_I2C_Init(I2C_BUS1);
}

BSP_Status_t BSP_I2C_MasterWrite(
    I2C_Bus_t bus, uint8_t dev_addr, const uint8_t *data, uint16_t len)
{
    BSP_Status_t status;

    if ((bus >= I2C_BUS_COUNT) || (s_i2c_rt[bus].busy != 0U)) {
        return (bus >= I2C_BUS_COUNT) ? BSP_PARAM : BSP_BUSY;
    }
    status = I2C_WaitStatus(DL_I2C_CONTROLLER_STATUS_IDLE, 1U);
    return (status == BSP_OK) ?
               I2C_WritePart(dev_addr, data, len, 1U) : status;
}

BSP_Status_t BSP_I2C_MasterRead(
    I2C_Bus_t bus, uint8_t dev_addr, uint8_t *buf, uint16_t len)
{
    BSP_Status_t status;

    if ((bus >= I2C_BUS_COUNT) || (s_i2c_rt[bus].busy != 0U)) {
        return (bus >= I2C_BUS_COUNT) ? BSP_PARAM : BSP_BUSY;
    }
    status = I2C_WaitStatus(DL_I2C_CONTROLLER_STATUS_IDLE, 1U);
    return (status == BSP_OK) ? I2C_ReadPart(dev_addr, buf, len) : status;
}

BSP_Status_t BSP_I2C_MasterWriteRead(I2C_Bus_t bus, uint8_t dev_addr,
                                    const uint8_t *tx_data, uint16_t tx_len,
                                    uint8_t *rx_buf, uint16_t rx_len)
{
    BSP_Status_t status;

    if ((bus >= I2C_BUS_COUNT) || (s_i2c_rt[bus].busy != 0U)) {
        return (bus >= I2C_BUS_COUNT) ? BSP_PARAM : BSP_BUSY;
    }
    status = I2C_WaitStatus(DL_I2C_CONTROLLER_STATUS_IDLE, 1U);
    if (status == BSP_OK) {
        status = I2C_WritePart(dev_addr, tx_data, tx_len, 0U);
    }
    if (status == BSP_OK) {
        status = I2C_ReadPart(dev_addr, rx_buf, rx_len);
    }
    return status;
}

static BSP_Status_t I2C_Probe(uint8_t dev_addr)
{
    const uint32_t event_mask =
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_NACK |
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST;
    uint32_t timeout;
    uint32_t events;
    uint32_t controller_status;

    if (dev_addr > 0x7FU) {
        return BSP_PARAM;
    }

    controller_status = DL_I2C_getControllerStatus(I2C_SENSOR_INST);
    if (((controller_status & DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) ||
        ((controller_status & DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U)) {
        return BSP_BUSY;
    }

    /*
     * 地址扫描使用零字节写事务，只发送START、7位地址和STOP。
     * 不能在启动后立刻检查BUSY=0，因为硬件可能尚未来得及置位BUSY。
     * 必须等待TX_DONE或NACK，才能判断目标地址是否真正应答。
     */
    DL_I2C_resetControllerTransfer(I2C_SENSOR_INST);
    DL_I2C_flushControllerTXFIFO(I2C_SENSOR_INST);
    DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);
    DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);

    DL_I2C_startControllerTransferAdvanced(
        I2C_SENSOR_INST,
        dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_TX,
        0U,
        DL_I2C_CONTROLLER_START_ENABLE,
        DL_I2C_CONTROLLER_STOP_ENABLE,
        DL_I2C_CONTROLLER_ACK_ENABLE);

    timeout = I2C_BLOCK_TIMEOUT;
    while (timeout > 0U) {
        events = DL_I2C_getRawInterruptStatus(
            I2C_SENSOR_INST,
            event_mask);

        if ((events & DL_I2C_INTERRUPT_CONTROLLER_NACK) != 0U) {
            DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);
            I2C_Abort();
            return BSP_ERROR;
        }

        if ((events &
             DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST) != 0U) {
            DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);
            I2C_Abort();
            return BSP_BUSY;
        }

        if ((events & DL_I2C_INTERRUPT_CONTROLLER_TX_DONE) != 0U) {
            DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);

            timeout = I2C_BLOCK_TIMEOUT;
            while (((DL_I2C_getControllerStatus(I2C_SENSOR_INST) &
                     DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U) &&
                   (timeout > 0U)) {
                timeout--;
            }

            if (timeout == 0U) {
                I2C_Abort();
                return BSP_TIMEOUT;
            }

            DL_I2C_resetControllerTransfer(I2C_SENSOR_INST);
            return BSP_OK;
        }

        timeout--;
    }

    DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);
    I2C_Abort();
    return BSP_TIMEOUT;
}

BSP_Status_t BSP_I2C_ScanBus(I2C_Bus_t bus,
                              uint8_t *out_addr_list,
                              uint8_t max_count,
                              uint8_t *out_found_count)
{
    const uint32_t event_mask =
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_NACK |
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST;
    BSP_Status_t result = BSP_OK;
    BSP_Status_t probe_status;
    uint32_t controller_status;
    uint8_t address;
    uint8_t found = 0U;

    if ((bus >= I2C_BUS_COUNT) ||
        (out_addr_list == 0) ||
        (out_found_count == 0) ||
        (max_count == 0U)) {
        return BSP_PARAM;
    }

    *out_found_count = 0U;

    /*
     * 扫描是同步独占操作。异步OLED、灰度或测距事务正在运行时，
     * 不允许插入地址探测，否则会破坏DMA状态机。
     */
    if (s_i2c_rt[bus].busy != 0U) {
        return BSP_BUSY;
    }

    controller_status = DL_I2C_getControllerStatus(I2C_SENSOR_INST);
    if (((controller_status & DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) ||
        ((controller_status & DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U)) {
        return BSP_BUSY;
    }

    /*
     * I2C中断处理函数会读取IIDX并消费TX_DONE/NACK。
     * 扫描期间暂时关闭本I2C的NVIC入口，由I2C_Probe直接读取原始中断状态。
     */
    NVIC_DisableIRQ(I2C_SENSOR_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(I2C_SENSOR_INST_INT_IRQN);
    DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);

    for (address = 0x08U; address <= 0x77U; address++) {
        probe_status = I2C_Probe(address);

        if (probe_status == BSP_OK) {
            if (found < max_count) {
                out_addr_list[found] = address;
                found++;
            }
        } else if (probe_status == BSP_ERROR) {
            /* 地址NACK表示该地址没有设备，继续扫描。 */
        } else {
            result = probe_status;
            break;
        }
    }

    DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);
    DL_I2C_resetControllerTransfer(I2C_SENSOR_INST);
    NVIC_ClearPendingIRQ(I2C_SENSOR_INST_INT_IRQN);
    NVIC_EnableIRQ(I2C_SENSOR_INST_INT_IRQN);

    *out_found_count = found;
    return result;
}

static BSP_Status_t I2C_Queue(I2C_Bus_t bus,
                              I2C_AsyncOp_t op,
                              uint8_t dev_addr,
                              const uint8_t *tx_data,
                              uint16_t tx_len,
                              uint8_t *rx_buf,
                              uint16_t rx_len,
                              I2C_Callback_t callback)
{
    I2C_Runtime_t *rt;
    uint32_t key;
    uint32_t controller_status;

    if ((bus >= I2C_BUS_COUNT) || (dev_addr > 0x7FU) ||
        (tx_len > I2C_TX_COPY_BUF_LEN) ||
        ((tx_len != 0U) && (tx_data == 0)) ||
        ((rx_len != 0U) && (rx_buf == 0))) {
        return BSP_PARAM;
    }

    controller_status = DL_I2C_getControllerStatus(I2C_SENSOR_INST);
    if ((controller_status & DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) {
        return BSP_BUSY;
    }

    rt = &s_i2c_rt[bus];
    key = BSP_EnterCritical();
    if (rt->busy != 0U) {
        BSP_ExitCritical(key);
        return BSP_BUSY;
    }
    rt->busy = 1U;
    rt->op = op;
    rt->dev_addr = dev_addr;
    rt->tx_len = tx_len;
    rt->rx_buf = rx_buf;
    rt->rx_len = rx_len;
    rt->callback = callback;
    rt->completion_pending = 0U;
    rt->completion_status = BSP_BUSY;
    rt->phase = I2C_PHASE_IDLE;
    if (tx_len != 0U) {
        memcpy(rt->tx_copy, tx_data, tx_len);
    }
    rt->debug.state = (uint8_t)op;
    rt->debug.dev_addr = dev_addr;
    rt->debug.tx_len = tx_len;
    rt->debug.rx_len = rx_len;
    rt->debug.need_read = (rx_len != 0U) ? 1U : 0U;
    rt->debug.start_tick = BSP_GET_TICK();
    BSP_ExitCritical(key);

    if (op == I2C_ASYNC_READ) {
        DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);
        DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);
        DL_DMA_setSrcAddr(DMA, DMA_CH3_CHAN_ID,
            (uint32_t)&I2C_SENSOR_INST->MASTER.MRXDATA);
        DL_DMA_setDestAddr(
            DMA, DMA_CH3_CHAN_ID, (uint32_t)rt->rx_buf);
        DL_DMA_setTransferSize(DMA, DMA_CH3_CHAN_ID, rt->rx_len);
        rt->phase = I2C_PHASE_RX;
        DL_DMA_enableChannel(DMA, DMA_CH3_CHAN_ID);
        DL_I2C_startControllerTransferAdvanced(
            I2C_SENSOR_INST, rt->dev_addr,
            DL_I2C_CONTROLLER_DIRECTION_RX, rt->rx_len,
            DL_I2C_CONTROLLER_START_ENABLE,
            DL_I2C_CONTROLLER_STOP_ENABLE,
            DL_I2C_CONTROLLER_ACK_ENABLE);
    } else {
        DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
        DL_I2C_flushControllerTXFIFO(I2C_SENSOR_INST);
        DL_DMA_setSrcAddr(
            DMA, DMA_CH2_CHAN_ID, (uint32_t)rt->tx_copy);
        DL_DMA_setDestAddr(DMA, DMA_CH2_CHAN_ID,
            (uint32_t)&I2C_SENSOR_INST->MASTER.MTXDATA);
        DL_DMA_setTransferSize(DMA, DMA_CH2_CHAN_ID, rt->tx_len);
        rt->phase = I2C_PHASE_TX;
        DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
        DL_I2C_startControllerTransferAdvanced(
            I2C_SENSOR_INST, rt->dev_addr,
            DL_I2C_CONTROLLER_DIRECTION_TX, rt->tx_len,
            DL_I2C_CONTROLLER_START_ENABLE,
            (op == I2C_ASYNC_WRITE) ?
                DL_I2C_CONTROLLER_STOP_ENABLE :
                DL_I2C_CONTROLLER_STOP_DISABLE,
            DL_I2C_CONTROLLER_ACK_ENABLE);
    }

    return BSP_OK;
}

BSP_Status_t BSP_I2C_MasterWrite_DMA_Async(
    I2C_Bus_t bus, uint8_t dev_addr,
    const uint8_t *tx_data, uint16_t tx_len, I2C_Callback_t callback)
{
    return I2C_Queue(bus, I2C_ASYNC_WRITE, dev_addr,
                     tx_data, tx_len, 0, 0U, callback);
}

BSP_Status_t BSP_I2C_MasterRead_DMA_Async(
    I2C_Bus_t bus, uint8_t dev_addr,
    uint8_t *rx_buf, uint16_t rx_len, I2C_Callback_t callback)
{
    return I2C_Queue(bus, I2C_ASYNC_READ, dev_addr,
                     0, 0U, rx_buf, rx_len, callback);
}

BSP_Status_t BSP_I2C_MasterWriteRead_DMA_Async(
    I2C_Bus_t bus, uint8_t dev_addr,
    const uint8_t *tx_data, uint16_t tx_len,
    uint8_t *rx_buf, uint16_t rx_len, I2C_Callback_t callback)
{
    return I2C_Queue(bus, I2C_ASYNC_WRITE_READ, dev_addr,
                     tx_data, tx_len, rx_buf, rx_len, callback);
}

uint8_t BSP_I2C_IsBusy(I2C_Bus_t bus)
{
    if (bus >= I2C_BUS_COUNT) {
        return 0U;
    }
    return (s_i2c_rt[bus].busy != 0U) ||
           ((DL_I2C_getControllerStatus(I2C_SENSOR_INST) &
             DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U) ? 1U : 0U;
}

BSP_Status_t BSP_I2C_GetDebug(I2C_Bus_t bus, BSP_I2C_Debug_t *debug)
{
    if ((bus >= I2C_BUS_COUNT) || (debug == 0)) {
        return BSP_PARAM;
    }
    *debug = s_i2c_rt[bus].debug;
    debug->sr1 = (uint16_t)DL_I2C_getControllerStatus(I2C_SENSOR_INST);
    return BSP_OK;
}

void BSP_I2C_Task(I2C_Bus_t bus)
{
    I2C_Runtime_t *rt;
    I2C_Callback_t callback;
    BSP_Status_t status;

    if ((bus >= I2C_BUS_COUNT) || (s_i2c_rt[bus].busy == 0U)) {
        return;
    }

    rt = &s_i2c_rt[bus];
    if (rt->completion_pending == 0U) {
        if ((uint32_t)(BSP_GET_TICK() - rt->debug.start_tick) <
            I2C_DMA_TIMEOUT_MS) {
            return;
        }
        I2C_Abort();
        rt->completion_status = BSP_TIMEOUT;
        rt->completion_pending = 1U;
    }

    status = rt->completion_status;
    rt->debug.state = 0U;
    rt->debug.tx_pos = (status == BSP_OK) ? rt->tx_len : 0U;
    if (status != BSP_OK) {
        rt->debug.error_count++;
        rt->debug.error_source = (status == BSP_TIMEOUT) ? 2U : 1U;
        rt->debug.error_state = (uint8_t)rt->op;
        rt->debug.error_dev_addr = rt->dev_addr;
        rt->debug.error_tx_len = rt->tx_len;
        rt->debug.error_rx_len = rt->rx_len;
        rt->debug.error_sr1 =
            (uint16_t)DL_I2C_getControllerStatus(I2C_SENSOR_INST);
    }
    callback = rt->callback;
    rt->busy = 0U;
    rt->op = I2C_ASYNC_NONE;
    rt->phase = I2C_PHASE_IDLE;
    rt->completion_pending = 0U;
    if (callback != 0) {
        callback(bus, (status == BSP_OK) ? 0 :
                      ((status == BSP_TIMEOUT) ? -2 : -1));
    }
}

void BSP_I2C_TaskAll(void)
{
    BSP_I2C_Task(I2C_BUS1);
}

static void I2C_MarkCompletion(BSP_Status_t status)
{
    I2C_Runtime_t *rt = &s_i2c_rt[I2C_BUS1];

    if ((rt->busy != 0U) && (rt->completion_pending == 0U)) {
        rt->completion_status = status;
        rt->completion_pending = 1U;
    }
}

static void I2C_StartRepeatedRead(void)
{
    I2C_Runtime_t *rt = &s_i2c_rt[I2C_BUS1];

    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
    DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);
    DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);
    DL_DMA_setSrcAddr(DMA, DMA_CH3_CHAN_ID,
        (uint32_t)&I2C_SENSOR_INST->MASTER.MRXDATA);
    DL_DMA_setDestAddr(
        DMA, DMA_CH3_CHAN_ID, (uint32_t)rt->rx_buf);
    DL_DMA_setTransferSize(DMA, DMA_CH3_CHAN_ID, rt->rx_len);
    rt->phase = I2C_PHASE_RX;
    DL_DMA_enableChannel(DMA, DMA_CH3_CHAN_ID);
    DL_I2C_startControllerTransferAdvanced(
        I2C_SENSOR_INST, rt->dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_RX, rt->rx_len,
        DL_I2C_CONTROLLER_START_ENABLE,
        DL_I2C_CONTROLLER_STOP_ENABLE,
        DL_I2C_CONTROLLER_ACK_ENABLE);
}

static void I2C_HandleInterrupt(void)
{
    I2C_Runtime_t *rt = &s_i2c_rt[I2C_BUS1];
    DL_I2C_IIDX iidx;

    do {
        iidx = DL_I2C_getPendingInterrupt(I2C_SENSOR_INST);
        if ((iidx == DL_I2C_IIDX_CONTROLLER_NACK) ||
            (iidx == DL_I2C_IIDX_CONTROLLER_ARBITRATION_LOST)) {
            if (rt->busy != 0U) {
                I2C_Abort();
                I2C_MarkCompletion(BSP_ERROR);
            }
        } else if (iidx == DL_I2C_IIDX_CONTROLLER_TX_DONE) {
            if ((rt->busy != 0U) && (rt->phase == I2C_PHASE_TX)) {
                DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
                if (rt->op == I2C_ASYNC_WRITE_READ) {
                    I2C_StartRepeatedRead();
                } else {
                    I2C_MarkCompletion(BSP_OK);
                }
            }
        } else if (iidx == DL_I2C_IIDX_CONTROLLER_RX_DONE) {
            if ((rt->busy != 0U) && (rt->phase == I2C_PHASE_RX)) {
                DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);
                I2C_MarkCompletion(BSP_OK);
            }
        }
    } while ((uint32_t)iidx != 0U);
}

void BSP_I2C_EV_ISR(I2C_Bus_t bus)
{
    if (bus == I2C_BUS1) {
        I2C_HandleInterrupt();
    }
}

void BSP_I2C_ER_ISR(I2C_Bus_t bus)
{
    BSP_I2C_EV_ISR(bus);
}

void BSP_I2C_DMA_RX_ISR(I2C_Bus_t bus)
{
    BSP_I2C_EV_ISR(bus);
}

void BSP_I2C_DMA_TX_ISR(I2C_Bus_t bus)
{
    BSP_I2C_EV_ISR(bus);
}

void I2C_SENSOR_INST_IRQHandler(void)
{
    I2C_HandleInterrupt();
}
