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
//#include "HardwareSerial.h"

#ifdef XINPUT_INTERFACE // defined by usb_dev.h -> usb_desc.h
#if F_CPU >= 20000000

static const uint32_t timeout = 250;  // ms

void (*usb_xinput_recv_callback)(void) = NULL;

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
	uint16_t count;

	if (!usb_configuration) return 0;
	count = usb_rx_byte_count(XINPUT_RX_ENDPOINT);
	return count;
}


// Function receives packets from the RX endpoint
int usb_xinput_recv(void *buffer, uint8_t nbytes)
{
	usb_packet_t *rx_packet;
	uint32_t begin = millis();

	while (1) {
		if (!usb_configuration) return -1;
		rx_packet = usb_rx(XINPUT_RX_ENDPOINT);
		if (rx_packet) break;
		if (millis() - begin > timeout || !timeout) return 0;
		yield();
	}
	memcpy(buffer, rx_packet->buf, nbytes);
	usb_free(rx_packet);
	return nbytes;
}

// Maximum number of transmit packets to queue so we don't starve other endpoints for memory
#define TX_PACKET_LIMIT 3

// Function used to send packets out of the TX endpoint
// This is used to send button reports
int usb_xinput_send(const void *buffer, uint8_t nbytes)
{
	usb_packet_t *tx_packet;
	uint32_t begin = millis();

	while (1) {
		if (!usb_configuration) return -1;
		if (usb_tx_packet_count(XINPUT_TX_ENDPOINT) < TX_PACKET_LIMIT) {
			tx_packet = usb_malloc();
			if (tx_packet) break;
		}
		if (millis() - begin > timeout) return 0;
		yield();
	}
	memcpy(tx_packet->buf, buffer, nbytes);
	tx_packet->len = nbytes;
	usb_tx(XINPUT_TX_ENDPOINT, tx_packet);
	return nbytes;
}

#endif // F_CPU
#endif // XINPUT_INTERFACE