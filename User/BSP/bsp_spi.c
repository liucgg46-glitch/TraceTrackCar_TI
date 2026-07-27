#include "bsp_spi.h"

typedef struct {
    SPI_Regs *inst;
    uint8_t dma_tx_channel;
    uint8_t dma_rx_channel;
    uint8_t dma_available;
    IRQn_Type irqn;
} SPI_Cfg_t;

typedef struct {
    volatile uint8_t busy;
    volatile uint8_t dma_active;
    volatile uint8_t dma_tx_done;
    volatile uint8_t dma_rx_done;
    volatile uint8_t dma_tx_empty;
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    uint16_t len;
    uint8_t dummy_tx;
    uint8_t dummy_rx;
    uint32_t start_tick;
    SPI_Callback_t callback;
    void *ctx;
} SPI_Async_t;

static const SPI_Cfg_t s_spi_cfg[SPI_BUS_COUNT] = {
    [SPI_BUS1] = {
        SPI_DISPLAY_INST,
        DMA_CH0_CHAN_ID,
        DMA_CH1_CHAN_ID,
        1U,
        SPI_DISPLAY_INST_INT_IRQN
    },
    [SPI_BUS2] = {
        SPI_ICM20948_INST,
        0U,
        0U,
        0U,
        SPI_ICM20948_INST_INT_IRQN
    }
};

static SPI_Async_t s_spi_async[SPI_BUS_COUNT];

#define SPI_DMA_INTERRUPTS \
    (DL_SPI_INTERRUPT_DMA_DONE_RX | \
     DL_SPI_INTERRUPT_DMA_DONE_TX | \
     DL_SPI_INTERRUPT_TX_EMPTY)

static void SPI_StopDma(SPI_Bus_t bus)
{
    const SPI_Cfg_t *cfg = &s_spi_cfg[bus];

    DL_SPI_disableInterrupt(cfg->inst, SPI_DMA_INTERRUPTS);
    DL_DMA_disableChannel(DMA, cfg->dma_tx_channel);
    DL_DMA_disableChannel(DMA, cfg->dma_rx_channel);
}

static void SPI_StartDma(SPI_Bus_t bus)
{
    const SPI_Cfg_t *cfg = &s_spi_cfg[bus];
    SPI_Async_t *rt = &s_spi_async[bus];
    uint8_t drain[4];
    uint8_t *tx_source;
    uint8_t *rx_destination;

    SPI_StopDma(bus);
    while (DL_SPI_drainRXFIFO8(cfg->inst, drain, sizeof(drain)) != 0U) {
    }

    rt->dummy_tx = 0xFFU;
    rt->dummy_rx = 0U;
    tx_source = (rt->tx_buf != 0) ? rt->tx_buf : &rt->dummy_tx;
    rx_destination = (rt->rx_buf != 0) ? rt->rx_buf : &rt->dummy_rx;

    DL_DMA_setSrcAddr(
        DMA, cfg->dma_tx_channel, (uint32_t)tx_source);
    DL_DMA_setDestAddr(
        DMA, cfg->dma_tx_channel, (uint32_t)&cfg->inst->TXDATA);
    DL_DMA_setTransferSize(DMA, cfg->dma_tx_channel, rt->len);
    DL_DMA_setSrcIncrement(DMA, cfg->dma_tx_channel,
        (rt->tx_buf != 0) ? DL_DMA_ADDR_INCREMENT :
                            DL_DMA_ADDR_UNCHANGED);

    DL_DMA_setSrcAddr(
        DMA, cfg->dma_rx_channel, (uint32_t)&cfg->inst->RXDATA);
    DL_DMA_setDestAddr(
        DMA, cfg->dma_rx_channel, (uint32_t)rx_destination);
    DL_DMA_setTransferSize(DMA, cfg->dma_rx_channel, rt->len);
    DL_DMA_setDestIncrement(DMA, cfg->dma_rx_channel,
        (rt->rx_buf != 0) ? DL_DMA_ADDR_INCREMENT :
                            DL_DMA_ADDR_UNCHANGED);

    rt->dma_tx_done = 0U;
    rt->dma_rx_done = 0U;
    rt->dma_tx_empty = 0U;
    rt->dma_active = 1U;
    rt->start_tick = BSP_GET_TICK();

    DL_SPI_clearInterruptStatus(cfg->inst, SPI_DMA_INTERRUPTS);
    DL_SPI_enableInterrupt(cfg->inst, SPI_DMA_INTERRUPTS);
    DL_DMA_enableChannel(DMA, cfg->dma_rx_channel);
    DL_DMA_enableChannel(DMA, cfg->dma_tx_channel);
}

