/* MIT License
 * 
 * Copyright (c) 2016 Zachery Littell
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

// C language implementation
#ifdef __cplusplus
extern "C" {
#endif
int usb_xinput_recv(void *buffer, uint32_t timeout);
int usb_xinput_available(void);
int usb_xinput_send(const void *buffer, uint32_t timeout);
#ifdef __cplusplus
}
#endif


// C++ interface
#ifdef __cplusplus
class usb_xinput_class
{
public:
	int available(void) {return usb_xinput_available(); }
	int recv(void *buffer, uint16_t timeout) { return usb_xinput_recv(buffer, timeout); }
	int send(const void *buffer, uint16_t timeout) { return usb_xinput_send(buffer, timeout); }
};

extern usb_xinput_class XInputUSB;

#endif // __cplusplus

#endif // XINPUT_INTERFACE

#endif // USBxinput_h_
