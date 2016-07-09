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
#include <arpa/inet.h>
#include "SyncRegister.h"

CSyncRegister::CSyncRegister()
{
	data[0] = data[1] = data[2] = 0u;
}

CSyncRegister::~CSyncRegister()
{
}

void CSyncRegister::Push(const unsigned char in)
{
	data[2] = data[2] << 1;
	if (0x80000000u & data[1])
		data[2] |= 1;
	data[1] = data[1] << 1;
	if (0x80000000u & data[0])
		data[1] |= 1;
	data[0] = data[0] << 1;
	data[0] |= 0x01u & in;
}

bool CSyncRegister::IsHeadSync()
{
	return data[2]==0x55555555u && data[1]==0x55555555u && data[0]==0x55557650u;
}

bool CSyncRegister::IsDataSync()
{
	return (data[0] & 0xffffffu) == 0xaab468u;
}

bool CSyncRegister::IsTermSync()
{
	return (data[1] & 0xffffu)==0xaaaau && data[0]==0xaaaa135eu;
}

void CSyncRegister::GetDataFrame(unsigned char *frame)
{
	union {
		unsigned int i;
		unsigned char c[4];
	} u;
	u.i = htonl(data[0]);
	for (int i=11; i>=8; i--)
		frame[i] = u.c[i-8];
	u.i = htonl(data[1]);
	for (int i=7; i>=4; i--)
		frame[i] = u.c[i-4];
	u.i = htonl(data[2]);
	for (int i=3; i>=0; i--)
		frame[i] = u.c[i];
	return;
}

