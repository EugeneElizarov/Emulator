/*
 * messages.h
 *
 *  Created on: 22 мар. 2026 г.
 *      Author: eugen
 */

#ifndef VENDOR_SPI_DEMO_MESSAGES_H_
#define VENDOR_SPI_DEMO_MESSAGES_H_

#include <stdbool.h>
#include <stdint.h>
#include "messages_list.h"

#define MESSAGE_COUNT					50
#define PROCESSOR_COUNT					20
#define MESSAGE_PROCESSOR_BUFFER_SIZE	2048

#define COUNT_MSG_ARGS(...)             sizeof((uint8_t[]){__VA_ARGS__})
#define MESSAGES(...)					COUNT_MSG_ARGS(__VA_ARGS__), (uint8_t[]){__VA_ARGS__}

#define P8(p)							((uint8_t)((p) & 0xFF))
#define P16(p8_1, p8_2)					((((uint16_t)P8(p8_2)) << 8) + P8(p8_1))
#define P32(p8_1, p8_2, p8_3, p8_4)		((((uint32_t)P8(p8_4)) << 24) + 			\
		                                 (((uint32_t)P8(p8_3)) << 16) + 			\
		                                 (((uint32_t)P8(p8_2)) << 8)  + P8(p8_1))
#define P16232(p16_1, p16_2)			((((uint32_t)((p16_2)) & 0xFFFF) << 16) + ((p16_1) & 0xFFFF))

#if defined (__clang__)
#pragma anon_unions
#endif

#define FLOAT2UINT(fl)					(((union{float f; uint32_t u;}){.f = (fl)}).u)
#define UINT2FLOAT(ui)					(((union{float f; uint32_t u;}){.u = (ui)}).f)
#define INT2UINT(vi)					(((union{int32_t i; uint32_t u;}){.i = (vi)}).u)
#define UINT2INT(ui)					(((union{int32_t i; uint32_t u;}){.u = (ui)}).i)


typedef struct Message_t Message_t;

struct __attribute__((packed)) Message_t
{
  uint8_t msg;
  union __attribute__((packed))
  {
    uint8_t u;
    int8_t i;
    char c;
    unsigned char uc;
  }param1;
  union __attribute__((packed))
  {
    uint16_t u;
    int16_t i;
    uint8_t u8[2];
    int8_t i8[2];
    char c[2];
    unsigned char uc[2];
  }param2;
  union __attribute__((packed))
  {
    void *p;
    uint32_t u;
    int32_t i;
    uint16_t u16[2];
    int16_t i16[2];
    uint8_t u8[4];
    int8_t i8[4];
    char c[4];
    unsigned char uc[4];
  }param3;
};

typedef struct Message_t *Message;

typedef bool (* MessageProcessor)(Message message);

void Message_Init(void);
bool Message_Add(const uint8_t ID, uint8_t par1, uint16_t par2, uint32_t par3);
bool Message_AddPtr(const uint8_t ID, uint8_t par1, uint16_t par2, void *par3);
bool Message_AddProcessor(MessageProcessor processor, uint8_t message_count, const uint8_t *message_list);
void Message_Poll(void);

#endif /* VENDOR_SPI_DEMO_MESSAGES_H_ */
