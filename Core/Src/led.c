/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    led.c
  * @brief   LED blink-mask engine. Non-blocking, timing based on HAL_GetTick().
  ******************************************************************************
  */
/* USER CODE END Header */

#include "led.h"
#include "main.h"

typedef struct { GPIO_TypeDef *port; uint16_t pin; } led_hw_t;

/* Pin binding comes from the CubeMX-generated main.h, not hardcoded:
   regenerating the project keeps this table correct. */
static const led_hw_t s_hw[LED_COUNT] = {
    { LED1G_GPIO_Port, LED1G_Pin }, { LED1R_GPIO_Port, LED1R_Pin },
    { LED2G_GPIO_Port, LED2G_Pin }, { LED2R_GPIO_Port, LED2R_Pin },
    { LED3G_GPIO_Port, LED3G_Pin }, { LED3R_GPIO_Port, LED3R_Pin }
};

static uint32_t s_pattern[LED_COUNT];   /* value used for reload */
static uint32_t s_work[LED_COUNT];      /* current bit queue */
static uint32_t s_last;

static void led_apply(led_id_t id, uint8_t on)
{
    uint8_t level = (uint8_t)(on ^ ((LED_ACTIVE_LOW_MASK >> (uint32_t)id) & 1u));
    HAL_GPIO_WritePin(s_hw[id].port, s_hw[id].pin,
                      (level != 0u) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void led_step(void)
{
    uint32_t i;
    for (i = 0u; i < (uint32_t)LED_COUNT; i++)
    {
        uint32_t m = s_work[i];
        led_apply((led_id_t)i, (uint8_t)(m & 1u));
#if LED_STRICT_SHIFT_FIRST
        m >>= 1;
        if ((m == 0u) || (m == 1u)) { m = s_pattern[i]; }
#else
        if ((m == 0u) || (m == 1u)) { m = s_pattern[i]; }
        else                        { m >>= 1; }
#endif
        s_work[i] = m;
    }
}

void LED_Init(void)
{
    uint32_t i;
    for (i = 0u; i < (uint32_t)LED_COUNT; i++)
    {
        s_pattern[i] = 0u;
        s_work[i]    = 0u;
        led_apply((led_id_t)i, 0u);
    }
    s_last = HAL_GetTick();
}

void LED_Task(void)
{
    uint32_t now = HAL_GetTick();
    if ((uint32_t)(now - s_last) >= LED_STEP_MS)
    {
        s_last = now;   /* missed steps are not caught up: indication must not burst */
        led_step();
    }
}

void LED_SetPattern(led_id_t id, uint32_t mask)
{
    uint32_t primask;
    if (id >= LED_COUNT) { return; }

    primask = __get_PRIMASK();
    __disable_irq();
    s_pattern[id] = mask;
    s_work[id]    = mask;
    __set_PRIMASK(primask);

    led_apply(id, (uint8_t)(mask & 1u));   /* immediate reaction, no 125 ms wait */
}

uint32_t LED_GetPattern(led_id_t id)
{
    return (id < LED_COUNT) ? s_pattern[id] : 0u;
}

void LED_SetAll(uint32_t mask)
{
    uint32_t i;
    for (i = 0u; i < (uint32_t)LED_COUNT; i++) { LED_SetPattern((led_id_t)i, mask); }
}

void LED_On(led_id_t id)  { LED_SetPattern(id, 0xFFFFFFFFu); }
void LED_Off(led_id_t id) { LED_SetPattern(id, 0x00000000u); }