static void SPI_CompleteAsync(SPI_Bus_t bus, BSP_Status_t status)
{
    SPI_Async_t *rt = &s_spi_async[bus];
    SPI_Callback_t callback = rt->callback;
    void *context = rt->ctx;

    if (rt->dma_active != 0U) {
        SPI_StopDma(bus);
    }
    rt->dma_active = 0U;
    rt->busy = 0U;
    rt->callback = 0;
    rt->ctx = 0;

    if (callback != 0) {
        callback(bus, context, status);
    }
}

static void SPI_DrainRxFifo(SPI_Regs *inst)
{
    uint8_t drain[4];

    if (inst == 0) {
        return;
    }

    while (DL_SPI_drainRXFIFO8(inst, drain, sizeof(drain)) != 0U) {
    }
}

static BSP_Status_t SPI_TransferBlocking(SPI_Bus_t bus,
                                        const uint8_t *tx_buf,
                                        uint8_t *rx_buf,
                                        uint16_t len,
                                        uint8_t dummy_tx)
{
    SPI_Regs *inst;
    uint16_t index;

    if ((bus >= SPI_BUS_COUNT) || (len == 0U)) {
        return BSP_PARAM;
    }

    inst = s_spi_cfg[bus].inst;

    /*
     * 阻塞事务开始前清空历史RX字节。若FIFO残留一个旧字节，
     * 后续每次发送和接收数量仍相等，但整帧会永久错位一字节。
     */
    SPI_DrainRxFifo(inst);

    for (index = 0U; index < len; index++) {
        uint32_t timeout = SPI_BLOCK_TIMEOUT;
        uint8_t tx = (tx_buf != 0) ? tx_buf[index] : dummy_tx;

        while (DL_SPI_isTXFIFOFull(inst) && (timeout > 0U)) {
            timeout--;
        }
        if (timeout == 0U) {
            return BSP_TIMEOUT;
        }
        DL_SPI_transmitData8(inst, tx);

        timeout = SPI_BLOCK_TIMEOUT;
        while (DL_SPI_isRXFIFOEmpty(inst) && (timeout > 0U)) {
            timeout--;
        }
        if (timeout == 0U) {
            return BSP_TIMEOUT;
        }
        tx = DL_SPI_receiveData8(inst);
        if (rx_buf != 0) {
            rx_buf[index] = tx;
        }
    }
    return BSP_SPI_WaitIdle(bus);
}

void BSP_SPI_Init(SPI_Bus_t bus)
{
    if (bus < SPI_BUS_COUNT) {
        s_spi_async[bus].busy = 0U;
        s_spi_async[bus].dma_active = 0U;
        s_spi_async[bus].tx_buf = 0;
        s_spi_async[bus].rx_buf = 0;
        s_spi_async[bus].len = 0U;
        s_spi_async[bus].callback = 0;
        s_spi_async[bus].ctx = 0;
        if (s_spi_cfg[bus].dma_available != 0U) {
            SPI_StopDma(bus);
            NVIC_ClearPendingIRQ(s_spi_cfg[bus].irqn);
            NVIC_EnableIRQ(s_spi_cfg[bus].irqn);
        }
    }
}

void BSP_SPI_InitAll(void)
{
    SPI_Bus_t bus;

    for (bus = (SPI_Bus_t)0; bus < SPI_BUS_COUNT;
         bus = (SPI_Bus_t)(bus + 1)) {
        BSP_SPI_Init(bus);
    }
}

uint8_t BSP_SPI_TransferByte(SPI_Bus_t bus, uint8_t tx, uint8_t *rx)
{
    uint8_t value = 0xFFU;

    if (SPI_TransferBlocking(bus, &tx, &value, 1U, 0xFFU) != BSP_OK) {
        return 0U;
    }
    if (rx != 0) {
        *rx = value;
    }
    return 1U;
}

BSP_Status_t BSP_SPI_Transfer(SPI_Bus_t bus,
                             const uint8_t *tx_buf,
                             uint8_t *rx_buf,
                             uint16_t len,
                             uint8_t dummy_tx)
{
    if ((bus >= SPI_BUS_COUNT) || (s_spi_async[bus].busy != 0U)) {
        return (bus >= SPI_BUS_COUNT) ? BSP_PARAM : BSP_BUSY;
    }
    return SPI_TransferBlocking(bus, tx_buf, rx_buf, len, dummy_tx);
}

uint8_t BSP_SPI_ReadWriteByte(SPI_Bus_t bus, uint8_t data)
{
    uint8_t rx = 0xFFU;

    return (BSP_SPI_TransferByte(bus, data, &rx) != 0U) ? rx : 0xFFU;
}

