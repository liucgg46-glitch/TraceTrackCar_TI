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
    I2C_PHASE_RX,
    I2C_PHASE_REPEAT_WAIT
} I2C_AsyncPhase_t;

typedef struct {
    volatile uint8_t busy;
    volatile uint8_t completion_pending;
    volatile BSP_Status_t completion_status;
    volatile I2C_AsyncPhase_t phase;
    volatile uint8_t repeat_read_pending;
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

#define I2C_REPEAT_START_DIAG_FIX_V2 1U
#define I2C_REPEAT_START_RUNTIME_FIX_V3 1U
#define I2C_STARTUP_STALE_BUS_FIX_V4_1 1U
#define I2C_BUSBSY_PREFLIGHT_FIX_V5 1U
#define I2C_ONE_BYTE_TX_FIFO_FIX_V6 1U
#define I2C_HW_READ_ON_TX_EMPTY_FIX_V7 1U
#define I2C_MCTR_SINGLE_WRITE_ERRATA_FIX_V8 1U

#define I2C_CONTROLLER_TX_FIFO_DEPTH 8U
#define I2C_CONTROLLER_START_SETTLE_CYCLES 32U

#define I2C_DIAG_EVENT_NONE          0U
#define I2C_DIAG_EVENT_TX_DONE       1U
#define I2C_DIAG_EVENT_REPEAT_WAIT   2U
#define I2C_DIAG_EVENT_RX_START      3U
#define I2C_DIAG_EVENT_RX_DONE       4U
#define I2C_DIAG_EVENT_NACK          5U
#define I2C_DIAG_EVENT_ARB_LOST      6U
#define I2C_DIAG_EVENT_TIMEOUT       7U
#define I2C_DIAG_EVENT_QUEUE         8U
#define I2C_DIAG_EVENT_TX_START      9U
#define I2C_DIAG_EVENT_HW_REPEAT    10U

static void I2C_HandleInterrupt(void);
static void I2C_MarkCompletion(BSP_Status_t status);
static void I2C_StartControllerTransfer(
    uint8_t dev_addr,
    DL_I2C_CONTROLLER_DIRECTION direction,
    uint16_t length,
    DL_I2C_CONTROLLER_START start,
    DL_I2C_CONTROLLER_STOP stop,
    DL_I2C_CONTROLLER_ACK ack,
    uint8_t read_on_tx_empty);

static uint16_t I2C_GetLineState(void)
{
    return (uint16_t)(I2C_SENSOR_INST->MASTER.MBMON & 0x03U);
}

static void I2C_RecordDebugEvent(I2C_Runtime_t *rt,
                                 uint8_t event,
                                 uint32_t iidx)
{
    if (rt == 0) {
        return;
    }

    rt->debug.phase = (uint8_t)rt->phase;
    rt->debug.repeat_read_pending = rt->repeat_read_pending;
    rt->debug.last_event = event;
    rt->debug.last_iidx = (uint8_t)iidx;
    rt->debug.current_status =
        (uint16_t)DL_I2C_getControllerStatus(I2C_SENSOR_INST);
    rt->debug.line_state = I2C_GetLineState();
}

static void I2C_Abort(void)
{
    const uint32_t clear_mask =
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_RX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_NACK |
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST;

    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
    DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);

    /*
     * MSPM0Gx51x勘误I2C_ERR_05禁止在事务进行中切换ACTIVE，
     * 否则可能出现Controller已IDLE但SDA持续为低、BUSBSY无法清除。
     * 只有Controller BUSY已经清零时才复位事务控制和FIFO。
     */
    if ((DL_I2C_getControllerStatus(I2C_SENSOR_INST) &
         DL_I2C_CONTROLLER_STATUS_BUSY) == 0U) {
        DL_I2C_resetControllerTransfer(I2C_SENSOR_INST);
        DL_I2C_flushControllerTXFIFO(I2C_SENSOR_INST);
        DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);
    }
    DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, clear_mask);
    NVIC_ClearPendingIRQ(I2C_SENSOR_INST_INT_IRQN);

    s_i2c_rt[I2C_BUS1].repeat_read_pending = 0U;
}

