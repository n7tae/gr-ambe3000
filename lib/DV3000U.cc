/*
 *   Copyright (C) 2014 by Jonathan Naylor G4KLX and John Hays K7VE
 *   Copyright 2016 by Jeremy McDermond (NH6Z)
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
 *   Adapted by K7VE from G4KLX dv3000d
 */

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cerrno>

#include <netinet/in.h>

#include "DV3000U.h"

CDV3000U::CDV3000U()
{
}

CDV3000U::~CDV3000U()
{
}

bool CDV3000U::checkResponse(PDV3K_PACKET p, unsigned char response)
{
	if(p->start_byte != DV3K_START_BYTE || p->header.packet_type != DV3K_TYPE_CONTROL || p->field_id != response)
		return true;

	return false;
}

bool CDV3000U::OpenDevice(char *ttyname, int baud)
{
	struct termios tty;
	
	fd = open(ttyname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		std::cerr << "AMBEserver: error when opening " << ttyname << ": " << strerror(errno) << std::endl;
		return true;
	}

	if (tcgetattr(fd, &tty) != 0) {
		std::cerr << "AMBEserver tcgetattr: " << strerror(errno) << std::endl;
		close(fd);
		return true;
	}
	
	//  Input speed = output speed
	cfsetispeed(&tty, B0);
	
	switch(baud) {
		case 230400:
			cfsetospeed(&tty, B230400);
			break;
		case 460800:
			cfsetospeed(&tty, B460800);
			break;
		default:
			std::cerr << "AMBEserver: unsupported baud rate " << baud << std::endl;
			close(fd);
			return true;
	}
	
	tty.c_lflag    &= ~(ECHO | ECHOE | ICANON | IEXTEN | ISIG);
	tty.c_iflag    &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON | IXOFF | IXANY);
	tty.c_cflag    &= ~(CSIZE | CSTOPB | PARENB);
	tty.c_cflag    |= CS8 | CRTSCTS;
	tty.c_oflag    &= ~(OPOST);
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 1;
	
	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		std::cerr << "AMBEserver: tcsetattr: " << strerror(errno) << std::endl;
		close(fd);
		return true;
	}

	if (initDV3K()) {
		close(fd);
		return true;
	}

	return false; 
}

