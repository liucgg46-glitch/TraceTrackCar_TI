#include "bsp_adc.h"

static uint8_t s_adc_running;
static uint16_t s_adc_last;

void BSP_ADC_Init(void)
{
    s_adc_running = 1U;
    s_adc_last = 0U;
    DL_ADC12_enableConversions(ADC_GRAY_INST);
}

uint16_t BSP_ADC_GetRaw(BSP_ADC_Ch_t ch)
{
    uint32_t timeout;

    if ((ch >= BSP_ADC_CH_COUNT) || (s_adc_running == 0U)) {
        return s_adc_last;
    }

    DL_ADC12_clearInterruptStatus(
        ADC_GRAY_INST, DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED);
    DL_ADC12_startConversion(ADC_GRAY_INST);

    timeout = BSP_ADC_CONVERSION_TIMEOUT;
    while ((DL_ADC12_getRawInterruptStatus(ADC_GRAY_INST,
               DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED) == 0U) &&
           (timeout > 0U)) {
        timeout--;
    }

    if (timeout != 0U) {
        s_adc_last = (uint16_t)DL_ADC12_getMemResult(
            ADC_GRAY_INST, ADC_GRAY_ADCMEM_0);
    }
    return s_adc_last;
}

BSP_Status_t BSP_ADC_GetRawArray(
    uint16_t *out_buf, uint8_t max_count, uint8_t *out_count)
{
    if ((out_buf == 0) || (out_count == 0) || (max_count == 0U)) {
        return BSP_PARAM;
    }

    out_buf[0] = BSP_ADC_GetRaw(BSP_ADC_CH1);
    *out_count = 1U;
    return BSP_OK;
}

void BSP_ADC_Start(void)
{
    DL_ADC12_enableConversions(ADC_GRAY_INST);
    s_adc_running = 1U;
}

void BSP_ADC_Stop(void)
{
    s_adc_running = 0U;
    DL_ADC12_disableConversions(ADC_GRAY_INST);
}
