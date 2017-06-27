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
// Handle Read-Property requests that come from other devices
// currently, values given back come from this function, but this 
// this should be improved to allow more objects and flexibility 
//
// TODO:  Make generic functions for encoding the content to
// allow for a larger complex server device. Breakout to function
// calls each object type that is requested (or not).
//
#include "os.h"
#include "bacnet_struct.h"
#include "bacnet_enum.h"
#include "bacnet_const.h"
#include "bacnet_api.h"
#include "bacnet_device.h"
#include "bacnet_text.h"
#include "bacdcode.h"
#include "reject.h"
#include "debug.h"
#include "pdu.h"
#include "options.h"
#include "version.h"

// from main.c
extern int BACnet_Time_Sync_Seconds;
extern int BACnet_COV_Support;

int encode_device_property(uint8_t * apdu,
    enum BACnetPropertyIdentifier property, int array_index)
{
    int apdu_len = 0;           /* length of new apdu to be sent */
    char *name_string = { "BACnet4Linux" };
    char *description_string = { "BACnet Stack for Linux" };
    char *vendor_string = { "GNU" };
    time_t t;                   /* seconds since epoch time */
    struct tm *my_tm;           // local date and time
    BACNET_BIT_STRING bit_string;
    int i = 0;

    switch (property) {
    case PROP_OBJECT_IDENTIFIER:
        apdu_len = encode_tagged_object_id(&apdu[0], OBJECT_DEVICE,
            BACnet_Device_Instance);
        break;
    case PROP_OBJECT_NAME:
        apdu_len = encode_tagged_character_string(&apdu[0], name_string);
        break;
    case PROP_OBJECT_TYPE:
        apdu_len = encode_tagged_enumerated(&apdu[0], OBJECT_DEVICE);
        break;
    case PROP_DESCRIPTION:
        apdu_len =
            encode_tagged_character_string(&apdu[0], description_string);
        break;
    case PROP_SYSTEM_STATUS:
        apdu_len =
            encode_tagged_enumerated(&apdu[0],
            STATUS_OPERATIONAL_READ_ONLY);
        break;
    case PROP_VENDOR_NAME:
        apdu_len = encode_tagged_character_string(&apdu[0], vendor_string);
        break;
    case PROP_VENDOR_IDENTIFIER:
        apdu_len =
            encode_tagged_unsigned(&apdu[0], BACnet_Vendor_Identifier);
        break;
    case PROP_MODEL_NAME:
        apdu_len = encode_tagged_character_string(&apdu[0], name_string);
        break;
    case PROP_FIRMWARE_REVISION:
        apdu_len =
            encode_tagged_character_string(&apdu[0],
            (char *) Program_Version);
        break;
    case PROP_APPLICATION_SOFTWARE_VERSION:
        apdu_len =
            encode_tagged_character_string(&apdu[0],
            (char *) Program_Version);
        break;
    case PROP_LOCAL_TIME:
        t = time(NULL);
        my_tm = localtime(&t);
        apdu_len =
            encode_tagged_time(&apdu[0], my_tm->tm_hour, my_tm->tm_min,
            my_tm->tm_sec, 0);
        break;
    case PROP_LOCAL_DATE:
        t = time(NULL);
        my_tm = localtime(&t);
        // year = years since 1900
        // month 1=Jan
        // day = day of month
        // wday 1=Monday...7=Sunday
        apdu_len = encode_tagged_date(&apdu[0],
            my_tm->tm_year,
            my_tm->tm_mon + 1,
            my_tm->tm_mday, ((my_tm->tm_wday == 0) ? 7 : my_tm->tm_wday));
        break;
    case PROP_PROTOCOL_VERSION:
        apdu_len = encode_tagged_unsigned(&apdu[0], 1);
        break;
    case PROP_PROTOCOL_CONFORMANCE_CLASS:
        apdu_len = encode_tagged_unsigned(&apdu[0], 1);
        break;
    case PROP_PROTOCOL_SERVICES_SUPPORTED:
        bitstring_init(&bit_string);
        for (i = 0; i < MAX_BACNET_SERVICES_SUPPORTED; i++) {
            // initialize all the services to not-supported
            bitstring_set_bit(&bit_string, i, false);
        }
        bitstring_set_bit(&bit_string, SERVICE_SUPPORTED_WHO_IS, true);
        bitstring_set_bit(&bit_string, SERVICE_SUPPORTED_I_AM, true);
        bitstring_set_bit(&bit_string, SERVICE_SUPPORTED_READ_PROPERTY,
            true);
        if (BACnet_Time_Sync_Seconds)
            bitstring_set_bit(&bit_string,
                SERVICE_SUPPORTED_TIME_SYNCHRONIZATION, true);
        if (BACnet_COV_Support)
            bitstring_set_bit(&bit_string,
                SERVICE_SUPPORTED_CONFIRMED_COV_NOTIFICATION, true);
        apdu_len = encode_tagged_bitstring(&apdu[0], &bit_string);
        break;
    case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED:
        bitstring_init(&bit_string);
        bitstring_set_bit(&bit_string, OBJECT_ANALOG_INPUT, false);
        bitstring_set_bit(&bit_string, OBJECT_ANALOG_OUTPUT, false);
        bitstring_set_bit(&bit_string, OBJECT_ANALOG_VALUE, false);
        bitstring_set_bit(&bit_string, OBJECT_BINARY_INPUT, false);
        bitstring_set_bit(&bit_string, OBJECT_BINARY_OUTPUT, false);
        bitstring_set_bit(&bit_string, OBJECT_BINARY_VALUE, false);
        bitstring_set_bit(&bit_string, OBJECT_CALENDAR, false);
        bitstring_set_bit(&bit_string, OBJECT_COMMAND, false);
        bitstring_set_bit(&bit_string, OBJECT_DEVICE, true);
        bitstring_set_bit(&bit_string, OBJECT_EVENT_ENROLLMENT, false);
        bitstring_set_bit(&bit_string, OBJECT_FILE, false);
        bitstring_set_bit(&bit_string, OBJECT_GROUP, false);
        bitstring_set_bit(&bit_string, OBJECT_LOOP, false);
        bitstring_set_bit(&bit_string, OBJECT_MULTI_STATE_INPUT, false);
        bitstring_set_bit(&bit_string, OBJECT_MULTI_STATE_OUTPUT, false);
        bitstring_set_bit(&bit_string, OBJECT_NOTIFICATION_CLASS, false);
        bitstring_set_bit(&bit_string, OBJECT_PROGRAM, false);
        bitstring_set_bit(&bit_string, OBJECT_SCHEDULE, false);
        bitstring_set_bit(&bit_string, OBJECT_AVERAGING, false);
        bitstring_set_bit(&bit_string, OBJECT_MULTI_STATE_VALUE, false);
        bitstring_set_bit(&bit_string, OBJECT_TRENDLOG, false);
        bitstring_set_bit(&bit_string, OBJECT_LIFE_SAFETY_POINT, false);
        bitstring_set_bit(&bit_string, OBJECT_LIFE_SAFETY_ZONE, false);
        apdu_len = encode_tagged_bitstring(&apdu[0], &bit_string);
        break;
    case PROP_OBJECT_LIST:
        if (array_index == 0)
            apdu_len = encode_tagged_unsigned(&apdu[0], 1);
        else
            apdu_len = encode_tagged_object_id(&apdu[0], OBJECT_DEVICE,
                BACnet_Device_Instance);
        break;
    case PROP_MAX_APDU_LENGTH_ACCEPTED:
        apdu_len = encode_tagged_unsigned(&apdu[0], MAX_APDU);
        break;
    case PROP_SEGMENTATION_SUPPORTED:
        apdu_len = encode_tagged_enumerated(&apdu[0], SEGMENTATION_NONE);
        break;
    case PROP_APDU_TIMEOUT:
        apdu_len =
            encode_tagged_unsigned(&apdu[0], BACnet_APDU_Timeout * 1000);
        break;
    case PROP_NUMBER_OF_APDU_RETRIES:
        /* to change this, edit invoke_id.c */
        apdu_len = encode_tagged_unsigned(&apdu[0], 1);
        break;
    default:
        break;
    }

