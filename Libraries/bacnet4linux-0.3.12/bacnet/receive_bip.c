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
#include "os.h"
#include "debug.h"
#include "main.h"
#include "net.h"
#include "pdu.h"
#include "bacnet_api.h"
#include "bacnet_const.h"

static void get_dest_address(struct msghdr msgh, struct in_addr *dest_addr)
{
    struct cmsghdr *cmsgptr;
    struct in_addr *dstaddr_ptr;

    for (cmsgptr = CMSG_FIRSTHDR(&msgh);
        cmsgptr != NULL; cmsgptr = CMSG_NXTHDR(&msgh, cmsgptr)) {
        if ((cmsgptr->cmsg_level == IPPROTO_IP) &&
            (cmsgptr->cmsg_type == DSTADDR_SOCKOPT)) {
            dstaddr_ptr = dstaddr(cmsgptr);
            memcpy(dest_addr, dstaddr_ptr, sizeof(struct in_addr));
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////
/* function to receive UDP packets */
void receive_bip(void)
{
    int bytes = 0;
    int addrlen = 0, bvlc_len = 0;
    struct BACnet_NPDU *npdu;
    struct in_addr dest_addr = { 0 };
    struct sockaddr_in cliaddr = { 0 };
    struct sockaddr_in addr = { 0 };
    struct msghdr msgh = { 0 };
    union control_data cmsg = { {0} };
    struct iovec iov[1] = { {0} };
    int same_src = 0;
    uint8_t *mtu = NULL;

    /* Make sure the socket is open */
    if (bip_sockfd < 0)
        return;

    /* Initialize the structure sizes */
    addrlen = sizeof(struct sockaddr_in);
    memset(&msgh, 0, sizeof(msgh));
    msgh.msg_name = &cliaddr;
    msgh.msg_namelen = sizeof(cliaddr);
    msgh.msg_iov = iov;
    msgh.msg_iovlen = 1;
    msgh.msg_control = &cmsg;
    msgh.msg_controllen = sizeof(cmsg);

    /* Peek at the packet to get the header information (src/dst addresses) */
    bytes = recvmsg(bip_sockfd, &msgh, MSG_PEEK);
#if 0
    // for some reason this returns zero bytes.
    if (bytes <= 0) {
        debug_printf(5, "receive_bip: 0 bytes received!\n");
        if (bytes != 0)
            perror("receive_bip");
        return;
    }
#endif
    // allocate space for our message
    npdu = npdu_alloc();
    mtu = calloc(1, DEFAULT_MTU);
    if (!mtu || !npdu) {
        error_printf("unable to allocate memory: %s\n", strerror(errno));
        if (mtu)
            free(mtu);
        if (npdu)
            npdu_free(npdu);
        return;
    }
    //get the destination address
    get_dest_address(msgh, &dest_addr);
    debug_printf(2, "receive_bip: DST=%s SRC=%s:%4X \n",
        inet_ntoa(dest_addr),
        inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    /* Read the data from the socket */
    bytes =
        recvfrom(bip_sockfd, &mtu[0], DEFAULT_MTU, 0,
        (struct sockaddr *) &addr, &addrlen);
    // got nothing...
    if (bytes <= 0) {
        debug_printf(4, "receive_bip: 0 bytes received!\n");
        goto free_packet;
    }
    // see if it is from me
    //compare the source address with our local address
    same_src =
        memcmp(&cliaddr.sin_addr, &BACnet_Device_IP_Address,
        sizeof(cliaddr.sin_addr));
    //if the source address is the same as our local address
    //don't do anything with it
    if (same_src == 0) {
        debug_printf(3,
            "receive_bip: source address matches local address\n");
        goto free_packet;
    }
    //
    /* check if this is a packet we want to process */
    /* BVLC packets begin with 0x81 and the function (buf[1]) goes from 0x01 .. 0x0B */
    /* Original-Unicast-NPDU and Original-Broadcast-NPDU have the following header format (always 4 bytes)
     *****************************************************************************************************************
     * BVLC Type:      1 byte      0x81     BVLL for BACnet/IP
     * BVLC Function:  1 byte      0x0A or 0x0B   Original-Unicast-NPDU (0x0A), Original-Broadcast-NPDU (0x0B)
     * BVLC Length: 2 bytes     L     Length L, in octets, of the BVLL message
     * BACnet NPDU: Variable Length
     *****************************************************************************************************************
     */
    if ((mtu[0] == 0x81)) {
        // TODO:
        // -- for now, we're only going to support Original-Unicast-NPDU and Original-Broadcast-NPDU
        //    since we aren't interested in being a BBMD or the rest of BVLC right now
        if ((mtu[1] == Original_Unicast_NPDU)
            || (mtu[1] == Original_Broadcast_NPDU)) {
            //copy bytes from bvlci buffer to npdu buffer
            bvlc_len = (int) mtu[2] * 256 + (int) mtu[3];
            npdu->pdu_len = bvlc_len - 4;
            memmove(&npdu->src.ip, &cliaddr.sin_addr,
                sizeof(npdu->src.ip));
            npdu->pdu = &mtu[4];
            debug_printf(3,
                "receive_bip: Rec'd BVLL from %s len = %d "
                "bytes = %d function = %02X\n",
                inet_ntoa(npdu->src.ip), bvlc_len, bytes, mtu[1]);
            debug_printf(3, "receive_bip: bvlc_len %d\n", bvlc_len);
            debug_printf(3, "receive_bip: npdu len %d\n", npdu->pdu_len);
            debug_printf(4, "receive_bip: whole packet:\n");
            debug_dump_data(4, mtu, bvlc_len);
            receive_npdu(npdu);
        }
    } else
        debug_printf(2, "receive_bip: non-B/IP packet - discarded.\n");
    // cleanup
  free_packet:
    npdu_free(npdu);
    free(mtu);

    return;
}
