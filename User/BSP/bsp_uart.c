#include "bsp_uart.h"

typedef struct {
    UART_Regs *inst;
    IRQn_Type irqn;
} UART_Cfg_t;

typedef struct {
    volatile uint16_t rx_head;
    volatile uint16_t rx_tail;
    volatile uint16_t tx_head;
    volatile uint16_t tx_tail;
    volatile uint16_t rx_overflow;
    volatile uint16_t tx_drop;
} UART_Runtime_t;

static const UART_Cfg_t s_uart_cfg[UART_PORT_COUNT] = {
    [UART_PORT1]      = {UART_E220_INST,  UART_E220_INST_INT_IRQN},
    [UART_PORT2]      = {UART_K210_INST,  UART_K210_INST_INT_IRQN},
    [UART_PORT_DEBUG] = {UART_DEBUG_INST, UART_DEBUG_INST_INT_IRQN}
};

static UART_Runtime_t s_uart_rt[UART_PORT_COUNT];
static uint8_t s_uart_rx_ring[UART_PORT_COUNT][UART_RX_BUF_SIZE];
static uint8_t s_uart_tx_ring[UART_PORT_COUNT][UART_TX_BUF_SIZE];
static BSP_UART_TxReadyFn_t s_tx_guard[UART_PORT_COUNT];
static BSP_UART_TxDeferredFn_t s_tx_deferred[UART_PORT_COUNT];

static uint16_t UART_Count(uint16_t head, uint16_t tail, uint16_t size)
{
    return (head >= tail) ? (head - tail) : (uint16_t)(size - tail + head);
}

static uint16_t UART_Next(uint16_t value, uint16_t size)
{
    value++;
    return (value >= size) ? 0U : value;
}

static void UART_KickTx(UART_Port_t port)
{
    UART_Runtime_t *rt = &s_uart_rt[port];
    UART_Regs *inst = s_uart_cfg[port].inst;

    while ((rt->tx_tail != rt->tx_head) &&
           !DL_UART_Main_isTXFIFOFull(inst)) {
        DL_UART_Main_transmitData(inst, s_uart_tx_ring[port][rt->tx_tail]);
        rt->tx_tail = UART_Next(rt->tx_tail, UART_TX_BUF_SIZE);
    }

    if (rt->tx_tail == rt->tx_head) {
        DL_UART_Main_disableInterrupt(inst, DL_UART_MAIN_INTERRUPT_TX);
    } else {
        DL_UART_Main_enableInterrupt(inst, DL_UART_MAIN_INTERRUPT_TX);
    }
}

void BSP_UART_Init(UART_Port_t port)
{
    UART_Runtime_t *rt;

    if (port >= UART_PORT_COUNT) {
        return;
    }

    rt = &s_uart_rt[port];
    rt->rx_head = 0U;
    rt->rx_tail = 0U;
    rt->tx_head = 0U;
    rt->tx_tail = 0U;
    rt->rx_overflow = 0U;
    rt->tx_drop = 0U;
    s_tx_guard[port] = 0;
    s_tx_deferred[port] = 0;

    DL_UART_Main_disableInterrupt(
        s_uart_cfg[port].inst, DL_UART_MAIN_INTERRUPT_TX);
    DL_UART_Main_enableInterrupt(
        s_uart_cfg[port].inst, DL_UART_MAIN_INTERRUPT_RX);
    NVIC_ClearPendingIRQ(s_uart_cfg[port].irqn);
    NVIC_EnableIRQ(s_uart_cfg[port].irqn);
}

void BSP_UART_InitAll(void)
{
    UART_Port_t port;

    for (port = (UART_Port_t)0; port < UART_PORT_COUNT;
         port = (UART_Port_t)(port + 1)) {
        BSP_UART_Init(port);
    }
}

void BSP_UART_SetTxReadyGuard(
    UART_Port_t port, BSP_UART_TxReadyFn_t guard)
{
    if (port < UART_PORT_COUNT) {
        s_tx_guard[port] = guard;
    }
}

void BSP_UART_SetTxDeferredHandler(
    UART_Port_t port, BSP_UART_TxDeferredFn_t handler)
{
    if (port < UART_PORT_COUNT) {
        s_tx_deferred[port] = handler;
    }
}

