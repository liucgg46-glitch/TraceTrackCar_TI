#include "bsp_key.h"

typedef struct {
    BSP_GPIO_Id_t gpio_id;
    uint8_t active_level;
} BSP_Key_Cfg_t;

typedef struct {
    uint8_t stable_pressed;
    uint8_t last_raw_pressed;
    uint8_t pressed_event;
    uint8_t released_event;
    uint32_t last_change_ms;
} BSP_Key_Runtime_t;

static const BSP_Key_Cfg_t s_key_cfg[BSP_KEY_COUNT] = {
    [BSP_KEY5] = {BSP_KEY5_GPIO, BSP_KEY5_ACTIVE_LEVEL}
};

static volatile BSP_Key_Runtime_t s_key_rt[BSP_KEY_COUNT];

static uint8_t Key_ReadRawPressed(BSP_Key_Id_t id)
{
    uint8_t level;

    if (id >= BSP_KEY_COUNT) {
        return 0U;
    }

    level = BSP_GPIO_Read(s_key_cfg[id].gpio_id);
    return (level == s_key_cfg[id].active_level) ? 1U : 0U;
}

void BSP_Key_Init(BSP_Key_Id_t id)
{
    uint8_t raw;

    if (id >= BSP_KEY_COUNT) {
        return;
    }

    raw = Key_ReadRawPressed(id);
    s_key_rt[id].stable_pressed  = raw;
    s_key_rt[id].last_raw_pressed = raw;
    s_key_rt[id].pressed_event   = 0U;
    s_key_rt[id].released_event  = 0U;
    s_key_rt[id].last_change_ms  = BSP_GET_TICK();
}

void BSP_Key_InitAll(void)
{
    BSP_Key_Id_t id;

    for (id = (BSP_Key_Id_t)0; id < BSP_KEY_COUNT;
         id = (BSP_Key_Id_t)(id + 1)) {
        BSP_Key_Init(id);
    }
}

void BSP_Key_Update(BSP_Key_Id_t id)
{
    uint8_t raw;
    uint32_t now;

    if (id >= BSP_KEY_COUNT) {
        return;
    }

    raw = Key_ReadRawPressed(id);
    now = BSP_GET_TICK();

    if (raw != s_key_rt[id].last_raw_pressed) {
        s_key_rt[id].last_raw_pressed = raw;
        s_key_rt[id].last_change_ms   = now;
        return;
    }

    if ((uint32_t)(now - s_key_rt[id].last_change_ms) <
        BSP_KEY_DEBOUNCE_MS) {
        return;
    }

    if (raw != s_key_rt[id].stable_pressed) {
        s_key_rt[id].stable_pressed = raw;
        if (raw != 0U) {
            s_key_rt[id].pressed_event = 1U;
        } else {
            s_key_rt[id].released_event = 1U;
        }
    }
}

void BSP_Key_UpdateAll(void)
{
    BSP_Key_Id_t id;

    for (id = (BSP_Key_Id_t)0; id < BSP_KEY_COUNT;
         id = (BSP_Key_Id_t)(id + 1)) {
        BSP_Key_Update(id);
    }
}

uint8_t BSP_Key_IsPressed(BSP_Key_Id_t id)
{
    return (id < BSP_KEY_COUNT) ? s_key_rt[id].stable_pressed : 0U;
}

uint8_t BSP_Key_WasPressed(BSP_Key_Id_t id)
{
    uint8_t result;
    uint32_t primask;

    if (id >= BSP_KEY_COUNT) {
        return 0U;
    }

    primask = BSP_EnterCritical();
    result = s_key_rt[id].pressed_event;
    s_key_rt[id].pressed_event = 0U;
    BSP_ExitCritical(primask);
    return result;
}

uint8_t BSP_Key_WasReleased(BSP_Key_Id_t id)
{
    uint8_t result;
    uint32_t primask;

    if (id >= BSP_KEY_COUNT) {
        return 0U;
    }

    primask = BSP_EnterCritical();
    result = s_key_rt[id].released_event;
    s_key_rt[id].released_event = 0U;
    BSP_ExitCritical(primask);
    return result;
}

BSP_KeyEvent_t BSP_Key_GetEvent(BSP_Key_Id_t id)
{
    if (BSP_Key_WasPressed(id) != 0U) {
        return BSP_KEY_EVENT_PRESSED;
    }
    if (BSP_Key_WasReleased(id) != 0U) {
        return BSP_KEY_EVENT_RELEASED;
    }
    return BSP_KEY_EVENT_NONE;
}
