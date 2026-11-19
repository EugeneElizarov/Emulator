/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    btn.c
  * @brief   Debounced button state machine with an event queue.
  *          Non-blocking, timing based on HAL_GetTick().
  ******************************************************************************
  */
/* USER CODE END Header */

#include "btn.h"
#include "main.h"

static uint8_t  s_raw_prev, s_cnt, s_stable;
static uint8_t  s_long_sent, s_xlong_sent;
static uint32_t s_t_press, s_last_sample;

static btn_evt_t s_q[BTN_QUEUE_LEN];
static uint8_t   s_head, s_tail, s_used;
static uint32_t  s_ovf;

static uint8_t btn_raw(void)
{
    GPIO_PinState lvl = (BTN_ACTIVE_LEVEL != 0u) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    return (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == lvl) ? 1u : 0u;
}

static uint16_t btn_hold(uint32_t now)
{
    uint32_t h = now - s_t_press;
    return (uint16_t)((h > 0xFFFFu) ? 0xFFFFu : h);
}

/* On overflow the oldest event is displaced: recent events matter more for UI. */
static void btn_push(btn_event_t type, uint16_t hold)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    if (s_used >= BTN_QUEUE_LEN)
    {
        s_head = (uint8_t)((s_head + 1u) % BTN_QUEUE_LEN);
        s_used--;
        s_ovf++;
    }
    s_q[s_tail].type    = type;
    s_q[s_tail].hold_ms = hold;
    s_tail = (uint8_t)((s_tail + 1u) % BTN_QUEUE_LEN);
    s_used++;
    __set_PRIMASK(primask);
}

bool BTN_Pop(btn_evt_t *out)
{
    uint32_t primask;
    bool ok;

    if (out == NULL) { return false; }

    primask = __get_PRIMASK();
    __disable_irq();
    ok = (s_used != 0u);
    if (ok)
    {
        *out = s_q[s_head];
        s_head = (uint8_t)((s_head + 1u) % BTN_QUEUE_LEN);
        s_used--;
    }
    __set_PRIMASK(primask);
    return ok;
}

void BTN_Init(void)
{
    s_raw_prev    = btn_raw();
    s_cnt         = 0u;
    s_stable      = 0u;   /* held at boot -> PRESS is reported after 20 ms */
    s_long_sent   = 0u;
    s_xlong_sent  = 0u;
    s_t_press     = 0u;
    s_last_sample = HAL_GetTick();
    s_head = 0u; s_tail = 0u; s_used = 0u; s_ovf = 0u;
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
            s_t_press = now;
            s_long_sent = 0u;
            s_xlong_sent = 0u;
            btn_push(BTN_EV_PRESS, 0u);
        }
        else
        {
            btn_push(BTN_EV_RELEASE, btn_hold(now));
        }
    }

    if (s_stable != 0u)
    {
        uint32_t h = now - s_t_press;
        if ((s_long_sent == 0u) && (h >= BTN_LONG_MS))
        {
            s_long_sent = 1u;
            btn_push(BTN_EV_LONG, (uint16_t)BTN_LONG_MS);
        }
        if ((s_xlong_sent == 0u) && (h >= BTN_XLONG_MS))
        {
            s_xlong_sent = 1u;
            btn_push(BTN_EV_XLONG, (uint16_t)BTN_XLONG_MS);
        }
    }
}

uint8_t  BTN_Pending(void)   { return s_used; }
bool     BTN_IsPressed(void) { return (s_stable != 0u); }
uint32_t BTN_Overflows(void) { return s_ovf; }
uint32_t BTN_HoldMs(void)    { return (s_stable != 0u) ? (HAL_GetTick() - s_t_press) : 0u; }