uint16_t BSP_UART_TxFree(UART_Port_t port)
{
    uint16_t count;
    uint32_t key;

    if (port >= UART_PORT_COUNT) {
        return 0U;
    }

    key = BSP_EnterCritical();
    count = UART_Count(s_uart_rt[port].tx_head, s_uart_rt[port].tx_tail,
                       UART_TX_BUF_SIZE);
    BSP_ExitCritical(key);
    return (uint16_t)(UART_TX_BUF_SIZE - 1U - count);
}

uint16_t BSP_UART_Write(
    UART_Port_t port, const uint8_t *data, uint16_t len)
{
    UART_Runtime_t *rt;
    uint16_t written = 0U;
    uint32_t key;

    if ((port >= UART_PORT_COUNT) || (data == 0) || (len == 0U)) {
        return 0U;
    }

    rt = &s_uart_rt[port];
    key = BSP_EnterCritical();
    while (written < len) {
        uint16_t next = UART_Next(rt->tx_head, UART_TX_BUF_SIZE);
        if (next == rt->tx_tail) {
            break;
        }
        s_uart_tx_ring[port][rt->tx_head] = data[written];
        rt->tx_head = next;
        written++;
    }
    rt->tx_drop = (uint16_t)(rt->tx_drop + (len - written));
    BSP_ExitCritical(key);

    if (written != 0U) {
        DL_UART_Main_enableInterrupt(
            s_uart_cfg[port].inst, DL_UART_MAIN_INTERRUPT_TX);
        UART_KickTx(port);
    }
    return written;
}

BSP_Status_t BSP_UART_WriteFrameNow(
    UART_Port_t port, const uint8_t *data, uint16_t len)
{
    if ((port >= UART_PORT_COUNT) || (data == 0) || (len == 0U) ||
        (len >= UART_TX_BUF_SIZE)) {
        return BSP_PARAM;
    }
    if (BSP_UART_TxFree(port) < len) {
        return BSP_BUSY;
    }
    return (BSP_UART_Write(port, data, len) == len) ? BSP_OK : BSP_BUSY;
}

BSP_Status_t BSP_UART_WriteFrame(
    UART_Port_t port, const uint8_t *data, uint16_t len)
{
    BSP_Status_t status;

    if ((port >= UART_PORT_COUNT) || (data == 0) || (len == 0U)) {
        return BSP_PARAM;
    }

    if ((s_tx_guard[port] != 0) && (s_tx_guard[port]() == 0U)) {
        return (s_tx_deferred[port] != 0) ?
                   s_tx_deferred[port](data, len) : BSP_BUSY;
    }

    status = BSP_UART_WriteFrameNow(port, data, len);
    if ((status == BSP_BUSY) && (s_tx_deferred[port] != 0)) {
        status = s_tx_deferred[port](data, len);
    }
    return status;
}

BSP_Status_t BSP_UART_SendData_NonBlocking(
    UART_Port_t port, const uint8_t *data, uint16_t len)
{
    return BSP_UART_WriteFrame(port, data, len);
}

uint16_t BSP_UART_Read(UART_Port_t port, uint8_t *buf, uint16_t len)
{
    UART_Runtime_t *rt;
    uint16_t read = 0U;
    uint32_t key;

    if ((port >= UART_PORT_COUNT) || (buf == 0) || (len == 0U)) {
        return 0U;
    }

    rt = &s_uart_rt[port];
    key = BSP_EnterCritical();
    while ((read < len) && (rt->rx_tail != rt->rx_head)) {
        buf[read] = s_uart_rx_ring[port][rt->rx_tail];
        rt->rx_tail = UART_Next(rt->rx_tail, UART_RX_BUF_SIZE);
        read++;
    }
    BSP_ExitCritical(key);
    return read;
}

uint8_t BSP_UART_GetChar(UART_Port_t port, uint8_t *ch)
{
    return (BSP_UART_Read(port, ch, 1U) == 1U) ? 1U : 0U;
}

uint16_t BSP_UART_Available(UART_Port_t port)
{
    uint16_t count;
    uint32_t key;

    if (port >= UART_PORT_COUNT) {
        return 0U;
    }
    key = BSP_EnterCritical();
    count = UART_Count(s_uart_rt[port].rx_head, s_uart_rt[port].rx_tail,
                       UART_RX_BUF_SIZE);
    BSP_ExitCritical(key);
    return count;
}

