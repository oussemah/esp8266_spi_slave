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
  
  char msg[] = "sending x \n\r";
  msg[8]=(char)c;
  uart0_tx_buffer(msg, sizeof(msg));
  
  spi_mast_byte_write(HSPI,c);
  c++;
  if (c > 'Z')
	c = '0';
}


void interupt_test() {
   ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts
   //wdt_feed();

   uint32 gpio_status;
   gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
   //clear interrupt status
   GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

   //ets_uart_printf("GPIO Interrupt!\r\n");
   send_spi_char();

   ETS_GPIO_INTR_ENABLE(); // Enable gpio interrupts
}

void ICACHE_FLASH_ATTR gpio_init() {
   ETS_GPIO_INTR_DISABLE(); // Disable gpio interrupts
   ETS_GPIO_INTR_ATTACH(interupt_test, 2); // GPIO12 interrupt handler
   PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2); // Set GPIO12 function
   gpio_output_set(0, 0, 0, GPIO_ID_PIN(2)); // Set GPIO12 as input
   GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(2)); // Clear GPIO12 status
   gpio_pin_intr_state_set(GPIO_ID_PIN(2), 1); // Interrupt on any GPIO12 edge
   ETS_GPIO_INTR_ENABLE(); // Enable gpio interrupts
   //wdt_feed();
}

void do_nothing()
{
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

  gpio_init();
  
  interupt_test();

  os_timer_disarm(&dbgTimer);
  os_timer_setfn(&dbgTimer, (os_timer_func_t *)do_nothing, NULL);
  os_timer_arm(&dbgTimer, 1000, 1);
}
