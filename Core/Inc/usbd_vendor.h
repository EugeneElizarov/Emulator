/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbd_vendor.h
  * @brief   Vendor-specific USB device (WinUSB via MS OS 2.0 descriptors).
  *          Enumeration only: EP0 control pipe, no data endpoints yet.
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef USBD_VENDOR_H
#define USBD_VENDOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ------------------------- project configuration ------------------------- */
#define USB_VID            0x2A73u   /* internal use only, not a licensed VID */
#define USB_PID            0x0101u
#define USB_BCD_DEVICE     0x0100u   /* bump to invalidate Windows descriptor cache */
#define USB_MANUFACTURER   "Elizarov Lab"
#define USB_PRODUCT        "Physio Emulator"   /* name shown in Device Manager */
#define USB_DEV_GUID       "{7B3E9C42-5A18-4D6F-9E27-3C81B4A05D69}"
#define USB_MS_VENDOR_REQ  0x20u     /* bMS_VendorCode in BOS platform capability */
#define USB_DETACH_MS      20u       /* forced SE0 so the host re-enumerates */

/* ------------------------------- public API ------------------------------ */
void    USBDV_Init(void);            /* call after MX_USB_PCD_Init() */
uint8_t USBDV_IsConfigured(void);    /* 1 once SET_CONFIGURATION(1) received */

#ifdef __cplusplus
}
#endif
#endif /* USBD_VENDOR_H */