BSP_Status_t BSP_SPI_TransferAsync_DMA(SPI_Bus_t bus,
                                      uint8_t *tx_buf,
                                      uint8_t *rx_buf,
                                      uint16_t len,
                                      SPI_Callback_t cb,
                                      void *ctx)
{
    uint32_t key;

    if ((bus >= SPI_BUS_COUNT) || (len == 0U) ||
        ((tx_buf == 0) && (rx_buf == 0))) {
        return BSP_PARAM;
    }

    key = BSP_EnterCritical();
    if (s_spi_async[bus].busy != 0U) {
        BSP_ExitCritical(key);
        return BSP_BUSY;
    }
    s_spi_async[bus].busy = 1U;
    s_spi_async[bus].tx_buf = tx_buf;
    s_spi_async[bus].rx_buf = rx_buf;
    s_spi_async[bus].len = len;
    s_spi_async[bus].callback = cb;
    s_spi_async[bus].ctx = ctx;
    BSP_ExitCritical(key);

    if (s_spi_cfg[bus].dma_available != 0U) {
        SPI_StartDma(bus);
    }
    return BSP_OK;
}

uint8_t BSP_SPI_IsBusy(SPI_Bus_t bus)
{
    if (bus >= SPI_BUS_COUNT) {
        return 0U;
    }
    return (s_spi_async[bus].busy != 0U) ||
           DL_SPI_isBusy(s_spi_cfg[bus].inst) ? 1U : 0U;
}

BSP_Status_t BSP_SPI_WaitIdle(SPI_Bus_t bus)
{
    uint32_t timeout;

    if (bus >= SPI_BUS_COUNT) {
        return BSP_PARAM;
    }

    timeout = SPI_BLOCK_TIMEOUT;
    while (DL_SPI_isBusy(s_spi_cfg[bus].inst) && (timeout > 0U)) {
        timeout--;
    }
    return (timeout != 0U) ? BSP_OK : BSP_TIMEOUT;
}

void BSP_SPI_Task(SPI_Bus_t bus)
{
    SPI_Callback_t cb;
    void *ctx;
    BSP_Status_t status;

    if ((bus >= SPI_BUS_COUNT) || (s_spi_async[bus].busy == 0U)) {
        return;
    }

    if (s_spi_async[bus].dma_active != 0U) {
        if ((s_spi_async[bus].dma_rx_done != 0U) &&
            (s_spi_async[bus].dma_tx_empty != 0U)) {
            SPI_CompleteAsync(bus, BSP_OK);
        } else if ((uint32_t)(BSP_GET_TICK() -
                   s_spi_async[bus].start_tick) >= SPI_DMA_TIMEOUT_MS) {
            SPI_CompleteAsync(bus, BSP_TIMEOUT);
        }
        return;
    }

    status = SPI_TransferBlocking(bus, s_spi_async[bus].tx_buf,
                                  s_spi_async[bus].rx_buf,
                                  s_spi_async[bus].len, 0xFFU);
    cb = s_spi_async[bus].callback;
    ctx = s_spi_async[bus].ctx;
    s_spi_async[bus].busy = 0U;
    if (cb != 0) {
        cb(bus, ctx, status);
    }
}

void BSP_SPI_TaskAll(void)
{
    SPI_Bus_t bus;

    for (bus = (SPI_Bus_t)0; bus < SPI_BUS_COUNT;
         bus = (SPI_Bus_t)(bus + 1)) {
        BSP_SPI_Task(bus);
    }
}

void BSP_SPI_DMA_RX_ISR(SPI_Bus_t bus)
{
    if ((bus < SPI_BUS_COUNT) && (s_spi_async[bus].dma_active != 0U)) {
        s_spi_async[bus].dma_rx_done = 1U;
    }
}

void BSP_SPI_DMA_TX_ISR(SPI_Bus_t bus)
{
    if ((bus < SPI_BUS_COUNT) && (s_spi_async[bus].dma_active != 0U)) {
        s_spi_async[bus].dma_tx_done = 1U;
    }
}

void SPI_DISPLAY_INST_IRQHandler(void)
{
    DL_SPI_IIDX iidx = DL_SPI_getPendingInterrupt(SPI_DISPLAY_INST);

    if (iidx == DL_SPI_IIDX_DMA_DONE_RX) {
        BSP_SPI_DMA_RX_ISR(SPI_BUS1);
    } else if (iidx == DL_SPI_IIDX_DMA_DONE_TX) {
        BSP_SPI_DMA_TX_ISR(SPI_BUS1);
    } else if (iidx == DL_SPI_IIDX_TX_EMPTY) {
        if (s_spi_async[SPI_BUS1].dma_active != 0U) {
            s_spi_async[SPI_BUS1].dma_tx_empty = 1U;
        }
    }
}
