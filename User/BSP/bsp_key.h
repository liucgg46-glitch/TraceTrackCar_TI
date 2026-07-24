#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "bsp_common.h"
#include "bsp_gpio.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * KEY1～KEY4为外接按键，KEY5为核心板板载USER按键。
 * 所有按键使用内部上拉，按下时GPIO接地，低电平有效。
 *
 * KEY1：PB0，核心板A08
 * KEY2：PB1，核心板A09
 * KEY3：PB13，核心板B10
 * KEY4：PA8，核心板B12
 * KEY5：PB31，核心板板载USER
 */
#define BSP_KEY_DEBOUNCE_MS 20U

#define BSP_KEY1_ENABLE 1
#define BSP_KEY2_ENABLE 1
#define BSP_KEY3_ENABLE 1
#define BSP_KEY4_ENABLE 1
#define BSP_KEY5_ENABLE 1

#define BSP_KEY1_GPIO         BSP_GPIO_KEY1
#define BSP_KEY1_ACTIVE_LEVEL 0U
#define BSP_KEY2_GPIO         BSP_GPIO_KEY2
#define BSP_KEY2_ACTIVE_LEVEL 0U
#define BSP_KEY3_GPIO         BSP_GPIO_KEY3
#define BSP_KEY3_ACTIVE_LEVEL 0U
#define BSP_KEY4_GPIO         BSP_GPIO_KEY4
#define BSP_KEY4_ACTIVE_LEVEL 0U
#define BSP_KEY5_GPIO         BSP_GPIO_USER_KEY
#define BSP_KEY5_ACTIVE_LEVEL 0U

typedef enum {
    BSP_KEY1 = 0,
    BSP_KEY2,
    BSP_KEY3,
    BSP_KEY4,
    BSP_KEY5,
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