/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    led.h
  * @brief   LED indication driven by a 32-bit blink mask (bit queue).
  *          LSB is displayed, then the mask is shifted right; step = 125 ms.
  *          When the mask reaches 1 or 0 it is reloaded from the stored value.
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LED_STEP_MS            125u  /* interval between bit-queue steps */

/* 0 (default): display LSB, then reload if mask is 1 or 0, else shift.
   Every bit of the pattern is displayed.
   1: shift first, then reload if mask is 1 or 0 (literal reading of the spec).
   NOTE: with 1 the most significant set bit is never displayed, e.g. a
   pattern of 0x80000000 would keep the LED permanently off. */
#define LED_STRICT_SHIFT_FIRST 0u

/* Bit N = 1 -> LED N is active low. Default: all LEDs active high. */
#define LED_ACTIVE_LOW_MASK    0x00u

typedef enum {
    LED_1G = 0, LED_1R, LED_2G, LED_2R, LED_3G, LED_3R,
    LED_COUNT
} led_id_t;

void     LED_Init(void);
void     LED_Task(void);                          /* call from main loop, timing is internal */
void     LED_SetPattern(led_id_t id, uint32_t mask);
uint32_t LED_GetPattern(led_id_t id);
void     LED_SetAll(uint32_t mask);
void     LED_On(led_id_t id);                     /* mask 0xFFFFFFFF */
void     LED_Off(led_id_t id);                    /* mask 0x00000000 */

#ifdef __cplusplus
}
#endif
#endif /* LED_H */
