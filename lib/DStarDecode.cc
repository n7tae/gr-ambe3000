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

static const unsigned char head_sync[] = {	// 64 + 15 = 79 bits
	1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,
	1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,
	1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,
	1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,
	  1,1,1, 0,1,1,0, 0,1,0,1, 0,0,0,0
};
static const unsigned char data_sync[] = {	// 24 bits
	1,0,1,0, 1,0,1,0, 1,0,1,1, 0,1,0,0, 0,1,1,0, 1,0,0,0
};
const unsigned char term_sync[] = {	// 48 bits
	1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 
	1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0, 
	0,0,0,1, 0,0,1,1, 0,1,0,1, 1,1,1,0
};
static const unsigned int headsize = 79;
static const unsigned int datasize = 24;
static const unsigned int termsize = 48;


CDStarDecode::CDStarDecode() :
	index(0),
	headflag(0),
	dataflag(0),
	termflag(0),
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
				printf("FLAG1: %02X - FLAG2: %02X - FLAG3: %02X\n", h[0], h[1], h[2]);
				printf("RPT 2: %c%c%c%c%c%c%c%c ", h[3], h[4], h[5], h[6], h[7], h[8], h[9], h[10]);
				printf("RPT 1: %c%c%c%c%c%c%c%c ", h[11], h[12], h[13], h[14], h[15], h[16], h[17], h[18]);
				printf("YOUR: %c%c%c%c%c%c%c%c ", h[19], h[20], h[21], h[22], h[23], h[24], h[25], h[26]);
				printf("MY: %c%c%c%c%c%c%c%c/%c%c%c%c\n", h[27], h[28], h[29], h[30], h[31], h[32], h[33], h[34], h[35], h[36], h[37], h[38]);
				voiceframecount = headflag = dataflag = termflag = index = 0;
				readmode = datamode;
			}
			continue;
		}

		if (datamode == readmode) {
			int i = index++ / 8;
			buffer[i] = (buffer[i]<<1) | bit;
			if (index >= 96) {
				// voice frame is complete
				if ((buffer[9]==0xaa) && (buffer[10]==0xb4) && (buffer[11]==0x68)) {	// is it a sync frame?
					if (0 != voiceframecount++%21) {
						printf("Voice frame %d is an UNEXPECTED sync frame\n", voiceframecount);
						readmode = nullmode;
					}
				} else {
					if (0 == voiceframecount++%21) {
						printf("Voice frame %d SHOULD have been a sync frame\n", voiceframecount);
						readmode = nullmode;
					}
				}
				if (dv3000u.DecodeData(buffer, out))
					return true;
				decoded = true;
			}
		}

		// search for sync patterns
		if (bit == head_sync[headflag]) {
			headflag++;
			if (headsize == headflag) {
				termflag = dataflag = headflag = voiceframecount = index = 0;
				readmode = headmode;
				continue;
			}
		} else
			headflag = 0;

		// if we see a datasync here, we'll just go to readmode and ignore this first voice frame.
		// TODO: use a ring buffer to store the last 96 bits so we can send this first frame to get decoded
		if (bit == data_sync[dataflag]) {
			dataflag++;
			if (datasize == dataflag) {
				readmode = datamode;
				dataflag = termflag = headflag = index = 0;
				voiceframecount = 1;
			}
		} else
			dataflag = 0;

		if (bit == term_sync[termflag])
			termflag++;
			if (termsize == termflag) {
				termflag = dataflag = headflag = index = voiceframecount = 0;
				readmode = nullmode;
			}
		else
			termflag = 0;
			
	}

	if (! decoded) {
		for (int i=0; i<160; i++)
			out[i] = (short int)0;
	}

	return false;
}
