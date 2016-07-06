/* -*- c++ -*- */
/* 
 * Copyright 2016 Thomas Early AC2IE.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <fcntl.h>

#include "DStarDecode.h"
#include "DStarHeader.h"

CDStarDecode::CDStarDecode() :
	index(0),
	voiceframecount(0),
	readmode(nullmode)
{
}

CDStarDecode::~CDStarDecode()
{
	dv3000u.CloseDevice();
}

bool CDStarDecode::OpenDevice(char *device, int baudrate)
{
	return dv3000u.OpenDevice(device, baudrate);
}

void CDStarDecode::CloseDevice()
{
	dv3000u.CloseDevice();
	return;
}

bool CDStarDecode::Process(const unsigned char *in, short int *out)
{
	bool decoded = false;
	for (int inp=0; inp<96; inp++) {
		unsigned char bit = in[inp];
		if (headmode == readmode) {
			buffer[index++] = bit;
			if (index >= 660) {
				CDStarHeader dsh(buffer);
				// print the header
				const char *h = dsh.GetHeader();
				printf("DSTAR HEADER: ");
				//printf("FLAG1: %02X - FLAG2: %02X - FLAG3: %02X\n", h[0], h[1], h[2]);
				printf("RPT2: %c%c%c%c%c%c%c%c ", h[3], h[4], h[5], h[6], h[7], h[8], h[9], h[10]);
				printf("RPT1: %c%c%c%c%c%c%c%c ", h[11], h[12], h[13], h[14], h[15], h[16], h[17], h[18]);
				printf("YOUR: %c%c%c%c%c%c%c%c ", h[19], h[20], h[21], h[22], h[23], h[24], h[25], h[26]);
				printf("MY: %c%c%c%c%c%c%c%c/%c%c%c%c\n", h[27], h[28], h[29], h[30], h[31], h[32], h[33], h[34], h[35], h[36], h[37], h[38]);
				voiceframecount = index = 0;
				readmode = datamode;
			}
			continue;
		}
		sr.Push(bit);
		if (datamode == readmode) {
			if (++index >= 96) {
				sr.GetDataFrame(buffer);
				//printf("dataframe[%i]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", voiceframecount,
				//	buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11]);
				// voice frame is complete
				if (sr.IsDataSync()) {	// is it a sync frame?
					if (0 != voiceframecount++%21) {
						printf("Voice frame %d is an unexpected sync frame\n", voiceframecount);
						readmode = nullmode;
					}
				} else {
					if (0 == voiceframecount++%21) {
						printf("Voice frame %d should have been a sync frame\n", voiceframecount);
						printf("Data bytes: 0x%02x 0x%02x 0x%02x.\n", buffer[9], buffer[10], buffer[11]);
						readmode = nullmode;
					}
				}
				if (dv3000u.DecodeData(buffer, out))
					return true;
				decoded = true;
				index = 0;
			}
		}

		// search for sync patterns
		if (nullmode == readmode) {
			if (sr.IsHeadSync()) {
				printf("Got headsync flag!\n");
				voiceframecount = index = 0;
				readmode = headmode;
				continue;
			}
		}

		// A voiceframe is suppose to immediately follow the 660-bit header.
		// This is here to put us back into voice mode if we missed the header or dropped a datasync flag
		if (datamode != readmode) {
			if (sr.IsDataSync()) {
				printf("Got datasync flag!\n");
				readmode = datamode;
				index = 0;
				sr.GetDataFrame(buffer);
				//printf("dataframe[0]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
				//	buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11]);
				if (dv3000u.DecodeData(buffer, out))
					return true;
				decoded = true;
				voiceframecount = 1;
			}
		}
		
		if (sr.IsTermSync()) {
			printf("Got termsync flag!\n");
			index = voiceframecount = 0;
			readmode = nullmode;
		}
	}

	if (! decoded) {
		for (int i=0; i<160; i++)
			out[i] = (short int)0;
	}

	return false;
}