uint8_t BSP_UART_IsTxBusy(UART_Port_t port)
{
    if (port >= UART_PORT_COUNT) {
        return 0U;
    }
    return ((s_uart_rt[port].tx_head != s_uart_rt[port].tx_tail) ||
            DL_UART_Main_isBusy(s_uart_cfg[port].inst)) ? 1U : 0U;
}

void BSP_UART_FlushRx(UART_Port_t port)
{
    uint32_t key;

    if (port >= UART_PORT_COUNT) {
        return;
    }
    key = BSP_EnterCritical();
    s_uart_rt[port].rx_tail = s_uart_rt[port].rx_head;
    BSP_ExitCritical(key);
}

void BSP_UART_FlushTx(UART_Port_t port)
{
    uint32_t key;

    if (port >= UART_PORT_COUNT) {
        return;
    }
    key = BSP_EnterCritical();
    s_uart_rt[port].tx_tail = s_uart_rt[port].tx_head;
    DL_UART_Main_disableInterrupt(
        s_uart_cfg[port].inst, DL_UART_MAIN_INTERRUPT_TX);
    BSP_ExitCritical(key);
}

BSP_Status_t BSP_UART_GetStats(UART_Port_t port, UART_Stats_t *stats)
{
    uint32_t key;

    if ((port >= UART_PORT_COUNT) || (stats == 0)) {
        return BSP_PARAM;
    }
    key = BSP_EnterCritical();
    stats->rx_overflow = s_uart_rt[port].rx_overflow;
    stats->tx_drop = s_uart_rt[port].tx_drop;
    stats->rx_count = UART_Count(s_uart_rt[port].rx_head,
                                 s_uart_rt[port].rx_tail, UART_RX_BUF_SIZE);
    stats->tx_count = UART_Count(s_uart_rt[port].tx_head,
                                 s_uart_rt[port].tx_tail, UART_TX_BUF_SIZE);
    BSP_ExitCritical(key);
    return BSP_OK;
}

void BSP_UART_ClearStats(UART_Port_t port)
{
    uint32_t key;

    if (port >= UART_PORT_COUNT) {
        return;
    }
    key = BSP_EnterCritical();
    s_uart_rt[port].rx_overflow = 0U;
    s_uart_rt[port].tx_drop = 0U;
    BSP_ExitCritical(key);
}

void BSP_UART_Task(UART_Port_t port)
{
    if (port < UART_PORT_COUNT) {
        UART_KickTx(port);
    }
}

void BSP_UART_TaskAll(void)
{
    UART_Port_t port;

    for (port = (UART_Port_t)0; port < UART_PORT_COUNT;
         port = (UART_Port_t)(port + 1)) {
        BSP_UART_Task(port);
    }
}

void BSP_UART_USART_ISR(UART_Port_t port)
{
    UART_Runtime_t *rt;
    UART_Regs *inst;
    DL_UART_IIDX iidx;

    if (port >= UART_PORT_COUNT) {
        return;
    }

    rt = &s_uart_rt[port];
    inst = s_uart_cfg[port].inst;
    do {
        iidx = DL_UART_Main_getPendingInterrupt(inst);
        if (iidx == DL_UART_MAIN_IIDX_RX) {
            while (!DL_UART_Main_isRXFIFOEmpty(inst)) {
                uint16_t next = UART_Next(rt->rx_head, UART_RX_BUF_SIZE);
                uint8_t value = DL_UART_Main_receiveData(inst);
                if (next == rt->rx_tail) {
                    rt->rx_overflow++;
                } else {
                    s_uart_rx_ring[port][rt->rx_head] = value;
                    rt->rx_head = next;
                }
            }
        } else if (iidx == DL_UART_MAIN_IIDX_TX) {
            UART_KickTx(port);
        }
    } while (iidx != DL_UART_MAIN_IIDX_NO_INTERRUPT);
}

void BSP_UART_DMA_TX_ISR(UART_Port_t port)
{
    (void)port;
}

void UART_DEBUG_INST_IRQHandler(void)
{
    BSP_UART_USART_ISR(UART_PORT_DEBUG);
}

void UART_E220_INST_IRQHandler(void)
{
    BSP_UART_USART_ISR(UART_PORT1);
}

void UART_K210_INST_IRQHandler(void)
{
    BSP_UART_USART_ISR(UART_PORT2);
}
