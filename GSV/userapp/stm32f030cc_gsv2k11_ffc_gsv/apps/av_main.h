/**
 * @file av_main.h
 *
 * @brief sample main entry for audio/video based software
 */

/**
 * @brief  sample main entry for audio/video based software
 * @return never return
 */
#ifndef __av_main_h
#define __av_main_h

#ifdef GSV_BASE_PROJECT
int main(void);
#else
#include "sys.h"
int GsvMain(void);
#endif

#include "bsp.h"   /* this file includes low level i2c/uart/timer/etc. functions on customer platform */
#include "kapi.h"  /* this file includes kernal APIs */
#include "av_uart_cmd.h" /* accept command */
#include "av_key_cmd.h" /* accept key */
#include "av_irda_cmd.h" /* accept ir */
#include "av_edid_manage.h" /* edid manage */
#include "av_event_handler.h" /* routing and event */

#if GSV1K
#include "gsv1k_device.h"
#endif
#if GSV2K0
#include "gsv2k0_device.h"
#endif
#if GSV2K11
#include "gsv2k11_device.h"
#endif

#endif
