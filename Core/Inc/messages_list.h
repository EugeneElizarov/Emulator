/*
 * messages_list.h
 *
 *  Created on: 26 мар. 2026 г.
 *      Author: eugen
 */

#ifndef VENDOR_SPI_DEMO_MESSAGES_LIST_H_
#define VENDOR_SPI_DEMO_MESSAGES_LIST_H_

//#include "def.h"

#define MESSAGE_START_NUMERED_BLOCK(block_name, start_no)						\
	MESSAGE_##block_name##_START_ID = start_no,									\
	MESSAGE_##block_name##_PRESTART_ID = (MESSAGE_##block_name##_START_ID - 1)

#define MESSAGE_START_BLOCK(block_name)											\
	MESSAGE_##block_name##_START_ID,											\
	MESSAGE_##block_name##_PRESTART_ID = (MESSAGE_##block_name##_START_ID - 1)

#define MESSAGE_END_BLOCK(block_name)											\
	MESSAGE_##block_name##_END_ID,												\
	MESSAGE_##block_name##_LAST_ID = (MESSAGE_##block_name##_END_ID - 1)

#define MESSAGE_BLOCK(block_name, ...)											\
MESSAGE_START_BLOCK(block_name),												\
__VA_ARGS__,																	\
MESSAGE_END_BLOCK(block_name)

#define MESSAGE_NUMERED_BLOCK(block_name, start_no, ...)						\
MESSAGE_START_NUMERED_BLOCK(block_name, start_no),								\
__VA_ARGS__,																	\
MESSAGE_END_BLOCK(block_name)

typedef enum
{
	MSG_NO = 0,

	MESSAGE_BLOCK(KEY,
  MESSAGE_KEY_DOWN,
  MESSAGE_KEY_PRESS,
  MESSAGE_KEY_LONG_PRESS,
  MESSAGE_KEY_VERY_LONG_PRESS,
  MESSAGE_KEY_UP,
	MESSAGE_KEY_CHANGE_STATE),

	MSG_USER = 0x80
}MessageID;

#define MESSAGE_BLOCK_START_ID(block_name)		MESSAGE_##block_name##_START_ID
#define MESSAGE_BLOCK_END_ID(block_name)		MESSAGE_##block_name##_LAST_ID
#define MESSAGE_BLOCK_SIZE(block_name)			(MESSAGE_BLOCK_END_ID(block_name) - MESSAGE_BLOCK_START_ID(block_name))
#define MESSAGE_BLOCK_ASSERT(block_name, MSG)	(((MSG) >= MESSAGE_BLOCK_START_ID(block_name)) && ((MSG) <= MESSAGE_BLOCK_END_ID(block_name)))

#endif /* VENDOR_SPI_DEMO_MESSAGES_LIST_H_ */
