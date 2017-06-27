/*####COPYRIGHTBEGIN####
 -------------------------------------------
 Copyright (c) 2000-2002 by Greg Holloway, hollowaygm@telus.net

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

 Modified by Steve Karg <skarg@users.sourceforge.net> 15 June 2003
 -------------------------------------------
####COPYRIGHTEND####*/
//
// APDU receiving and simple processing function for I-Am service
//
#include "os.h"
#include "bacnet_struct.h"
#include "bacnet_const.h"
#include "bacnet_device.h"
#include "bacnet_api.h"
#include "bacdcode.h"
#include "ethernet.h"
#include "invoke_id.h"
#include "debug.h"
#include "net.h"
#include "main.h"


/* Local function to test whether a received device_id is in a list of
** 'interesting' device ID's
*/
static int isInRange(struct deviceRange *deviceRangeHead, int testVal)
{

    if (deviceRangeHead) {
        do {
            // printf( "Min: %d, Max: %d. testVal: %d\n", deviceRangeHead->min, deviceRangeHead->max, testVal);
            if (testVal >= deviceRangeHead->min
                && testVal <= deviceRangeHead->max) {
                return 1;
            }
        } while ((deviceRangeHead = deviceRangeHead->next));
        return 0;
    } else {
        return 1;
    }
}

/* handler for I-Am service */
// FIXME: send service request and len, rather than apdu...
void receive_IAm(uint8_t * apdu, int apdu_len,
    struct BACnet_Device_Address *src)
{
    int device_id = 0;
    int len = 0;
    int object_type;
    struct BACnet_Device_Info *dev_ptr;

    len = decode_object_id(&apdu[3], &object_type, &device_id);
    // load the data for device object
    if (object_type == OBJECT_DEVICE) {
        debug_printf(2, "receive-i-am: From Device %d\n", device_id);

        /* If this is not within the exclusive range of devices 
         ** we are looking for, just bail now 
         */
        if (!isInRange(deviceRangeList, device_id)) {
            debug_printf(2, "receive-i-am: Ignoring excluded device %d\n",
                device_id);
            return;
        }

        dev_ptr = device_add(device_id);        // device instance number
        if (!dev_ptr) {
            error_printf("Unable to allocate new device %d in table.\n",
                device_id);
            return;
        }
        /* max APDU is one byte */
        if (apdu[7] == 0x21) {
            dev_ptr->vendor_id = apdu[12];
            dev_ptr->max_apdu = apdu[8];
            dev_ptr->seg_support = apdu[10];
        }
        /* max APDU is two bytes */
        else {
            dev_ptr->vendor_id = apdu[13];
            dev_ptr->max_apdu = apdu[8] * 256 + apdu[9];
            dev_ptr->seg_support = apdu[11];
        }
        // don't update the address is it is not valid
        if (memcmp(Ethernet_Empty_MAC, src->mac,
                sizeof(Ethernet_Empty_MAC)) != 0)
            memmove(dev_ptr->src.mac, src->mac, sizeof(dev_ptr->src.mac));
        if (src->ip.s_addr > 0)
            dev_ptr->src.ip.s_addr = src->ip.s_addr;
        // should be no harm in updating the end device info            
        memmove(dev_ptr->src.adr, src->adr, sizeof(dev_ptr->src.adr));
        dev_ptr->src.len = src->len;
        dev_ptr->src.net = src->net;
        // FIXME: couldn't src.net be 0 to indicate local?
        /* device is on local network */
        if (dev_ptr->src.net == -1)
            dev_ptr->src.local = true;
        /* device is on remote network */
        else
            dev_ptr->src.local = false;
        // some comparison debug
        debug_printf(3, "receive-i-am: npdu   snet=%d slen=%d local=%d\n",
            (int) src->net, (int) src->len, (src->net == -1) ? 1 : 0);
        debug_printf(3, "receive-i-am: device snet=%d slen=%d local=%d\n",
            (int) dev_ptr->src.net,
            (int) dev_ptr->src.len, (int) dev_ptr->src.local);
        debug_printf(3, "receive-i-am: npdu   src_mac=%s src_ip=%s\n",
            hwaddrtoa(src->mac), inet_ntoa(src->ip));
        debug_printf(3, "receive-i-am: device src_mac=%s src_ip=%s\n",
            hwaddrtoa(dev_ptr->src.mac), inet_ntoa(dev_ptr->src.ip));
    }
    return;
}
