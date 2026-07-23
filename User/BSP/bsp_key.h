#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "bsp_common.h"
#include "bsp_gpio.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 核心板只有一个 USER 按键。
 * 为保持 STM32 小车上层接口不变，将它固定映射为原工程的 KEY5。
 */
#define BSP_KEY_DEBOUNCE_MS 20U

#define BSP_KEY1_ENABLE 0
#define BSP_KEY2_ENABLE 0
#define BSP_KEY3_ENABLE 0
#define BSP_KEY4_ENABLE 0
#define BSP_KEY5_ENABLE 1

#define BSP_KEY5_GPIO         BSP_GPIO_USER_KEY
#define BSP_KEY5_ACTIVE_LEVEL 0U

typedef enum {
    BSP_KEY5 = 0,
    BSP_KEY_COUNT
} BSP_Key_Id_t;

typedef enum {
    BSP_KEY_EVENT_NONE = 0,
    BSP_KEY_EVENT_PRESSED,
    BSP_KEY_EVENT_RELEASED
} BSP_KeyEvent_t;

void BSP_Key_Init(BSP_Key_Id_t id);
void BSP_Key_InitAll(void);
void BSP_Key_Update(BSP_Key_Id_t id);
void BSP_Key_UpdateAll(void);
uint8_t BSP_Key_IsPressed(BSP_Key_Id_t id);
uint8_t BSP_Key_WasPressed(BSP_Key_Id_t id);
uint8_t BSP_Key_WasReleased(BSP_Key_Id_t id);
BSP_KeyEvent_t BSP_Key_GetEvent(BSP_Key_Id_t id);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_KEY_H */
