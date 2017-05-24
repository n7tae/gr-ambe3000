#pragma once

/*
 *   Copyright 2016 by Thomas Early N7TAE
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

class CDStarHeader {
public:
	//CDStarHeader();
	CDStarHeader(unsigned char *in);
	~CDStarHeader();
	const char *GetHeader();
private:
	unsigned char header[41];
	void scramble(int *h1, int *h2);
	void deinterleave(int *h1, int *h2);
	void fec_decoder(int *h1, int *h2);
	void traceBack(int *out, int *m_pathMemory0, int *m_pathMemory1, int *m_pathMemory2, int *m_pathMemory3);
	void viterbiDecode (int n, int *data, int *m_pathMemory0, int *m_pathMemory1, int *m_pathMemory2, int *m_pathMemory3, int *m_pathMetric);
};

