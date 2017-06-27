/*####COPYRIGHTBEGIN####
 -------------------------------------------
 Copyright (c) 2004 by Steve Karg <skarg@users.sourceforge.net>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation; either version 2.1
 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public 
 License along with this program; if not, write to 
 The Free Software Foundation, Inc.
 59 Temple Place - Suite 330 
 Boston, MA  02111-1307, USA.

 (See the included file COPYING)
 -------------------------------------------
####COPYRIGHTEND####*/
#ifndef REJECT_H
#define REJECT_H

#include <stdint.h>

void send_reject(int device_instance, uint8_t invoke_id,
    uint8_t reject_reason);
void send_abort(int device_instance, uint8_t invoke_id,
    uint8_t abort_reason);
void send_error(int device_instance, uint8_t invoke_id,
    uint8_t service, int error_class, int error_code);

void send_reject_address(struct BACnet_Device_Address *dest,
    uint8_t invoke_id, uint8_t reject_reason);
void send_abort_address(struct BACnet_Device_Address *dest,
    uint8_t invoke_id, uint8_t abort_reason);
void send_error_address(struct BACnet_Device_Address *dest,
    uint8_t invoke_id, uint8_t service, int error_class, int error_code);

#endif
