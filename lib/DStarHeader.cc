/*
 *   Copyright 2016 by Thomas Early AC2IE
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Adapted from gr-dsd by Clayton Smith <argilo@gmail.com>
 *   See https://github.com/argilo
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdio>

#include "DStarHeader.h"

CDStarHeader::CDStarHeader(unsigned char *in)
{
	int h1[660];
	int h2[660];
	for (int i=0; i<660; i++)
		h1[i] = int(in[i]);
	scramble(h1, h2);
	deinterleave(h2, h1);
	fec_decoder(h1, h2);
	
	memset(header, 0, 41);
	// note we receive 330 bits, but we only use 328 of them (41 octets)
	// bits 329 and 330 are unused
	unsigned char bit2octet[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	int octetcount = 0;
	int bitcount = 0;
	for (int loop = 0; loop < 328; loop++) {
		if (h2[loop]) {
			header[octetcount] |= bit2octet[bitcount];
		};
		bitcount++;
		// increase octetcounter and reset bitcounter every 8 bits
		if (bitcount >= 8) {
			octetcount++;
			bitcount = 0;
		}
	}
	return;
}

CDStarHeader::~CDStarHeader()
{
}

void CDStarHeader::traceBack(int *out, int *m_pathMemory0, int *m_pathMemory1, int *m_pathMemory2, int *m_pathMemory3)
{
	enum FEC_STATE { S0, S1, S2, S3 } state;
	int loop;
	//int length=0;

	state = S0;

	for (loop=329; loop >= 0; loop--/*, length++*/) {

		switch (state) {
			case S0: // if state S0
				if (m_pathMemory0[loop]) {
					state = S2; // lower path
				} else {
					state = S0; // upper path
				};
				out[loop]=0;
				break;
			
			case S1: // if state S1
				if (m_pathMemory1[loop]) {
					state = S2; // lower path
				} else {
					state = S0; // upper path
				};
				out[loop]=1;
				break;
			
			case S2: // if state S2
				if (m_pathMemory2[loop]) {
					state = S3; // lower path
				} else {
					state = S1; // upper path
				};
				out[loop]=0;
				break;
			
			case S3: // if state S3
				if (m_pathMemory3[loop]) {
					state = S3; // lower path
				} else {
					state = S1; // upper path
				};
				out[loop]=1;
				break;
			
		}
	}
	return;
}

void CDStarHeader::viterbiDecode (int n, int *data, int *m_pathMemory0, int *m_pathMemory1, int *m_pathMemory2, int *m_pathMemory3, int *m_pathMetric)
{
	int tempMetric[4];
	int metric[8];

	metric[0] = (data[1]^0) + (data[0]^0);
	metric[1] = (data[1]^1) + (data[0]^1);
	metric[2] = (data[1]^1) + (data[0]^0);
	metric[3] = (data[1]^0) + (data[0]^1);
	metric[4] = (data[1]^1) + (data[0]^1);
	metric[5] = (data[1]^0) + (data[0]^0);
	metric[6] = (data[1]^0) + (data[0]^1);
	metric[7] = (data[1]^1) + (data[0]^0);

	// Pres. state = S0, Prev. state = S0 & S2
	int m1 = metric[0] + m_pathMetric[0];
	int m2 = metric[4] + m_pathMetric[2];
	if (m1 < m2) {
		m_pathMemory0[n] = 0;
		tempMetric[0] = m1;
	} else {
		m_pathMemory0[n] = 1;
		tempMetric[0] = m2;
	}

	// Pres. state = S1, Prev. state = S0 & S2
	m1 = metric[1] + m_pathMetric[0];
	m2 = metric[5] + m_pathMetric[2];
	if (m1 < m2) {
		m_pathMemory1[n] = 0;
		tempMetric[1] = m1;
	} else {
		m_pathMemory1[n] = 1;
		tempMetric[1] = m2;
	}

	// Pres. state = S2, Prev. state = S2 & S3
	m1 = metric[2] + m_pathMetric[1];
	m2 = metric[6] + m_pathMetric[3];
	if (m1 < m2) {
		m_pathMemory2[n] = 0;
		tempMetric[2] = m1;
	} else {
		m_pathMemory2[n] = 1;
		tempMetric[2] = m2;
	}

	// Pres. state = S3, Prev. state = S1 & S3
	m1 = metric[3] + m_pathMetric[1];
	m2 = metric[7] + m_pathMetric[3];
	if (m1 < m2) {
		m_pathMemory3[n] = 0;
		tempMetric[3] = m1;
	} else {
		m_pathMemory3[n] = 1;
		tempMetric[3] = m2;
	}

	for (int loop=0; loop<4; loop++) {
		m_pathMetric[loop] = tempMetric[loop];
	}
	return;
};

