/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbd_vendor.c
  * @brief   Minimal vendor-specific USB device on top of HAL PCD.
  *          Windows binds winusb.sys automatically via MS OS 2.0 (WCID)
  *          descriptors: no INF, no driver package, no test signing.
  *
  *          Scope of this revision: enumeration only (EP0 control pipe).
  *          Data endpoints and the application protocol are added later.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "usbd_vendor.h"
#include "usb.h"
#include "main.h"
#include <string.h>

#define EP0_MPS 64u

/* compile-time check, C99 / Arm Compiler 5 compatible */
#define CT_ASSERT(name, cond) typedef char ct_assert_##name[(cond) ? 1 : -1]

/* ---- MS OS 2.0 descriptor set lengths (computed by the preprocessor) ---- */
#define PROP_NAME   "DeviceInterfaceGUIDs"
#define NAME_BYTES  (sizeof(PROP_NAME) * 2u)             /* 21 * 2 = 42 */
#define DATA_BYTES  ((sizeof(USB_DEV_GUID) + 1u) * 2u)   /* (39+1)*2 = 80, REG_MULTI_SZ */
#define REG_LEN     (8u + NAME_BYTES + 2u + DATA_BYTES)  /* 132 */
#define FN_LEN      (8u + 20u + REG_LEN)                 /* 160 */
#define CFG_LEN     (8u + FN_LEN)                        /* 168 */
#define MSOS_LEN    (10u + CFG_LEN)                      /* 178 */
#define BOS_LEN     33u

static const uint8_t dev_desc[18] = {
    0x12u, 0x01u, 0x10u, 0x02u,        /* bcdUSB = 2.10 -> host requests BOS */
    0xFFu, 0x00u, 0x00u, EP0_MPS,      /* vendor-specific class, EP0 = 64 B */
    (uint8_t)(USB_VID & 0xFFu), (uint8_t)(USB_VID >> 8),
    (uint8_t)(USB_PID & 0xFFu), (uint8_t)(USB_PID >> 8),
    (uint8_t)(USB_BCD_DEVICE & 0xFFu), (uint8_t)(USB_BCD_DEVICE >> 8),
    0x01u, 0x02u, 0x03u, 0x01u         /* iManufacturer, iProduct, iSerial, 1 cfg */
};

static const uint8_t cfg_desc[18] = {
    0x09u, 0x02u, 0x12u, 0x00u, 0x01u, 0x01u, 0x00u, 0x80u, 0xFAu,
    0x09u, 0x04u, 0x00u, 0x00u, 0x00u, 0xFFu, 0x00u, 0x00u, 0x00u
};

static const uint8_t bos_desc[BOS_LEN] = {
    0x05u, 0x0Fu, (uint8_t)(BOS_LEN & 0xFFu), (uint8_t)(BOS_LEN >> 8), 0x01u,
    0x1Cu, 0x10u, 0x05u, 0x00u,        /* PLATFORM capability, 28 bytes */
    0xDFu,0x60u,0xDDu,0xD8u,0x89u,0x45u,0xC7u,0x4Cu,
    0x9Cu,0xD2u,0x65u,0x9Du,0x9Eu,0x64u,0x8Au,0x9Fu,  /* MS OS 2.0 UUID */
    0x00u,0x00u,0x03u,0x06u,           /* dwWindowsVersion = 0x06030000 (Win 8.1+) */
    (uint8_t)(MSOS_LEN & 0xFFu), (uint8_t)(MSOS_LEN >> 8),
    USB_MS_VENDOR_REQ, 0x00u
};

/* Everything except the trailing GUID string, which is filled in RAM at init */
static const uint8_t msos_head[] = {
    0x0Au,0x00u, 0x00u,0x00u, 0x00u,0x00u,0x03u,0x06u,
    (uint8_t)(MSOS_LEN & 0xFFu), (uint8_t)(MSOS_LEN >> 8),
    0x08u,0x00u, 0x01u,0x00u, 0x00u, 0x00u,
    (uint8_t)(CFG_LEN & 0xFFu), (uint8_t)(CFG_LEN >> 8),
    0x08u,0x00u, 0x02u,0x00u, 0x00u, 0x00u,
    (uint8_t)(FN_LEN & 0xFFu), (uint8_t)(FN_LEN >> 8),
    0x14u,0x00u, 0x03u,0x00u,
    'W','I','N','U','S','B',0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    (uint8_t)(REG_LEN & 0xFFu), (uint8_t)(REG_LEN >> 8), 0x04u,0x00u,
    0x07u,0x00u,                                             /* REG_MULTI_SZ */
    (uint8_t)(NAME_BYTES & 0xFFu), (uint8_t)(NAME_BYTES >> 8),
    'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,
    'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,
    'G',0,'U',0,'I',0,'D',0,'s',0, 0,0,
    (uint8_t)(DATA_BYTES & 0xFFu), (uint8_t)(DATA_BYTES >> 8)
};
CT_ASSERT(msos_head_size, sizeof(msos_head) == (MSOS_LEN - DATA_BYTES));
CT_ASSERT(dev_desc_size,  sizeof(dev_desc) == 18u);
CT_ASSERT(cfg_desc_size,  sizeof(cfg_desc) == 18u);
CT_ASSERT(bos_desc_size,  sizeof(bos_desc) == BOS_LEN);