bool CDV3000U::initDV3K()
{
	char prodId[17];
	char version[49];
	DV3K_PACKET responsePacket, ctrlPacket;

    ctrlPacket.start_byte = DV3K_START_BYTE;
    ctrlPacket.header.packet_type = DV3K_TYPE_CONTROL;
    ctrlPacket.header.payload_length = htons(1);
    ctrlPacket.field_id = DV3K_CONTROL_RESET;

	if (write(fd, &ctrlPacket, dv3k_packet_size(ctrlPacket)) == -1) {
		std::cerr << "initDV3k: error writing reset packet: " << strerror(errno) << std::endl;
		return true;
	}
	
	if (getresponse(&responsePacket)) {
		std::cerr << "initDV3k: error receiving response to reset" << std::endl;
		return true;
	}
	
	if (checkResponse(&responsePacket, DV3K_CONTROL_READY)) {
	   std::cerr << "initDV3k: invalid response to reset" << std::endl;
	   return true;
	}
	
	ctrlPacket.field_id = DV3K_CONTROL_PRODID;
	if (write(fd, &ctrlPacket, dv3k_packet_size(ctrlPacket)) == -1) {
		std::cerr << "initDV3k: error writing product id packet: " << strerror(errno) << std::endl;
		return true;
	}
	
	if (getresponse(&responsePacket)) {
		std::cerr << "initDV3k: error receiving response to product id request" << std::endl;
		return true;
	}
	
	if (checkResponse(&responsePacket, DV3K_CONTROL_PRODID)) {
	   std::cerr << "initDV3k: invalid response to product id query" << std::endl;
	   return true;
	}
	strncpy(prodId, responsePacket.payload.ctrl.data.prodid, sizeof(prodId));
	
	ctrlPacket.field_id = DV3K_CONTROL_VERSTRING;
	if (write(fd, &ctrlPacket, dv3k_packet_size(ctrlPacket)) == -1) {
		std::cerr << "initDV3k: error writing version packet: " << strerror(errno) << std::endl;
		return true;
	}
	
	if (getresponse(&responsePacket)) {
		std::cerr << "initDV3k: error receiving response to version request" << std::endl;
		return true;
	}
	
	if (checkResponse(&responsePacket, DV3K_CONTROL_VERSTRING)) {
	   std::cerr << "initDV3k: invalid response to version query" << std::endl;
	   return true;
	}
	strncpy(version, responsePacket.payload.ctrl.data.version, sizeof(version));
	
	std::cout << "Initialized " << prodId << " version " << version << std::endl;

	ctrlPacket.header.payload_length = htons(13);
	ctrlPacket.field_id = DV3K_CONTROL_RATEP;
	unsigned char DSTAR_RATE_P[] = { 0x01U, 0x30U, 0x07U, 0x63U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x48U };
	memcpy(ctrlPacket.payload.ctrl.data.ratep, DSTAR_RATE_P, 12);
	if (-1 == write(fd, &ctrlPacket, dv3k_packet_size(ctrlPacket))) {
		std::cerr << "initDV3K: error writing DStar control packet" << std::endl;
		return true;
	}

	if (getresponse(&responsePacket)) {
		std::cerr << "initDV3K: error receiving response to DStar control packet" << std::endl;
		return true;
	}

	if (checkResponse(&responsePacket, DV3K_CONTROL_RATEP)) {
		std::cerr << "intiDV3K: invalid response to DStar RATE_P control packet" << std::endl;
		return true;
	}

	return false;
}

void CDV3000U::CloseDevice()
{
	close(fd);
}

bool CDV3000U::getresponse(PDV3K_PACKET packet)
{
	ssize_t bytesRead;

	// get the start byte
	packet->start_byte = 0U;
	for (unsigned i = 0U; i < sizeof(DV3K_PACKET); ++i) {
		bytesRead = read(fd, packet, 1);
		if (bytesRead == -1) {
			std::cerr << "CDV3000U: Error reading from serial port: " << strerror(errno) << std::endl;
			return true;
		}
		if (packet->start_byte == DV3K_START_BYTE)
			break;
	}
	if (packet->start_byte != DV3K_START_BYTE) {
		std::cerr << "CDV3000U: Couldn't find start byte in serial data" << std::endl;
		return true;
	}

	// get the packet size and type (three bytes)
	ssize_t bytesLeft = sizeof(packet->header);
	ssize_t total = bytesLeft;
	while (bytesLeft > 0) {
		bytesRead = read(fd, ((uint8_t *) &packet->header) + total - bytesLeft, bytesLeft);
		if(bytesRead == -1) {
			std::cout << "AMBEserver: Couldn't read serial data header" << std::endl;
			return true;
		}
		bytesLeft -= bytesRead;
	}
	
	total = bytesLeft = ntohs(packet->header.payload_length);
    if (bytesLeft > 1 + int(sizeof(packet->payload))) {
        std::cout << "AMBEserver: Serial payload exceeds buffer size: " << int(bytesLeft) << std::endl;
        return true;
    }
    
    while (bytesLeft > 0) {
        bytesRead = read(fd, ((uint8_t *) &packet->field_id) + total - bytesLeft, bytesLeft);
        if (bytesRead == -1) {
            std::cerr << "AMBEserver: Couldn't read payload: " << strerror(errno) << std::endl;
            return true;
        }
        
        bytesLeft -= bytesRead;
    }
    
    return false;
}

