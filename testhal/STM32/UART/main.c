/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"

static VirtualTimer vt1, vt2;

static void restart(void *p) {

  (void)p;
  uartStartSend(&UARTD2, 14, "Hello World!\r\n");
}

static void ledoff(void *p) {

  (void)p;
  palSetPad(IOPORT3, GPIOC_LED);
}

/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1(void) {

  palClearPad(IOPORT3, GPIOC_LED);
}

/*
 * This callback is invoked when a transmission has phisically completed.
 */
static void txend2(void) {

  palSetPad(IOPORT3, GPIOC_LED);
  chSysLockFromIsr();
  chVTSetI(&vt1, MS2ST(5000), restart, NULL);
  chSysUnlockFromIsr();
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerr(uartflags_t e) {

  (void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxchar(uint16_t c) {

  (void)c;
  /* Flashing the LED each time a character is received.*/
  palClearPad(IOPORT3, GPIOC_LED);
  chSysLockFromIsr();
  if (!chVTIsArmedI(&vt2))
    chVTSetI(&vt2, MS2ST(200), ledoff, NULL);
  chSysUnlockFromIsr();
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxend(void) {

}

/*
 * UART driver configuration structure.
 */
static UARTConfig uart_cfg_1 = {
  txend1,
  txend2,
  rxend,
  rxchar,
  rxerr,
  38400,
  0,
  USART_CR2_LINEN,
  0
};

/*
 * Entry point, note, the main() function is already a thread in the system
 * on entry.
 */
int main(int argc, char **argv) {

  (void)argc;
  (void)argv;

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  uartStart(&UARTD2, &uart_cfg_1);

  /*
   * Starts the transmission, it will be handled entirely in background.
   */
  uartStartSend(&UARTD2, 13, "Starting...\r\n");

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
  return 0;
}