static uint8_t msos_desc[MSOS_LEN];
static uint8_t str_lang[4] = { 0x04u, 0x03u, 0x09u, 0x04u };  /* en-US */
static uint8_t str_manuf[2u + 2u * 64u];
static uint8_t str_prod[2u + 2u * 64u];
static uint8_t str_serial[2u + 2u * 24u];

static volatile uint8_t s_configured;
static volatile uint8_t s_addr_pending;
static uint8_t s_addr;
static uint8_t s_byte;
static uint8_t s_status[2];

static void str_make(uint8_t *dst, const char *src)
{
    uint8_t n = 0u;
    while ((src[n] != '\0') && (n < 63u))
    {
        dst[2u + (2u * n)] = (uint8_t)src[n];
        dst[3u + (2u * n)] = 0u;
        n++;
    }
    dst[0] = (uint8_t)(2u + (2u * n));
    dst[1] = 0x03u;
}

static void desc_build(void)
{
    static const char hex[] = "0123456789ABCDEF";
    char ser[25];
    uint32_t i, j, u[3];

    str_make(str_manuf, USB_MANUFACTURER);
    str_make(str_prod,  USB_PRODUCT);

    /* 96-bit unique device ID -> 24 hex characters (RM0316 section 34.1) */
    u[0] = *(volatile uint32_t *)(UID_BASE);
    u[1] = *(volatile uint32_t *)(UID_BASE + 4u);
    u[2] = *(volatile uint32_t *)(UID_BASE + 8u);
    for (i = 0u; i < 3u; i++)
    {
        for (j = 0u; j < 8u; j++)
        {
            ser[(i * 8u) + j] = hex[(u[i] >> (28u - (4u * j))) & 0xFu];
        }
    }
    ser[24] = '\0';
    str_make(str_serial, ser);

    (void)memcpy(msos_desc, msos_head, sizeof(msos_head));
    for (i = 0u; i < (sizeof(USB_DEV_GUID) - 1u); i++)
    {
        msos_desc[sizeof(msos_head) + (2u * i)]      = (uint8_t)USB_DEV_GUID[i];
        msos_desc[sizeof(msos_head) + (2u * i) + 1u] = 0u;
    }
    msos_desc[MSOS_LEN - 4u] = 0u;   /* string terminator */
    msos_desc[MSOS_LEN - 3u] = 0u;
    msos_desc[MSOS_LEN - 2u] = 0u;   /* MULTI_SZ list terminator */
    msos_desc[MSOS_LEN - 1u] = 0u;
}

/* STM32F303 has no software-switchable D+ pull-up (no USB_BCDR/DPPU), so the
   host does not see a detach when the MCU resets. Drive PA12 low briefly to
   force SE0, then hand the pin back to the USB alternate function. Only MODER
   is touched, so this does not depend on how HAL_PCD_MspInit configured the
   remaining GPIO attributes. Single blocking wait, at initialisation only. */
static void usb_force_detach(void)
{
    uint32_t moder = GPIOA->MODER;

    GPIOA->BSRR  = (1u << 28);                         /* BR12: ODR12 = 0 */
    GPIOA->MODER = (moder & ~(3u << 24)) | (1u << 24); /* PA12 -> output, D+ low */
    HAL_Delay(USB_DETACH_MS);
    GPIOA->MODER = moder;                              /* back to AF (USB_DP) */
}

static void ep0_send(PCD_HandleTypeDef *h, const uint8_t *p, uint16_t len, uint16_t want)
{
    if (len > want) { len = want; }
    (void)HAL_PCD_EP_Transmit(h, 0x00u, (uint8_t *)p, (uint32_t)len);
}

static void ep0_status(PCD_HandleTypeDef *h)
{
    (void)HAL_PCD_EP_Transmit(h, 0x00u, NULL, 0u);
}

