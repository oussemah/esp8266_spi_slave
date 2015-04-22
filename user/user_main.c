/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#ifdef SERVER_SSL_ENABLE
#include "ssl/cert.h"
#include "ssl/private_key.h"
#else
#ifdef CLIENT_SSL_ENABLE
unsigned char *default_certificate;
unsigned int default_certificate_len = 0;
unsigned char *default_private_key;
unsigned int default_private_key_len = 0;
#endif
#endif

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "driver/spi.h"
#include "gpio.h"

#include "espconn.h"

static volatile os_timer_t dbgTimer;

void send_spi_char(void)
{
  static uint8_t c='0';
  
  char snd[] = "sending x \n\r";
  char rcp[] = "xxxxxxxxxx\r\n";
  snd[8]=(char)c;
  uart0_tx_buffer(snd, sizeof(snd));
  
  spi_send_string(HSPI, snd, sizeof(snd));
  
  spi_read_string(HSPI, rcp, sizeof(snd) - 5);
  
  
  //rcp[4] = (char) spi_tx_rx8(HSPI, c);
  
  uart0_tx_buffer(rcp, sizeof(rcp));
  
  c++;
  if (c > 'Z') {
	rcp[4] = (char) spi_tx_rx8(HSPI, '\0');
	c = '0';
  }
}

void do_nothing()
{
  static char c='0';
  
  spi_tx8(HSPI, c);
  c++;
  if (c > 'Z')
	c = '0';

  return;	
}

#define PRINT_MSG uart0_tx_buffer(buffer, sizeof(buffer));

void user_init(void)
{
  uint8 buffer[128] = {0};
  int result = 0;
  
  uart_init(115200, 115200);

  os_sprintf(buffer, "Starting up\r\n\0", result);
  PRINT_MSG

  spi_master_init(HSPI);

  os_sprintf(buffer, "setting timers\0", result);
  PRINT_MSG

 
  os_timer_disarm(&dbgTimer);
  os_timer_setfn(&dbgTimer, (os_timer_func_t *)send_spi_char, NULL);
  os_timer_arm(&dbgTimer, 200, 1);
}