static void I2C_StartControllerTransfer(
    uint8_t dev_addr,
    DL_I2C_CONTROLLER_DIRECTION direction,
    uint16_t length,
    DL_I2C_CONTROLLER_START start,
    DL_I2C_CONTROLLER_STOP stop,
    DL_I2C_CONTROLLER_ACK ack,
    uint8_t read_on_tx_empty)
{
    uint32_t mctr;
    volatile uint32_t settle_cycles;

    /*
     * MSPM0Gx51x勘误I2C_ERR_07要求CTR.START和其余控制位一次写入。
     * 这里不调用会读改写MCTR的启动辅助函数，避免连续MCTR写导致
     * 只产生START电平而没有后续SCL脉冲。
     */
    DL_Common_updateReg(
        &I2C_SENSOR_INST->MASTER.MSA,
        (((uint32_t)dev_addr << I2C_MSA_SADDR_OFS) |
         (uint32_t)direction),
        (I2C_MSA_SADDR_MASK | I2C_MSA_DIR_MASK));

    mctr =
        ((uint32_t)length << I2C_MCTR_MBLEN_OFS) |
        I2C_MCTR_BURSTRUN_ENABLE |
        (uint32_t)start |
        (uint32_t)stop |
        (uint32_t)ack;

    if (read_on_tx_empty != 0U) {
        mctr |= I2C_MCTR_RD_ON_TXEMPTY_ENABLE;
    }

    I2C_SENSOR_INST->MASTER.MCTR = mctr;

    /*
     * I2C_ERR_13说明BURSTRUN置位后约3个I2C功能时钟BUSY才有效。
     * 当前I2C功能时钟为BUSCLK/1，保留32个CPU空循环，确保同步短
     * 事务不会在BUSY置位前误判完成并启动下一笔MCTR写。
     */
    settle_cycles = I2C_CONTROLLER_START_SETTLE_CYCLES;
    while (settle_cycles > 0U) {
        __NOP();
        settle_cycles--;
    }
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

static uint8_t I2C_WaitBusFree(void)
{
    uint32_t timeout = I2C_BLOCK_TIMEOUT;

    while (timeout > 0U) {
        uint32_t status =
            DL_I2C_getControllerStatus(I2C_SENSOR_INST);

        if (((status & DL_I2C_CONTROLLER_STATUS_BUSY) == 0U) &&
            ((status & DL_I2C_CONTROLLER_STATUS_BUSY_BUS) == 0U)) {
            return 1U;
        }
        timeout--;
    }

    return 0U;
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
    I2C_StartControllerTransfer(
        dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_TX,
        len,
        DL_I2C_CONTROLLER_START_ENABLE,
        (send_stop != 0U) ? DL_I2C_CONTROLLER_STOP_ENABLE :
                            DL_I2C_CONTROLLER_STOP_DISABLE,
        DL_I2C_CONTROLLER_ACK_DISABLE,
        0U);

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
    I2C_StartControllerTransfer(
        dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_RX,
        len,
        DL_I2C_CONTROLLER_START_ENABLE,
        DL_I2C_CONTROLLER_STOP_ENABLE,
        DL_I2C_CONTROLLER_ACK_DISABLE,
        0U);

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
    if (bus >= I2C_BUS_COUNT) {
        return;
    }

    memset(&s_i2c_rt[bus], 0, sizeof(s_i2c_rt[bus]));

    DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
    DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);

    NVIC_DisableIRQ(I2C_SENSOR_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(I2C_SENSOR_INST_INT_IRQN);
    NVIC_EnableIRQ(I2C_SENSOR_INST_INT_IRQN);

    /*
     * SysConfig已经完成模块复位和Controller使能。
     * BSP初始化阶段只清软件状态，不重复切换ACTIVE。
     */
}

void BSP_I2C_InitAll(void)
{
    BSP_I2C_Init(I2C_BUS1);
}

BSP_Status_t BSP_I2C_MasterWrite(
    I2C_Bus_t bus, uint8_t dev_addr, const uint8_t *data, uint16_t len)
{
    BSP_Status_t status;

    if ((bus >= I2C_BUS_COUNT) ||
        (BSP_I2C_IsBusy(bus) != 0U)) {
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

    if ((bus >= I2C_BUS_COUNT) ||
        (BSP_I2C_IsBusy(bus) != 0U)) {
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

    if ((bus >= I2C_BUS_COUNT) ||
        (BSP_I2C_IsBusy(bus) != 0U)) {
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
    if (((controller_status & DL_I2C_CONTROLLER_STATUS_BUSY) != 0U) ||
        ((controller_status & DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) ||
        (((controller_status &
           DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U) &&
         (I2C_GetLineState() != 0x03U))) {
        return BSP_BUSY;
    }

    /*
     * 地址扫描使用零字节写事务，只发送START、7位地址和STOP。
     * 不能在启动后立刻检查BUSY=0，因为硬件可能尚未来得及置位BUSY。
     * 必须等待TX_DONE或NACK，才能判断目标地址是否真正应答。
     */
    DL_I2C_flushControllerTXFIFO(I2C_SENSOR_INST);
    DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);
    DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);

    I2C_StartControllerTransfer(
        dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_TX,
        0U,
        DL_I2C_CONTROLLER_START_ENABLE,
        DL_I2C_CONTROLLER_STOP_ENABLE,
        DL_I2C_CONTROLLER_ACK_DISABLE,
        0U);

    timeout = I2C_BLOCK_TIMEOUT;
    while (timeout > 0U) {
        events = DL_I2C_getRawInterruptStatus(
            I2C_SENSOR_INST,
            event_mask);

        if ((events & DL_I2C_INTERRUPT_CONTROLLER_NACK) != 0U) {
            DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);
            /*
             * NACK后硬件会自动产生STOP。必须等STOP完成再探测下一地址，
             * 不能在事务尚未收尾时切换ACTIVE。
             */
            return (I2C_WaitBusFree() != 0U) ?
                   BSP_ERROR : BSP_TIMEOUT;
        }

        if ((events &
             DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST) != 0U) {
            DL_I2C_clearInterruptStatus(I2C_SENSOR_INST, event_mask);
            return (I2C_WaitBusFree() != 0U) ?
                   BSP_BUSY : BSP_TIMEOUT;
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
    if (((controller_status & DL_I2C_CONTROLLER_STATUS_BUSY) != 0U) ||
        ((controller_status & DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) ||
        (((controller_status &
           DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U) &&
         (I2C_GetLineState() != 0x03U))) {
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
    uint16_t fifo_written;
    const uint32_t event_mask =
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_RX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_NACK |
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST;

    if ((bus >= I2C_BUS_COUNT) || (dev_addr > 0x7FU) ||
        (tx_len > I2C_TX_COPY_BUF_LEN) ||
        ((tx_len != 0U) && (tx_data == 0)) ||
        ((rx_len != 0U) && (rx_buf == 0)) ||
        ((op == I2C_ASYNC_WRITE_READ) &&
         ((tx_len == 0U) ||
          (tx_len > I2C_CONTROLLER_TX_FIFO_DEPTH) ||
          (rx_len == 0U)))) {
        return BSP_PARAM;
    }

    rt = &s_i2c_rt[bus];
    if (rt->busy != 0U) {
        return BSP_BUSY;
    }

    controller_status =
        DL_I2C_getControllerStatus(I2C_SENSOR_INST);

    /*
     * 不使用BUSBSY作为排队硬门槛。
     * 只要Controller FSM空闲，就允许实际启动事务。
     */
    if (((controller_status &
          DL_I2C_CONTROLLER_STATUS_BUSY) != 0U) ||
        ((controller_status &
          DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) ||
        (((controller_status &
           DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U) &&
         (I2C_GetLineState() != 0x03U))) {
        return BSP_BUSY;
    }

    key = BSP_EnterCritical();
    if (rt->busy != 0U) {
        BSP_ExitCritical(key);
        return BSP_BUSY;
    }

    /*
     * 在发布busy状态前清除上一笔事务的完成和错误标志，避免旧事件
     * 在退出临界区后被当作新事务结果。
     */
    DL_I2C_clearInterruptStatus(
        I2C_SENSOR_INST,
        event_mask);

    rt->busy = 1U;
    rt->completion_pending = 0U;
    rt->completion_status = BSP_BUSY;
    rt->repeat_read_pending = 0U;
    rt->phase = I2C_PHASE_IDLE;
    rt->op = op;
    rt->dev_addr = dev_addr;
    rt->tx_len = tx_len;
    rt->rx_buf = rx_buf;
    rt->rx_len = rx_len;
    rt->callback = callback;

    if (tx_len != 0U) {
        memcpy(rt->tx_copy, tx_data, tx_len);
    }

    rt->debug.state = (uint8_t)op;
    rt->debug.dev_addr = dev_addr;
    rt->debug.tx_len = tx_len;
    rt->debug.tx_pos = 0U;
    rt->debug.rx_len = rx_len;
    rt->debug.need_read = (rx_len != 0U) ? 1U : 0U;
    rt->debug.start_tick = BSP_GET_TICK();
    rt->debug.tx_first =
        (tx_len != 0U) ? rt->tx_copy[0] : 0U;
    rt->debug.rx_first = 0U;
    rt->debug.status_queue =
        (uint16_t)controller_status;
    rt->debug.queue_count++;

    I2C_RecordDebugEvent(
        rt,
        I2C_DIAG_EVENT_QUEUE,
        (uint32_t)DL_I2C_IIDX_NO_INT);

    BSP_ExitCritical(key);

    if (op == I2C_ASYNC_WRITE_READ) {
        /*
         * MSPM0的RD_ON_TXEMPTY专门用于命令式读取：
         * 先以写地址发完TX FIFO，再由硬件在不发STOP的情况下产生
         * repeated START，并按MSA中的读方向接收MBLEN字节。
         *
         * MCTR的BURSTRUN不会自动清零，同时I2C_ERR_07禁止连续写MCTR。
         * 因此由I2C_StartControllerTransfer()一次写入RD_ON_TXEMPTY、
         * MBLEN、START、STOP和BURSTRUN全部字段。
         */
        DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
        DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);
        DL_I2C_flushControllerTXFIFO(I2C_SENSOR_INST);
        DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);

        fifo_written = DL_I2C_fillControllerTXFIFO(
            I2C_SENSOR_INST,
            rt->tx_copy,
            rt->tx_len);
        if (fifo_written != rt->tx_len) {
            I2C_Abort();
            rt->busy = 0U;
            rt->op = I2C_ASYNC_NONE;
            rt->phase = I2C_PHASE_IDLE;
            rt->callback = 0;
            return BSP_ERROR;
        }

        DL_DMA_setSrcAddr(
            DMA,
            DMA_CH3_CHAN_ID,
            (uint32_t)&I2C_SENSOR_INST->MASTER.MRXDATA);
        DL_DMA_setDestAddr(
            DMA,
            DMA_CH3_CHAN_ID,
            (uint32_t)rt->rx_buf);
        DL_DMA_setTransferSize(
            DMA,
            DMA_CH3_CHAN_ID,
            rt->rx_len);

        rt->phase = I2C_PHASE_RX;
        rt->repeat_read_pending = 1U;
        rt->debug.status_repeat_before =
            (uint16_t)DL_I2C_getControllerStatus(
                I2C_SENSOR_INST);
        rt->debug.repeat_start_count++;
        DL_DMA_enableChannel(DMA, DMA_CH3_CHAN_ID);

        I2C_RecordDebugEvent(
            rt,
            I2C_DIAG_EVENT_HW_REPEAT,
            (uint32_t)DL_I2C_IIDX_NO_INT);

        I2C_StartControllerTransfer(
            rt->dev_addr,
            DL_I2C_CONTROLLER_DIRECTION_RX,
            rt->rx_len,
            DL_I2C_CONTROLLER_START_ENABLE,
            DL_I2C_CONTROLLER_STOP_ENABLE,
            DL_I2C_CONTROLLER_ACK_DISABLE,
            1U);

        rt->debug.status_repeat_after =
            (uint16_t)DL_I2C_getControllerStatus(
                I2C_SENSOR_INST);
    } else if (op == I2C_ASYNC_READ) {
        DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);
        DL_I2C_flushControllerRXFIFO(I2C_SENSOR_INST);
        DL_DMA_setSrcAddr(
            DMA,
            DMA_CH3_CHAN_ID,
            (uint32_t)&I2C_SENSOR_INST->MASTER.MRXDATA);
        DL_DMA_setDestAddr(
            DMA,
            DMA_CH3_CHAN_ID,
            (uint32_t)rt->rx_buf);
        DL_DMA_setTransferSize(
            DMA,
            DMA_CH3_CHAN_ID,
            rt->rx_len);

        rt->phase = I2C_PHASE_RX;
        DL_DMA_enableChannel(DMA, DMA_CH3_CHAN_ID);

        I2C_RecordDebugEvent(
            rt,
            I2C_DIAG_EVENT_RX_START,
            (uint32_t)DL_I2C_IIDX_NO_INT);

        I2C_StartControllerTransfer(
            rt->dev_addr,
            DL_I2C_CONTROLLER_DIRECTION_RX,
            rt->rx_len,
            DL_I2C_CONTROLLER_START_ENABLE,
            DL_I2C_CONTROLLER_STOP_ENABLE,
            DL_I2C_CONTROLLER_ACK_DISABLE,
            0U);
    } else {
        DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);
        DL_I2C_flushControllerTXFIFO(I2C_SENSOR_INST);

        /*
         * MSPM0的TX DMA事件由TX FIFO阈值触发。
         * 灰度ping/固件/0xB0命令只有1字节，空FIFO启动后可能没有及时
         * 获得DMA数据，表现为TX_START后永远没有TX_DONE。
         *
         * 1字节事务直接预装FIFO；2字节及以上继续使用原DMA路径，
         * 因此OLED初始化和页面数据发送路径保持不变。
         */
        if (rt->tx_len == 1U) {
            fifo_written = DL_I2C_fillControllerTXFIFO(
                I2C_SENSOR_INST,
                rt->tx_copy,
                1U);

            if (fifo_written != 1U) {
                I2C_Abort();
                rt->busy = 0U;
                rt->op = I2C_ASYNC_NONE;
                rt->phase = I2C_PHASE_IDLE;
                rt->callback = 0;
                return BSP_ERROR;
            }
        } else {
            DL_DMA_setSrcAddr(
                DMA,
                DMA_CH2_CHAN_ID,
                (uint32_t)rt->tx_copy);
            DL_DMA_setDestAddr(
                DMA,
                DMA_CH2_CHAN_ID,
                (uint32_t)&I2C_SENSOR_INST->MASTER.MTXDATA);
            DL_DMA_setTransferSize(
                DMA,
                DMA_CH2_CHAN_ID,
                rt->tx_len);
            DL_DMA_enableChannel(DMA, DMA_CH2_CHAN_ID);
        }

        rt->phase = I2C_PHASE_TX;

        I2C_RecordDebugEvent(
            rt,
            I2C_DIAG_EVENT_TX_START,
            (uint32_t)DL_I2C_IIDX_NO_INT);

        I2C_StartControllerTransfer(
            rt->dev_addr,
            DL_I2C_CONTROLLER_DIRECTION_TX,
            rt->tx_len,
            DL_I2C_CONTROLLER_START_ENABLE,
            (op == I2C_ASYNC_WRITE) ?
                DL_I2C_CONTROLLER_STOP_ENABLE :
                DL_I2C_CONTROLLER_STOP_DISABLE,
            DL_I2C_CONTROLLER_ACK_DISABLE,
            0U);
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
    uint32_t status;

    if (bus >= I2C_BUS_COUNT) {
        return 0U;
    }

    if (s_i2c_rt[bus].busy != 0U) {
        return 1U;
    }

    status = DL_I2C_getControllerStatus(I2C_SENSOR_INST);

    /*
     * BUSBSY可能在START/STOP状态同步期间短暂变化；软件空闲时若
     * BUSBSY仍为1且任一线路为低，则按总线被占用处理，避免覆盖
     * 尚未收尾的事务或I2C_ERR_05造成的SDA卡低现场。
     */
    return (((status & DL_I2C_CONTROLLER_STATUS_BUSY) != 0U) ||
            ((status & DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) ||
            (((status & DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U) &&
             (I2C_GetLineState() != 0x03U))) ?
           1U : 0U;
}

BSP_Status_t BSP_I2C_GetDebug(I2C_Bus_t bus, BSP_I2C_Debug_t *debug)
{
    I2C_Runtime_t *rt;

    if ((bus >= I2C_BUS_COUNT) || (debug == 0)) {
        return BSP_PARAM;
    }

    rt = &s_i2c_rt[bus];
    *debug = rt->debug;
    debug->phase = (uint8_t)rt->phase;
    debug->repeat_read_pending = rt->repeat_read_pending;
    debug->current_status =
        (uint16_t)DL_I2C_getControllerStatus(I2C_SENSOR_INST);
    debug->line_state = I2C_GetLineState();
    debug->sr1 = debug->current_status;
    debug->sr2 = debug->line_state;
    debug->controller_config =
        I2C_SENSOR_INST->MASTER.MCR;
    debug->target_address =
        I2C_SENSOR_INST->MASTER.MSA;
    debug->controller_control =
        I2C_SENSOR_INST->MASTER.MCTR;
    debug->fifo_status =
        I2C_SENSOR_INST->MASTER.MFIFOSR;
    debug->raw_interrupts =
        I2C_SENSOR_INST->CPU_INT.RIS;
    debug->enabled_interrupts =
        I2C_SENSOR_INST->CPU_INT.IMASK;
    debug->dma_rx_remaining =
        (uint16_t)DL_DMA_getTransferSize(
            DMA,
            DMA_CH3_CHAN_ID);

    return BSP_OK;
}

void BSP_I2C_Task(I2C_Bus_t bus)
{
    I2C_Runtime_t *rt;
    I2C_Callback_t callback;
    BSP_Status_t status;
    uint32_t controller_status;
    uint32_t raw_events;
    const uint32_t event_mask =
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_RX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_NACK |
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST;

    if ((bus >= I2C_BUS_COUNT) ||
        (s_i2c_rt[bus].busy == 0U)) {
        return;
    }

    rt = &s_i2c_rt[bus];

    /*
     * 中断仍是正常完成路径；同时轮询RIS作为保险，避免NVIC入口被
     * 临时屏蔽或TX_DONE/RX_DONE未及时送达CPU时只能等100 ms超时。
     */
    raw_events = DL_I2C_getRawInterruptStatus(
        I2C_SENSOR_INST,
        event_mask);
    if (raw_events != 0U) {
        I2C_HandleInterrupt();
    }

    controller_status =
        DL_I2C_getControllerStatus(I2C_SENSOR_INST);

    /*
     * 对没有送达完成中断的事务，用硬件最终状态收尾。
     * STOP写完成应为IDLE且总线空闲；读完成还要求RX DMA计数归零。
     */
    if ((rt->completion_pending == 0U) &&
        ((controller_status &
          DL_I2C_CONTROLLER_STATUS_BUSY) == 0U) &&
        ((controller_status &
          DL_I2C_CONTROLLER_STATUS_IDLE) != 0U) &&
        ((controller_status &
          DL_I2C_CONTROLLER_STATUS_BUSY_BUS) == 0U)) {
        if ((rt->phase == I2C_PHASE_TX) &&
            (rt->op == I2C_ASYNC_WRITE)) {
            rt->debug.tx_done_count++;
            rt->debug.status_tx_done =
                (uint16_t)controller_status;
            I2C_RecordDebugEvent(
                rt,
                I2C_DIAG_EVENT_TX_DONE,
                (uint32_t)DL_I2C_IIDX_NO_INT);
            I2C_MarkCompletion(BSP_OK);
        } else if ((rt->phase == I2C_PHASE_RX) &&
                   (DL_DMA_getTransferSize(
                        DMA,
                        DMA_CH3_CHAN_ID) == 0U)) {
            DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);
            rt->debug.rx_done_count++;
            rt->debug.status_rx_done =
                (uint16_t)controller_status;
            I2C_RecordDebugEvent(
                rt,
                I2C_DIAG_EVENT_RX_DONE,
                (uint32_t)DL_I2C_IIDX_NO_INT);
            I2C_MarkCompletion(BSP_OK);
        }
    }

    /*
     * RX_DONE/TX_DONE只表示数据阶段结束，STOP可能仍在移出总线。
     * 必须等到控制器空闲且BUSBSY清零后再回调，否则下一笔事务会在
     * SDA尚未释放时启动，最终表现为IDLE与BUSBSY同时置位。
     */
    if ((rt->completion_pending != 0U) &&
        (rt->completion_status == BSP_OK) &&
        (((controller_status &
            DL_I2C_CONTROLLER_STATUS_BUSY) != 0U) ||
          ((controller_status &
            DL_I2C_CONTROLLER_STATUS_IDLE) == 0U) ||
          ((controller_status &
            DL_I2C_CONTROLLER_STATUS_BUSY_BUS) != 0U))) {
        if ((uint32_t)(BSP_GET_TICK() - rt->debug.start_tick) <
            I2C_DMA_TIMEOUT_MS) {
            return;
        }

        rt->debug.timeout_count++;
        I2C_RecordDebugEvent(
            rt,
            I2C_DIAG_EVENT_TIMEOUT,
            (uint32_t)DL_I2C_IIDX_NO_INT);

        I2C_Abort();
        rt->completion_status = BSP_TIMEOUT;
    }

    if (rt->completion_pending == 0U) {
        if ((uint32_t)(BSP_GET_TICK() - rt->debug.start_tick) <
            I2C_DMA_TIMEOUT_MS) {
            return;
        }

        rt->debug.timeout_count++;
        I2C_RecordDebugEvent(
            rt,
            I2C_DIAG_EVENT_TIMEOUT,
            (uint32_t)DL_I2C_IIDX_NO_INT);

        I2C_Abort();
        rt->completion_status = BSP_TIMEOUT;
        rt->completion_pending = 1U;
    }

    status = rt->completion_status;

    if ((status == BSP_OK) &&
        (rt->rx_len != 0U) &&
        (rt->rx_buf != 0)) {
        rt->debug.rx_first = rt->rx_buf[0];
    }

    rt->debug.state = 0U;
    rt->debug.tx_pos =
        (status == BSP_OK) ? rt->tx_len : 0U;

    if (status != BSP_OK) {
        rt->debug.error_count++;
        rt->debug.error_source =
            (status == BSP_TIMEOUT) ? 2U : 1U;
        rt->debug.error_state = (uint8_t)rt->op;
        rt->debug.error_dev_addr = rt->dev_addr;
        rt->debug.error_tx_pos = rt->debug.tx_pos;
        rt->debug.error_tx_len = rt->tx_len;
        rt->debug.error_rx_len = rt->rx_len;
        rt->debug.error_sr1 =
            (uint16_t)DL_I2C_getControllerStatus(I2C_SENSOR_INST);
        rt->debug.error_sr2 = I2C_GetLineState();
    }

    callback = rt->callback;

    rt->busy = 0U;
    rt->op = I2C_ASYNC_NONE;
    rt->phase = I2C_PHASE_IDLE;
    rt->repeat_read_pending = 0U;
    rt->completion_pending = 0U;
    rt->callback = 0;

    if (callback != 0) {
        callback(
            bus,
            (status == BSP_OK) ? 0 :
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

static void I2C_HandleInterrupt(void)
{
    I2C_Runtime_t *rt = &s_i2c_rt[I2C_BUS1];
    DL_I2C_IIDX iidx;

    do {
        iidx = DL_I2C_getPendingInterrupt(I2C_SENSOR_INST);

        if (iidx == DL_I2C_IIDX_CONTROLLER_NACK) {
            if (rt->busy != 0U) {
                rt->debug.nack_count++;
                I2C_RecordDebugEvent(
                    rt,
                    I2C_DIAG_EVENT_NACK,
                    (uint32_t)iidx);
                I2C_Abort();
                I2C_MarkCompletion(BSP_ERROR);
            }
        } else if (iidx ==
                   DL_I2C_IIDX_CONTROLLER_ARBITRATION_LOST) {
            if (rt->busy != 0U) {
                rt->debug.arb_lost_count++;
                I2C_RecordDebugEvent(
                    rt,
                    I2C_DIAG_EVENT_ARB_LOST,
                    (uint32_t)iidx);
                I2C_Abort();
                I2C_MarkCompletion(BSP_ERROR);
            }
        } else if (iidx ==
                   DL_I2C_IIDX_CONTROLLER_TX_DONE) {
            if ((rt->busy != 0U) &&
                (rt->phase == I2C_PHASE_TX)) {
                DL_DMA_disableChannel(DMA, DMA_CH2_CHAN_ID);

                rt->debug.tx_done_count++;
                rt->debug.status_tx_done =
                    (uint16_t)DL_I2C_getControllerStatus(
                        I2C_SENSOR_INST);
                I2C_RecordDebugEvent(
                    rt,
                    I2C_DIAG_EVENT_TX_DONE,
                    (uint32_t)iidx);

                if (rt->op != I2C_ASYNC_WRITE_READ) {
                    I2C_MarkCompletion(BSP_OK);
                }
            }
        } else if (iidx ==
                   DL_I2C_IIDX_CONTROLLER_RX_DONE) {
            if ((rt->busy != 0U) &&
                (rt->phase == I2C_PHASE_RX)) {
                DL_DMA_disableChannel(DMA, DMA_CH3_CHAN_ID);

                rt->debug.rx_done_count++;
                rt->debug.status_rx_done =
                    (uint16_t)DL_I2C_getControllerStatus(
                        I2C_SENSOR_INST);
                I2C_RecordDebugEvent(
                    rt,
                    I2C_DIAG_EVENT_RX_DONE,
                    (uint32_t)iidx);

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
