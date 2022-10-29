/* MIT License
 *
 * Copyright (c) 2016 Zachery Littell
 * Modified  (c) 2019 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "usb_dev.h"
#include "usb_xinput.h"
#include "core_pins.h" // for yield(), millis()
#include <string.h>    // for memcpy()
#include "avr/pgmspace.h" // for DMAMEM

#ifdef XINPUT_INTERFACE // defined by usb_dev.h -> usb_desc.h

#define TX_NUM  4
#define TX_TIMEOUT_MSEC 250
#define TX_BUFSIZE 32
static transfer_t tx_transfer[TX_NUM] __attribute__ ((used, aligned(32)));
DMAMEM static uint8_t txbuffer[TX_NUM * TX_BUFSIZE] __attribute__ ((aligned(32)));
static uint8_t tx_head=0;
static uint8_t transmit_previous_timeout=0;


#define RX_NUM 6
static transfer_t rx_transfer[RX_NUM] __attribute__ ((used, aligned(32)));
DMAMEM static uint8_t rx_buffer[RX_NUM * XINPUT_RX_SIZE] __attribute__ ((aligned(32)));
static uint16_t rx_count[RX_NUM];
static volatile uint8_t rx_head;
static volatile uint8_t rx_tail;
static uint8_t rx_list[RX_NUM + 1];
static volatile uint32_t rx_available;
static volatile uint8_t in_rx_event = 0;
static void rx_queue_transfer(int i);
static void rx_event(transfer_t *t);

void (*usb_xinput_recv_callback)(void) = NULL;


static void rx_queue_transfer(int i)
{
  NVIC_DISABLE_IRQ(IRQ_USB1);
  void *buffer = rx_buffer + i * XINPUT_RX_SIZE;
  usb_prepare_transfer(rx_transfer + i, buffer, XINPUT_RX_SIZE, i);
  arm_dcache_delete(buffer, XINPUT_RX_SIZE);
  usb_receive(XINPUT_RX_ENDPOINT, rx_transfer + i);
  NVIC_ENABLE_IRQ(IRQ_USB1);
}

// called by USB interrupt when any packet is received
static void rx_event(transfer_t *t)
{
  int len = XINPUT_RX_SIZE - ((t->status >> 16) & 0x7FFF);
  int i = t->callback_param;
  if (len > 0) {
    uint32_t head = rx_head;
    rx_count[i] = len;
    if (++head > RX_NUM) head = 0;
    rx_list[head] = i;
    rx_head = head;
    rx_available += len;

    if (usb_xinput_recv_callback)
    {
      in_rx_event = 1;
      usb_xinput_recv_callback();
      in_rx_event = 0;
    }

  } else {
    // received a zero length packet
    rx_queue_transfer(i);
  }
}

void usb_xinput_configure()
{
  memset(tx_transfer, 0, sizeof(tx_transfer));
  tx_head = 0;
  usb_config_tx(XINPUT_TX_ENDPOINT, XINPUT_TX_SIZE, 0, NULL);

  memset(rx_transfer, 0, sizeof(rx_transfer));
  memset(rx_count, 0, sizeof(rx_count));
  rx_head = 0;
  rx_tail = 0;
  rx_available = 0;
  in_rx_event = 0;
  usb_config_rx(XINPUT_RX_ENDPOINT, XINPUT_RX_SIZE, 0, rx_event);
  for (int i=0; i < RX_NUM; i++) rx_queue_transfer(i);
}

extern volatile uint8_t usb_configuration;

// Function returns whether the microcontroller's USB
// is configured or not (connected to driver)
bool usb_xinput_connected(void)
{
  return usb_configuration;
}

// Function to check if packets are available
// to be received on the RX endpoint
uint16_t usb_xinput_available(void)
{
  return rx_available;
}


// Function receives packets from the RX endpoint
int usb_xinput_recv(void *buffer, uint8_t nbytes)
{
  int count = 0;

  if (!in_rx_event)
    NVIC_DISABLE_IRQ(IRQ_USB1);

  uint32_t tail = rx_tail;
  if (tail != rx_head)
  {
    if (++tail > RX_NUM) tail = 0;
    uint32_t ii = rx_list[tail];

    count = rx_count[ii];
    if (nbytes < count)
      count = nbytes;

    uint8_t* p = rx_buffer + ii * XINPUT_RX_SIZE;
    memcpy(buffer, p, count);

    rx_available -= rx_count[ii];
    rx_tail = tail;
    rx_queue_transfer(ii);
  }

  if (!in_rx_event)
    NVIC_ENABLE_IRQ(IRQ_USB1);

  return count;
}

// Function used to send packets out of the TX endpoint
// This is used to send button reports
int usb_xinput_send(const void *_buffer, uint8_t nbytes)
{
  if (nbytes > TX_BUFSIZE) return -1;
  if (!usb_configuration) return -1;
  uint32_t head = tx_head;
  transfer_t *xfer = tx_transfer + head;
  uint32_t wait_begin_at = systick_millis_count;
  while (1) {
    uint32_t status = usb_transfer_status(xfer);
    if (!(status & 0x80)) {
      if (status & 0x68) {
        // TODO: what if status has errors???
        //printf("ERROR status = %x, i=%d, ms=%u\n",
        //        status, tx_head, systick_millis_count);
      }
      transmit_previous_timeout = 0;
      break;
    }
    if (transmit_previous_timeout) return -1;
    if (systick_millis_count - wait_begin_at > TX_TIMEOUT_MSEC) {
      // waited too long, assume the USB host isn't listening
      transmit_previous_timeout = 1;
      return -1;
    }
    if (!usb_configuration) return -1;
    yield();
  }
  delayNanoseconds(30); // TODO: why is status ready too soon?
  uint8_t *buffer = txbuffer + head * TX_BUFSIZE;
  memcpy(buffer, _buffer, nbytes);
  usb_prepare_transfer(xfer, buffer, nbytes, 0);
  arm_dcache_flush_delete(buffer, TX_BUFSIZE);
  usb_transmit(XINPUT_TX_ENDPOINT, xfer);
  if (++head >= TX_NUM) head = 0;
  tx_head = head;
  return 0;
}

#endif // XINPUT_INTERFACE
