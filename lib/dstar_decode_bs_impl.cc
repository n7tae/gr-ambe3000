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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dstar_decode_bs_impl.h"

namespace gr {
	namespace ambe3000 {

		dstar_decode_bs::sptr dstar_decode_bs::make(char *device, int baudrate)
		{
			return gnuradio::get_initial_sptr (new dstar_decode_bs_impl(device, baudrate));
		}

		// The private constructor
		dstar_decode_bs_impl::dstar_decode_bs_impl(char *device, int baudrate)
			: gr::block("dstar_decode_bs",
				gr::io_signature::make(1, 1, sizeof(unsigned char)),
				gr::io_signature::make(1, 1, sizeof(short)))
		{
			device_is_closed = dstarDecode.OpenDevice(device, baudrate);
			set_output_multiple(160);
		}

		// Our virtual destructor.
		dstar_decode_bs_impl::~dstar_decode_bs_impl()
		{
			dstarDecode.CloseDevice();
		}

		void dstar_decode_bs_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
		{
			ninput_items_required[0] = 96 * noutput_items / 160;
		}

		int dstar_decode_bs_impl::general_work (int noutput_items, gr_vector_int &ninput_items,
			gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
		{
			const unsigned char *in = (const unsigned char *) input_items[0];
			short *out = (short *) output_items[0];

			if (device_is_closed)
				return WORK_DONE;
			// Do <+signal processing+>
			int count = 0;
			for (int i=0; i<noutput_items; i+=160) {
				int thistime = 160;
				if (dstarDecode.Process(in, out, thistime))
					return WORK_DONE;
				count += thistime;
				in += 96;
				out += thistime;
			}
			// Tell runtime system how many input items we consumed on
			// each input stream.
			consume_each(96 * noutput_items / 160);

			// Tell runtime system how many output items we produced.
			return count;
		}

	} /* namespace ambe3000 */
} /* namespace gr */