    return apdu_len;
}


//Perhaps, here, we need to just pass the apdu service request?
int receive_readproperty(uint8_t * service_request, int service_len,
    struct BACnet_Device_Address *src, int src_max_apdu, uint8_t invoke_id)
{
    int object = 0, property = 0;
    int instance = 0;
    int apdu_len = 0;           /* length of new apdu to be sent */
    int offset = 0;             /* length of encodings */
    int who_sent = 0;           /* which device made the request */
    time_t t = 0;               /* seconds since epoch time */
    struct tm t_struct;
    struct tm *t_ptr = NULL;    /* broken down time structure */
    bool abort_message = false;
    bool error_message = false;
    uint8_t *apdu = NULL;       // for sending message
    uint32_t array_index = 0;
    uint8_t tag_number = 0;
    uint32_t len_value_type = 0;

    debug_printf(5, "RRP: Entered 'receive_readproperty'\n");
    t_ptr = &t_struct;
    who_sent = device_which_sent(src);
    debug_printf(2, "RRP:From device %d\n", who_sent);
    t = time(NULL);
    t_ptr = localtime(&t);
    // Tag 0: Object ID         
    offset = 0;
    if (!decode_is_context_tag(&service_request[offset], 0))
        goto error_reject;
    offset++;
    offset +=
        decode_object_id(&service_request[offset], &object, &instance);
    // Tag 1: Property ID
    offset += decode_tag_number_and_value(&service_request[offset],
        &tag_number, &len_value_type);
    if (tag_number != 1)
        goto error_reject;
    offset += decode_enumerated(&service_request[offset], len_value_type,
        &property);
    // Tag 2: Optional Array Index
    if (offset < service_len) {
        offset += decode_tag_number_and_value(&service_request[offset],
            &tag_number, &len_value_type);
        if (tag_number == 2) {
            offset +=
                decode_unsigned(&service_request[offset], len_value_type,
                &array_index);
            debug_printf(2,
                "RRP: Device %d is looking for %s %d : %s[%u]\n", who_sent,
                enum_to_text_object(object), instance,
                enum_to_text_property(property), array_index);
        } else {
            debug_printf(2, "RRP: Device %d is looking for %s %d : %s\n",
                who_sent, enum_to_text_object(object),
                instance, enum_to_text_property(property));
            array_index = BACNET_ARRAY_ALL;
        }

    } else {
        debug_printf(2, "RRP: Device %d is looking for %s %d : %s\n",
            who_sent, enum_to_text_object(object),
            instance, enum_to_text_property(property));
        array_index = BACNET_ARRAY_ALL;
    }

