/* -*- c++ -*- */
/* 
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
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

    dstar_decode_bs::sptr
    dstar_decode_bs::make(char *device, int baudrate)
    {
	
      return gnuradio::get_initial_sptr
        (new dstar_decode_bs_impl(device, baudrate));
    }

    /*
     * The private constructor
     */
    dstar_decode_bs_impl::dstar_decode_bs_impl(char *device, int baudrate)
      : gr::block("dstar_decode_bs",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(short)))
    {
		device_is_closed = dv3000u.OpenDevice(device, baudrate);
		set_output_multiple(160);
	}

    /*
     * Our virtual destructor.
     */
    dstar_decode_bs_impl::~dstar_decode_bs_impl()
    {
		dv3000u.CloseDevice();
    }

    void
    dstar_decode_bs_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = 12 * noutput_items / 160;
    }

    int
    dstar_decode_bs_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      short *out = (short *) output_items[0];

	  if (device_is_closed)
		return WORK_DONE;
      // Do <+signal processing+>
      for (int i=0; i<noutput_items; i+=160) {
		  if (dv3000u.DecodeData(in, out))
			return WORK_DONE;
		  in += 12;
		  out += 160;
	  }
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (12 * noutput_items / 160);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace ambe3000 */
} /* namespace gr */

