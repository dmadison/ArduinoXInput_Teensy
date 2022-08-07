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

#ifndef USBxinput_h_
#define USBxinput_h_

#include "usb_desc.h"

#if defined(XINPUT_INTERFACE)

#include <inttypes.h>
#include <stdbool.h>

// C language implementation
#ifdef __cplusplus
extern "C" {
#endif
void usb_xinput_configure();
bool usb_xinput_connected(void);
uint16_t usb_xinput_available(void);
int usb_xinput_send(const void *buffer, uint8_t nbytes);
int usb_xinput_recv(void *buffer, uint8_t nbytes);
extern void (*usb_xinput_recv_callback)(void);
#ifdef __cplusplus
}
#endif


// C++ interface
#ifdef __cplusplus
class XInputUSB
{
public:
  static bool connected(void) { return usb_xinput_connected(); }
  static uint16_t available(void) { return usb_xinput_available(); }
  static int send(const void *buffer, uint8_t nbytes) { return usb_xinput_send(buffer, nbytes); }
  static int recv(void *buffer, uint8_t nbytes) { return usb_xinput_recv(buffer, nbytes); }
  static void setRecvCallback(void (*callback)(void)) { usb_xinput_recv_callback = callback; }
};

#endif // __cplusplus

#endif // XINPUT_INTERFACE

#endif // USBxinput_h_
