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
 Modified by Coleman Brumley <cbrumley@users.sourceforge.net> 18 Nov 2003
 Modified by Steve Karg <skarg@users.sourceforge.net> October 2004
 -------------------------------------------
####COPYRIGHTEND####*/
//
//This is coded such that it uses the entire packet from an 8802 packet
//it needs to be modified such that it *only* works with the NPDU stuff
//in other words, receive_8802 needs to be modified to strip off the data
//not needed here.  We do that already in receive_bip, but it needs 
// duplicated in receive_8802
//
// Receive NPDU from receive packet, then send to receive apdu function 
//
#include "os.h"
#include "bacnet_struct.h"
#include "bacnet_const.h"
#include "bacnet_api.h"
#include "packet.h"
#include "bits.h"
#include "debug.h"
#include "main.h"
#include "ethernet.h"
#include "net.h"


/* receives raw bytes and formats into NPDU */
/* returns 1 if a BACnet NPDU was received, 0 otherwise */
int receive_npdu(struct BACnet_NPDU *npdu)
{
    int apdu_offset = 0;        /* the first byte of the APDU */
    uint8_t *apdu = NULL;       /* the first byte of the APDU */
    int apdu_len = 0;           /* the first byte of the APDU */

#define TRIUMF
#ifdef TRIUMF
    npdu->version = npdu->pdu[0];
    npdu->control_byte = npdu->pdu[1];
    if (npdu->control_byte & BIT2)
        npdu->expecting_reply = 1;      /* BACnet-Confirmed Request PDU, BACnet Complex ACK PDU, or expecting reply */
    else
        npdu->expecting_reply = 0;      /* no reply is expected */

    /* Bits 0 and 1 are the priority, we just make it the lowest for now. */
    npdu->net_priority = 0;     /* default network priority to lowest */
    npdu->vendor_id = -1;
    npdu->hop_count = 0;

    debug_printf(3, "NPDU: Version: %d, Ctrl: %02X,  Buffer dump:\n",
        npdu->version, (unsigned) npdu->control_byte);
    debug_dump_data(3, npdu->pdu, npdu->pdu_len);

    switch (npdu->control_byte & '\xa8') {
        /* Msg Type = APDU, No Src, No Dest */
    case '\x0':
        npdu->src.len = 0;
        npdu->src.net = -1;
        npdu->dest.len = 0;
        npdu->dest.net = -1;
        npdu->src_present = false;
        npdu->dest_present = false;
        npdu->network_message = false;
        apdu_offset = 2;
        break;

        /* Msg Type = APDU,   Src, No Dest */
    case '\x08':
        npdu->src.len = npdu->pdu[4];
        npdu->src.net = npdu->pdu[3] + npdu->pdu[2] * 256;
        npdu->dest.len = 0;
        npdu->dest.net = -1;
        npdu->src_present = true;
        npdu->dest_present = false;
        npdu->network_message = false;
        apdu_offset = 5 + (npdu->src.len);
        break;

        /* Msg Type = APDU, No Src,   Dest */
    case '\x20':
        npdu->src.len = 0;
        npdu->src.net = -1;
        npdu->dest.len = npdu->pdu[4];
        npdu->dest.net = npdu->pdu[3] + npdu->pdu[2] * 256;
        npdu->hop_count = npdu->pdu[5 + (npdu->dest.len)];
        npdu->src_present = false;
        npdu->dest_present = true;
        npdu->network_message = false;
        apdu_offset = 5 + (npdu->dest.len) + 1;
        break;

        /* Msg Type = APDU,    Src,   Dest */
    case '\x28':
        npdu->dest.len = npdu->pdu[4];
        npdu->dest.net = npdu->pdu[3] + npdu->pdu[2] * 256;
        npdu->src.len = npdu->pdu[5 + npdu->dest.len + 2];
        npdu->src.net =
            npdu->pdu[5 + npdu->dest.len + 1] + npdu->pdu[5 +
            npdu->dest.len] * 256;
        npdu->hop_count =
            npdu->pdu[5 + (npdu->dest.len) + 3 + (npdu->src.len)];
        npdu->src_present = true;
        npdu->dest_present = true;
        npdu->network_message = false;
        apdu_offset = 5 + (npdu->dest.len) + 3 + (npdu->src.len) + 1;
        break;

        /* Msg Type = NetworkMsg, No Src, No Dest */
    case '\x80':
        npdu->src.len = 0;
        npdu->src.net = -1;
        npdu->dest.len = 0;
        npdu->dest.net = -1;
        npdu->src_present = false;
        npdu->dest_present = false;
        npdu->network_message = true;
        npdu->message_type = npdu->pdu[2];
        /* If  0x80 <= MsgType <= 0xFF, then 
         ** fetch Vendor ID and advance apdu_offset past Vendor ID field
         */
        if ((npdu->message_type >= 0x80) && (npdu->message_type <= 0xFF)) {
            npdu->vendor_id = npdu->pdu[3] * 256 + npdu->pdu[4];
            apdu_offset = 5;
        } else {
            npdu->vendor_id = -1;
            apdu_offset = 3;
        }
        break;

        /* Msg Type = NetworkMsg,   Src, No Dest */
    case '\x88':
        npdu->src.len = npdu->pdu[4];
        npdu->src.net = npdu->pdu[3] + npdu->pdu[2] * 256;
        npdu->dest.len = 0;
        npdu->dest.net = -1;
        npdu->src_present = true;
        npdu->dest_present = false;
        npdu->network_message = false;
        npdu->message_type = npdu->pdu[5 + (npdu->src.len)];
        /* If  0x80 <= MsgType <= 0xFF, then 
         ** fetch Vendor ID and advance apdu_offset past Vendor ID field
         */
        if ((npdu->message_type >= 0x80) && (npdu->message_type <= 0xFF)) {
            npdu->vendor_id =
                npdu->pdu[5 + npdu->src.len + 1] * 256 + npdu->pdu[5 +
                npdu->src.len + 2];
            apdu_offset = 5 + npdu->src.len + 3;
        } else {
            npdu->vendor_id = -1;
            apdu_offset = 5 + npdu->src.len + 1;
        }
        break;

        /* Msg Type = NetworkMsg, No Src,   Dest */
    case '\xa0':
        npdu->src.len = 0;
        npdu->src.net = -1;
        npdu->dest.len = npdu->pdu[4];
        npdu->dest.net = npdu->pdu[3] + npdu->pdu[2] * 256;
        npdu->hop_count = npdu->pdu[5 + (npdu->dest.len)];
        npdu->src_present = false;
        npdu->dest_present = true;
        npdu->network_message = false;
        apdu_offset = 6 + (npdu->dest.len);
        npdu->message_type = npdu->pdu[5 + (npdu->dest.len) + 1];
        /* If  0x80 <= MsgType <= 0xFF, then 
         ** fetch Vendor ID and advance apdu_offset past Vendor ID field
         */
        if ((npdu->message_type >= 0x80) && (npdu->message_type <= 0xFF)) {
            npdu->vendor_id =
                npdu->pdu[5 + npdu->dest.len + 3 + npdu->src.len +
                2] * 256 + npdu->pdu[5 + npdu->dest.len + 3 +
                npdu->src.len + 3];
            apdu_offset = 5 + npdu->dest.len + 3 + npdu->src.len + 4;
        } else {
            npdu->vendor_id = -1;
            apdu_offset = 5 + npdu->dest.len + 2;
        }
        break;

        /* Msg Type = APDU,    Src,   Dest */
    case '\xa8':
        npdu->dest.len = npdu->pdu[4];
        npdu->dest.net = npdu->pdu[3] + npdu->pdu[2] * 256;
        npdu->src.len = npdu->pdu[5 + npdu->dest.len + 2];
        npdu->src.net =
            npdu->pdu[5 + npdu->dest.len + 1] + npdu->pdu[5 +
            npdu->dest.len] * 256;
        npdu->hop_count =
            npdu->pdu[5 + (npdu->dest.len) + 3 + (npdu->src.len)];
        npdu->src_present = true;
        npdu->dest_present = true;
        npdu->network_message = true;

        npdu->message_type =
            npdu->pdu[5 + (npdu->dest.len) + 3 + npdu->src.len + 1];

        /* If  0x80 <= MsgType <= 0xFF, then  fetch Vendor ID
         */
        if ((npdu->message_type >= 0x80) && (npdu->message_type <= 0xFF)) {
            npdu->vendor_id =
                npdu->pdu[5 + npdu->dest.len + 3 + npdu->src.len +
                2] * 256 + npdu->pdu[5 + npdu->dest.len + 3 +
                npdu->src.len + 3];
            apdu_offset = 5 + npdu->dest.len + 3 + npdu->src.len + 4;
        } else {
            npdu->vendor_id = -1;
            apdu_offset = 5 + npdu->dest.len + 3 + npdu->src.len + 2;
        }
        break;


    default:
        debug_printf(3, "NPDU: Control byte %d\n", npdu->control_byte);
        break;
    }

    if (npdu->dest_present) {
        /* Ethernet and B/IP  or  ARCnet or MS/TP */
        if ((npdu->dest.len == 6) || (npdu->dest.len == 1))
            memmove(npdu->dest.adr, &npdu->pdu[5], npdu->dest.len);
        /* Broadcast */
        else if (npdu->dest.len == 0)
            memmove(npdu->dest.adr, Ethernet_Broadcast,
                sizeof(Ethernet_Broadcast));
    }

    /* Fill in npdu.src.adr field.  It's source depends on whether there is
     ** a Dest component in this PDU.  
     */
    if (npdu->src_present) {
        if (npdu->dest_present) {
            /* Ethernet and B/IP  or  ARCnet or MS/TP */
            if ((npdu->src.len == 6) || (npdu->src.len == 1))
                memmove(npdu->src.adr, &npdu->pdu[5 + npdu->dest.len + 3],
                    npdu->src.len);
        } else {
            /* Ethernet and B/IP  or  ARCnet or MS/TP */
            if ((npdu->src.len == 6) || (npdu->src.len == 1))
                memmove(npdu->src.adr, &npdu->pdu[5], npdu->src.len);
        }
    }                           /* Closing if(src_present)  */
#else
    int snet_start;

    debug_printf(3, "npdu: Packet length=%d\n", npdu->pdu_len);
    npdu->version = npdu->pdu[0];
    npdu->control_byte = npdu->pdu[1];  /* bit encoded Control Byte */
    debug_printf(3, "npdu: Control Byte=%02X\n", npdu->control_byte);

    /* figure out the NPDU */
    // Message Type field
    if (npdu->control_byte & BIT7)
        npdu->network_message = true;   /* This NSDU conveys a network layer message */
    else
        npdu->network_message = false;  /* This NSDU contains a BACnet APDU */

    /* Bit 6 is reserved */

    // Destination Specifier
    if (npdu->control_byte & BIT5)
        npdu->dest_present = true;      /* dest.net, dest.len, dest.adr, and Hop Count present */
    else
        npdu->dest_present = false;     /* dest.net, dest.len, dest.adr, and Hop Count absent */

    /* Bit 4 is reserved */

    // Source Specifier
    if (npdu->control_byte & BIT3)
        npdu->src_present = true;       /* SNET, src.len, and src.adr are present */
    else
        npdu->src_present = false;      /* SNET, src.len and src.adr are absent */

    // Data expecting Reply parameter
    if (npdu->control_byte & BIT2)
        npdu->expecting_reply = true;   /* BACnet-Confirmed Request PDU, BACnet Complex ACK PDU, or expecting reply */
    else
        npdu->expecting_reply = false;  /* no reply is expected */

    //Bits 0 and 1 are the priority, we just make it the lowest for now.
    npdu->net_priority = 0;     /* default network priority to lowest */

    if (!(npdu->src_present) && !(npdu->dest_present)) {        /* neither src or dest present */
        debug_printf(3, "npdu: Neither src nor dest are present\n");
        npdu->src.net = -1;
        npdu->src.len = 0;
        npdu->dest.net = -1;
        npdu->dest.len = 0;
        /* first byte of APDU (if neither src or dest present) */
        apdu_offset = 2;
    }

    /* Only the src information is available */
    else if ((npdu->src_present) && !(npdu->dest_present)) {    /* src present, destination absent */
        debug_printf(3, "npdu: src present, dest absent\n");
        npdu->src.net = npdu->pdu[2] * 256 + npdu->pdu[3];
        npdu->src.len = npdu->pdu[4];
        if (npdu->src.len == 6) /* Ethernet or B/IP */
            memmove(npdu->src.adr, &npdu->pdu[5], npdu->src.len);
        else if (npdu->src.len == 1)    /* ARCnet or MS/TP */
            memmove(npdu->src.adr, &npdu->pdu[5], npdu->src.len);
        npdu->dest.net = -1;
        npdu->dest.len = 0;
        /* calculate where the first byte of the APDU is */
        apdu_offset = 4 + npdu->src.len + 1;
    }

    /* Only the dest.net information is available */
    else if ((npdu->dest_present) && !(npdu->src_present)) {    /* destination present, src absent */
        debug_printf(3, "npdu: dest present, src absent\n");
        npdu->dest.net = npdu->pdu[2] * 256 + npdu->pdu[3];
        /* destination length.  if == 0, then a broadcast */
        npdu->dest.len = npdu->pdu[4];
        npdu->hop_count = npdu->pdu[5 + npdu->dest.len];        /* Hop count */
        //what is npdu->dest.len in b/ip?    
        //The dest.len and src.len for B/IP is 6 and 6
        //For B/IP, the src.adr is 4 byte IP follows by the 2 byte port
        if (npdu->dest.len == 6)        /* Ethernet */
            memmove(npdu->dest.adr, &npdu->pdu[5], npdu->dest.len);
        else if (npdu->dest.len == 1) { /* ARCnet or MS/TP */
            memmove(npdu->dest.adr, &npdu->pdu[5], npdu->dest.len);
        }
        npdu->src.net = -1;
        npdu->src.len = 0;
        /* calculate where the first byte of the APDU is */
        apdu_offset = 5 + npdu->dest.len + 1;
    }

    /* Both src and dst are available */
    else if ((npdu->dest_present) && (npdu->src_present)) {     /* destination and source present */
        debug_printf(3, "npdu: Both src and dest are present\n");
        npdu->dest.net = npdu->pdu[2] * 256 + npdu->pdu[3];
        /* destination length.  if == 0, then a broadcast */
        npdu->dest.len = npdu->pdu[4];
        //what is npdu->dest.len in b/ip?    
        //The dest.len and src.len for B/IP is 6 and 6
        //For B/IP, the src.adr is 4 byte IP followed by the 2 byte port
        // Note: if dest.len=0, dest.adr field is absent
        if (npdu->dest.len == 6)        /* Ethernet */
            memmove(npdu->dest.adr, &npdu->pdu[5], npdu->dest.len);
        else if (npdu->dest.len == 1)   /* ARCnet or MS/TP */
            memmove(npdu->dest.adr, &npdu->pdu[5], npdu->dest.len);
        snet_start = 5 + npdu->dest.len;
        npdu->src.net =
            npdu->pdu[snet_start] * 256 + npdu->pdu[snet_start + 1];
        npdu->src.len = npdu->pdu[snet_start + 2];
        //For B/IP, the src.adr is 4 byte IP follows by the 2 byte port
        if (npdu->src.len == 6) /* Ethernet */
            memmove(npdu->src.adr, &npdu->pdu[snet_start + 3],
                npdu->src.len);
        else if (npdu->src.len == 1)    /* ARCnet or MS/TP */
            memmove(npdu->src.adr, &npdu->pdu[snet_start + 3],
                npdu->src.len);
        npdu->hop_count = npdu->pdu[snet_start + npdu->src.len + 3];
        /* calculate where the first byte of the APDU is */
        apdu_offset = 4 + npdu->dest.len + 3 + npdu->src.len + 1 + 1;
    }

    /* network message at the end instead of an APDU */
    if (npdu->network_message) {
        /* not really the APDU */
        debug_printf(3, "npdu: Network layer message (no APDU)\n");
        npdu->message_type = npdu->pdu[apdu_offset];
        /* proprietary vendor message */
        if ((npdu->message_type >= 0x80) && (npdu->message_type <= 0xFF)) {
            npdu->vendor_id =
                npdu->pdu[apdu_offset + 1] * 256 +
                npdu->pdu[apdu_offset + 2];
            apdu_offset += 3;
        } else
            apdu_offset += 1;
    }
#endif

    debug_printf(3, "npdu: packet_len=%d apdu offset=%d\n",
        npdu->pdu_len, apdu_offset);
    debug_printf(3, "npdu: src.net=%d src.len=%d src.adr=%s\n",
        npdu->src.net, npdu->src.len, hwaddrtoa(npdu->src.adr));
    debug_printf(3, "npdu: dest.net=%d dest.len=%d dest.adr=%s\n",
        npdu->dest.net, npdu->dest.len, hwaddrtoa(npdu->dest.adr));
    debug_printf(3, "npdu: hop-count=%d\n", npdu->hop_count);

    /* if we made it to here, there is an APDU waiting to be read */

    /* length of APDU in bytes */
    apdu_len = npdu->pdu_len - apdu_offset;
    debug_printf(3, "npdu: apdu_len=%d\n", apdu_len);
    /* no good */
    if (apdu_len > MAX_APDU)
        return 0;
    apdu = &npdu->pdu[apdu_offset];

    /* NPDU is fully figured out */
    if (debug_get_level() >= 5)
        display_packet(npdu, apdu, apdu_len);

    /* hand off NPDU to APDU for further breakdown */
    receive_apdu(apdu, apdu_len, &npdu->src);

    return 1;                   /* a BACnet packet was successfully read */
}

/* end of receive_npdu.c network functions */