static void ep0_stall(PCD_HandleTypeDef *h)
{
    (void)HAL_PCD_EP_SetStall(h, 0x80u);
    (void)HAL_PCD_EP_SetStall(h, 0x00u);
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *h)
{
    const uint8_t *s = (const uint8_t *)h->Setup;
    uint8_t  rt = s[0];
    uint8_t  rq = s[1];
    uint16_t wv = (uint16_t)((uint16_t)s[2] | ((uint16_t)s[3] << 8));
    uint16_t wi = (uint16_t)((uint16_t)s[4] | ((uint16_t)s[5] << 8));
    uint16_t wl = (uint16_t)((uint16_t)s[6] | ((uint16_t)s[7] << 8));

    /* MS OS 2.0 descriptor set: vendor request, wIndex = 0x0007 */
    if ((rt == 0xC0u) && (rq == USB_MS_VENDOR_REQ) && (wi == 0x0007u))
    {
        ep0_send(h, msos_desc, (uint16_t)sizeof(msos_desc), wl);
        return;
    }

    if ((rt != 0x80u) && (rt != 0x00u) && (rt != 0x81u) &&
        (rt != 0x82u) && (rt != 0x01u) && (rt != 0x02u))
    {
        ep0_stall(h);
        return;
    }

    switch (rq)
    {
    case 0x06u: /* GET_DESCRIPTOR */
        switch (wv >> 8)
        {
        case 0x01u: ep0_send(h, dev_desc, (uint16_t)sizeof(dev_desc), wl); return;
        case 0x02u: ep0_send(h, cfg_desc, (uint16_t)sizeof(cfg_desc), wl); return;
        case 0x0Fu: ep0_send(h, bos_desc, (uint16_t)sizeof(bos_desc), wl); return;
        case 0x03u: /* STRING */
            switch (wv & 0xFFu)
            {
            case 0u: ep0_send(h, str_lang,   str_lang[0],   wl); return;
            case 1u: ep0_send(h, str_manuf,  str_manuf[0],  wl); return;
            case 2u: ep0_send(h, str_prod,   str_prod[0],   wl); return;
            case 3u: ep0_send(h, str_serial, str_serial[0], wl); return;
            default: break;
            }
            break;
        default: break;   /* DEVICE_QUALIFIER / OTHER_SPEED -> STALL (FS only) */
        }
        ep0_stall(h);
        return;

    case 0x05u: /* SET_ADDRESS: apply after the status stage completes */
        s_addr = (uint8_t)(wv & 0x7Fu);
        s_addr_pending = 1u;
        ep0_status(h);
        return;

    case 0x09u: /* SET_CONFIGURATION */
        s_configured = (wv == 1u) ? 1u : 0u;
        ep0_status(h);
        return;

    case 0x08u: /* GET_CONFIGURATION */
        s_byte = s_configured;
        ep0_send(h, &s_byte, 1u, wl);
        return;

    case 0x00u: /* GET_STATUS: bus powered, no remote wakeup, no halt */
        s_status[0] = 0u;
        s_status[1] = 0u;
        ep0_send(h, s_status, 2u, wl);
        return;

    case 0x0Au: /* GET_INTERFACE */
        s_byte = 0u;
        ep0_send(h, &s_byte, 1u, wl);
        return;

    case 0x01u: /* CLEAR_FEATURE */
    case 0x03u: /* SET_FEATURE   */
    case 0x0Bu: /* SET_INTERFACE */
        ep0_status(h);
        return;

    default:
        ep0_stall(h);
        return;
    }
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *h, uint8_t epnum)
{
    if ((epnum == 0u) && (s_addr_pending != 0u))
    {
        s_addr_pending = 0u;
        (void)HAL_PCD_SetAddress(h, s_addr);
    }
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *h, uint8_t epnum)
{
    if (epnum == 0u)
    {
        ep0_status(h);   /* status stage of a control write */
    }
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *h)
{
    s_configured   = 0u;
    s_addr_pending = 0u;
    (void)HAL_PCD_EP_Open(h, 0x00u, EP0_MPS, EP_TYPE_CTRL);
    (void)HAL_PCD_EP_Open(h, 0x80u, EP0_MPS, EP_TYPE_CTRL);
    (void)HAL_PCD_EP_Receive(h, 0x00u, NULL, 0u);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *h) { (void)h; }
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *h)  { (void)h; }

void USB_HP_CAN_TX_IRQHandler(void)  { HAL_PCD_IRQHandler(&hpcd_USB_FS); }
void USB_LP_CAN_RX0_IRQHandler(void) { HAL_PCD_IRQHandler(&hpcd_USB_FS); }

uint8_t USBDV_IsConfigured(void)
{
    return s_configured;
}

void USBDV_Init(void)
{
    desc_build();
    usb_force_detach();

    /* NVIC is enabled here (not in CubeMX) so that regeneration cannot drop it */
    HAL_NVIC_SetPriority(USB_HP_CAN_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USB_HP_CAN_TX_IRQn);
    HAL_NVIC_SetPriority(USB_LP_CAN_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);

    (void)HAL_PCD_Start(&hpcd_USB_FS);
}
