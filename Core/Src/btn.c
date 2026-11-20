/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    btn.c
  * @brief   Debounced button state machine. All messages are produced inside
  *          BTN_Task(), i.e. in the single application context, so no critical
  *          sections are needed around Message_Add().
  *
  *          Message mapping (param1 = button number, param2 = param3 = 0):
  *            debounced press   -> MESSAGE_KEY_DOWN, MESSAGE_KEY_PRESS
  *            hold >= 1000 ms   -> MESSAGE_KEY_LONG_PRESS       (once)
  *            hold >= 5000 ms   -> MESSAGE_KEY_VERY_LONG_PRESS  (once)
  *            debounced release -> MESSAGE_KEY_UP
  *          MESSAGE_KEY_CHANGE_STATE is intentionally not used.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "btn.h"
#include "main.h"
#include "messages.h"

static uint8_t  s_raw_prev, s_cnt, s_stable;
static uint8_t  s_long_sent, s_vlong_sent;
static uint32_t s_t_press, s_last_sample;

static uint8_t btn_raw(void)
{
    GPIO_PinState lvl = (BTN_ACTIVE_LEVEL != 0u) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    return (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == lvl) ? 1u : 0u;
}

/* param1 = button number; param2 and param3 are not used for key messages.
   Queue overflow is not handled at this level. */
static void btn_post(uint8_t id)
{
    (void)Message_Add(id, BTN_NUMBER, 0u, 0u);
}

void BTN_Init(void)
{
    s_raw_prev    = btn_raw();
    s_cnt         = 0u;
    s_stable      = 0u;   /* held at boot -> DOWN/PRESS are reported after 20 ms */
    s_long_sent   = 0u;
    s_vlong_sent  = 0u;
    s_t_press     = 0u;
    s_last_sample = HAL_GetTick();
}

void BTN_Task(void)
{
    uint32_t now = HAL_GetTick();
    uint8_t  raw;

    if ((uint32_t)(now - s_last_sample) < BTN_SAMPLE_MS) { return; }
    s_last_sample = now;

    raw = btn_raw();

    if (raw == s_raw_prev) { if (s_cnt < BTN_DEBOUNCE_N) { s_cnt++; } }
    else                   { s_raw_prev = raw; s_cnt = 1u; }

    if ((s_cnt >= BTN_DEBOUNCE_N) && (raw != s_stable))
    {
        s_stable = raw;
        if (raw != 0u)
        {
            s_t_press    = now;
            s_long_sent  = 0u;
            s_vlong_sent = 0u;
            btn_post(MESSAGE_KEY_DOWN);
            btn_post(MESSAGE_KEY_PRESS);
        }
        else
        {
            btn_post(MESSAGE_KEY_UP);
        }
    }

    if (s_stable != 0u)
    {
        uint32_t h = now - s_t_press;
        if ((s_long_sent == 0u) && (h >= BTN_LONG_MS))
        {
            s_long_sent = 1u;
            btn_post(MESSAGE_KEY_LONG_PRESS);
        }
        if ((s_vlong_sent == 0u) && (h >= BTN_VLONG_MS))
        {
            s_vlong_sent = 1u;
            btn_post(MESSAGE_KEY_VERY_LONG_PRESS);
        }
    }
}

bool     BTN_IsPressed(void) { return (s_stable != 0u); }
uint32_t BTN_HoldMs(void)    { return (s_stable != 0u) ? (HAL_GetTick() - s_t_press) : 0u; }
