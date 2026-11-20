/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    btn.h
  * @brief   KEY button: debounce and message generation inside the poll function.
  *          Messages are added to the pipeline from the application context only.
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

#define BTN_NUMBER        0u     /* button index passed in Message param1 */
#define BTN_SAMPLE_MS     5u     /* polling period */
#define BTN_DEBOUNCE_N    4u     /* 4 identical samples = 20 ms of stable state */
#define BTN_LONG_MS       1000u
#define BTN_VLONG_MS      5000u
#define BTN_ACTIVE_LEVEL  0u     /* button to GND: pressed = 0 (GPIO_PIN_RESET) */

void     BTN_Init(void);
void     BTN_Task(void);         /* poll: debounce + Message_Add, call from main loop */
bool     BTN_IsPressed(void);
uint32_t BTN_HoldMs(void);

#ifdef __cplusplus
}
#endif
#endif /* BTN_H */