void CDStarHeader::fec_decoder(int *in, int *out)
{
	int m_pathMemory0[330];
	int m_pathMemory1[330];
	int m_pathMemory2[330];
	int m_pathMemory3[330];
	memset(m_pathMemory0, 0, 330 * sizeof(int));
	memset(m_pathMemory1, 0, 330 * sizeof(int));
	memset(m_pathMemory2, 0, 330 * sizeof(int));
	memset(m_pathMemory3, 0, 330 * sizeof(int));
	int m_pathMetric[4];


	for (int loop=0; loop<4; loop++) {
		m_pathMetric[loop]=0;
	}; // end for


	int n=0;
	for (int loop2=0; loop2<660; loop2+=2, n++) {
		int data[2];

		if (in[loop2]) {
			data[1] = 1;
		} else {
			data[1] = 0;
		}

		if (in[loop2 + 1]) {
			data[0] = 1;
		} else {
			data[0] = 0;
		}

		viterbiDecode(n, data, m_pathMemory0, m_pathMemory1, m_pathMemory2, m_pathMemory3, m_pathMetric);
	}

	traceBack(out, m_pathMemory0, m_pathMemory1, m_pathMemory2, m_pathMemory3);

	return;
}

// function deinterleave
void CDStarHeader::deinterleave(int *in, int *out)
{
	// init vars
	int k=0;
	for (int loop=0; loop<660; loop++) {
		out[k]=in[loop];

		k += 24;

		if (k >= 672) {
			k -= 671;
		} else if (k >= 660) {
			k -= 647;
		}
	}
	return;
}

void CDStarHeader::scramble (int *in, int *out)
{
	static const int SCRAMBLER_TABLE_BITS[] = {
		0,0,0,0,1,1,1,0,1,1,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,0,
		0,0,1,0,0,1,1,0,0,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,0,0,0,0,1,1,0,0,
		1,1,0,1,0,1,0,0,1,1,1,0,0,1,1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,0,1,0,
		1,1,1,1,1,0,1,0,0,1,0,1,0,0,0,1,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,0,
		0,0,0,1,1,1,0,1,1,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,
		0,1,0,0,1,1,0,0,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,0,0,0,0,1,1,0,0,1,
		1,0,1,0,1,0,0,1,1,1,0,0,1,1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,0,1,0,1,
		1,1,1,1,0,1,0,0,1,0,1,0,0,0,1,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,
		0,0,1,1,1,0,1,1,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,
		1,0,0,1,1,0,0,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,0,0,0,0,1,1,0,0,1,1,
		0,1,0,1,0,0,1,1,1,0,0,1,1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,0,1,0,1,1,
		1,1,1,0,1,0,0,1,0,1,0,0,0,1,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,
		0,1,1,1,0,1,1,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,
		0,0,1,1,0,0,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,0,0,0,0,1,1,0,0,1,1,0,
		1,0,1,0,0,1,1,1,0,0,1,1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,0,1,0,1,1,1,
		1,1,0,1,0,0,1,0,1,0,0,0,1,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,
		1,1,1,0,1,1,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,
		0,1,1,0,0,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,0,0,0,0,1,1,0,0,1,1,0,1,
		0,1,0,0,1,1,1,0,0,1,1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,0,1,0,1,1,1,1,
		1,0,1,0,0,1,0,1,0,0,0,1,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,
		1,1,0,1,1,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,
		1,1,0,0,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,0,0,0,0,1,1,0,0,1,1,0,1,0,
		1,0,0,1,1,1,0,0,1,1,1,1,0,1,1,0
	};
		
	const int SCRAMBLER_TABLE_BITS_LENGTH=720;

	int m_count = 0;
	for (int loop=0; loop < 660; loop++) {
		out[loop] = in[loop] ^ SCRAMBLER_TABLE_BITS[m_count++];

		if (m_count >= SCRAMBLER_TABLE_BITS_LENGTH) {
			m_count = 0;
		}
	}
	return;
}

const char *CDStarHeader::GetHeader()
{
	return (const char *)&header[0];
}

