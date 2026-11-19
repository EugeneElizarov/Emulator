/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    btn.h
  * @brief   KEY button: debounce + press / long / extra-long / release events.
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef BTN_H
#define BTN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define BTN_SAMPLE_MS     5u      /* polling period */
#define BTN_DEBOUNCE_N    4u      /* 4 identical samples = 20 ms of stable state */
#define BTN_LONG_MS       1000u
#define BTN_XLONG_MS      5000u
#define BTN_QUEUE_LEN     8u
#define BTN_ACTIVE_LEVEL  0u      /* button to GND: pressed = 0 (GPIO_PIN_RESET) */

typedef enum {
    BTN_EV_NONE = 0, BTN_EV_PRESS, BTN_EV_RELEASE, BTN_EV_LONG, BTN_EV_XLONG
} btn_event_t;

typedef struct {
    btn_event_t type;
    uint16_t    hold_ms;   /* RELEASE: actual hold time; LONG/XLONG: the threshold */
} btn_evt_t;

void     BTN_Init(void);
void     BTN_Task(void);              /* call from main loop, timing is internal */
bool     BTN_Pop(btn_evt_t *out);     /* false when the queue is empty */
uint8_t  BTN_Pending(void);
bool     BTN_IsPressed(void);
uint32_t BTN_HoldMs(void);
uint32_t BTN_Overflows(void);         /* events displaced by queue overflow */

#ifdef __cplusplus
}
#endif
#endif /* BTN_H */