    apdu = pdu_alloc();
    if (apdu) {
        if ((object == OBJECT_DEVICE)
            && (instance == BACnet_Device_Instance)) {
            /* prepare a complex ACK response */
            apdu[0] = PDU_TYPE_COMPLEX_ACK;     /* complex ACK service */
            apdu[1] = invoke_id;        /* original invoke id from request */
            apdu[2] = SERVICE_CONFIRMED_READ_PROPERTY;  // service choice
            apdu_len = 3;
            // service ack follows
            apdu_len +=
                encode_context_object_id(&apdu[apdu_len], 0,
                object, instance);
            apdu_len +=
                encode_context_enumerated(&apdu[apdu_len], 1, property);
            // context 2 array index is optional
            if (array_index != BACNET_ARRAY_ALL) {
                apdu_len +=
                    encode_context_unsigned(&apdu[apdu_len], 2,
                    array_index);
            }
            // propertyValue
            apdu_len += encode_opening_tag(&apdu[apdu_len], 3);
            offset = encode_device_property(&apdu[apdu_len],
                property, array_index);
            apdu_len += offset;
            /* no match for property send back error */
            if (offset == 0) {
                send_error_address(src, invoke_id,
                    SERVICE_CONFIRMED_READ_PROPERTY, ERROR_CLASS_PROPERTY,
                    ERROR_CODE_UNKNOWN_PROPERTY);
                error_message = true;
            } else {
                apdu_len += encode_closing_tag(&apdu[apdu_len], 3);
            }
        } else {
            send_error_address(src, invoke_id,
                SERVICE_CONFIRMED_READ_PROPERTY, ERROR_CLASS_OBJECT,
                ERROR_CODE_UNKNOWN_OBJECT);
            error_message = true;
        }

        if (!error_message) {
            /* send complex ACK response */
            debug_printf(2, "RRP: Sending response to %d...\n", who_sent);
            /* hand the APDU off for the completion of the NPDU */
            if (apdu_len > src_max_apdu)
                send_abort_address(src, invoke_id,
                    ABORT_REASON_SEGMENTATION_NOT_SUPPORTED);
            else
                send_npdu_address(src, &apdu[0], apdu_len);
        }
        pdu_free(apdu);
    } else
        abort_message = true;

    if (abort_message) {
      error_reject:
        send_abort_address(src, invoke_id, ABORT_REASON_OTHER);
        debug_printf(2,
            "RRP: Sending an Abort (other) back to %d\n", who_sent);
    }

    return 1;
}