bool CDV3000U::EncodeAudio(const short *audio, unsigned char *data, int framecount)
{
	// Create Audio packet based on input floats
	DV3K_PACKET p;
	p.start_byte = DV3K_START_BYTE;
	p.header.payload_length = htons(322);
	p.header.packet_type = DV3K_TYPE_AUDIO;
	p.field_id = 0U;
	p.payload.audio.num_samples = 160U;
	for (int i=0; i<160; i++)
		p.payload.audio.samples[i] = htons(audio[i]);

	// send audio packet to DV3000
	int size = dv3k_packet_size(p);
	if (write(fd, &p, size) != size) {
		std::cerr << "EncodeAudio: error sending audio packet" << std::endl;
		return true;
	}

	// read data packet from DV3000
	p.start_byte = 0U;
	if (getresponse(&p))
		return true;
	if (p.start_byte!=DV3K_START_BYTE || htons(p.header.payload_length)!=11 ||
			p.header.packet_type!=DV3K_TYPE_AMBE || p.field_id!=1U ||
			p.payload.ambe.num_bits!=72U) {
		std::cerr << "EncodeAudio: unexpected audio packet response" << std::endl;
		return true;
	}

	// copy it to the output
	memcpy(data, p.payload.ambe.data, 9);
	
	// add the 3 bytes of data at the end of the voice frame for the "slow data"
	data += 9;
	if (framecount % 21) {
		// null data {0x66, 0x66, ...} after it has been processed
		unsigned char nulldata[] = {0x16u, 0x29u, 0xf5u};
		memcpy (data, nulldata, 3);
	} else {
		// every 21 frames (starting with the first frame) we need a sync
		unsigned char syncdata[] = {0x55u, 0x2du, 0x16u};
		memcpy (data, syncdata, 3);
	}
	// in both cased these bytes are bit-reversed because the GMSK modulator
	// starts with the least significant bit of the incoming byte
	// and so the GMSK modulator need to be fed bit-revered bytes
	
	return false;
}

bool CDV3000U::DecodeData(const unsigned char *data, short *audio)
{
	// Create data packet
	DV3K_PACKET p;
	p.start_byte = DV3K_START_BYTE;
	p.header.payload_length = htons(11);
	p.header.packet_type = DV3K_TYPE_AMBE;
	p.field_id = 1U;
	p.payload.ambe.num_bits = 72U;
	memcpy(p.payload.ambe.data, data, 9);

	// send data packet to DV3000
	int size = dv3k_packet_size(p);
	if (write(fd, &p, size) != size) {
		std::cerr << "DecodeData: error sending data packet" << std::endl;
		return true;
	}

	// read audio packet from DV3000
	p.start_byte = 0U;
	if (getresponse(&p))
		return true;
	if (p.start_byte!=DV3K_START_BYTE || htons(p.header.payload_length)!=322 ||
			p.header.packet_type!=DV3K_TYPE_AUDIO || p.field_id!=0U ||
			p.payload.audio.num_samples!=160U) {
		std::cerr << "EncodeAudio: unexpected audio packet response" << std::endl;
		return true;
	}
	
	for (int i=0; i<160; i++)
		audio[i] = ntohs(p.payload.audio.samples[i]);

	return false;
}

void CDV3000U::dump(const char *title, void *pointer, int length)
{
	assert(title != NULL);
	assert(pointer != NULL);

	const unsigned char *data = (const unsigned char *)pointer;

	std::cout << title << std::endl;

	unsigned int offset = 0U;

	while (length > 0) {

		unsigned int bytes = (length > 16) ? 16U : length;

		for (unsigned i = 0U; i < bytes; i++) {
			if (i)
				std::cout << " ";
			std::cout << std::hex << std::setw(2) << std::right << std::setfill('0') << int(data[offset + i]);
		}

		for (unsigned int i = bytes; i < 16U; i++)
			std::cout << "   ";

		std::cout << "   *";

		for (unsigned i = 0U; i < bytes; i++) {
			unsigned char c = data[offset + i];

			if (::isprint(c))
				std::cout << c;
			else
				std::cout << '.';
		}

		std::cout << '*' << std::endl;

		offset += 16U;

		if (length >= 16)
			length -= 16;
		else
			length = 0;
	}
}
