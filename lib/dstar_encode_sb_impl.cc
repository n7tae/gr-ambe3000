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
#include "dstar_encode_sb_impl.h"

namespace gr {
  namespace ambe3000 {

    dstar_encode_sb::sptr
    dstar_encode_sb::make(char *device, int baudrate)
    {
      return gnuradio::get_initial_sptr
        (new dstar_encode_sb_impl(device, baudrate));
    }

    /*
     * The private constructor
     */
    dstar_encode_sb_impl::dstar_encode_sb_impl(char *device, int baudrate)
      : gr::block("dstar_encode_sb",
              gr::io_signature::make(1, 1, sizeof(short int)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)))
    {
		framecount = 0;
		device_is_closed = dv3000u.OpenDevice(device, baudrate);
		set_output_multiple(12);
	}

    /*
     * Our virtual destructor.
     */
    dstar_encode_sb_impl::~dstar_encode_sb_impl()
    {
		dv3000u.CloseDevice();
    }

    void
    dstar_encode_sb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = 160 * noutput_items / 12;
    }

    int
    dstar_encode_sb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const short int *in = (const short int *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      if (device_is_closed)
		return WORK_DONE;	// DV3000 didn't open

      // Do <+signal processing+>
      for (int i=0; i<noutput_items; i+=12) {
        if (dv3000u.EncodeAudio(in, out, framecount))
			return WORK_DONE;	// EncodeAudio had a problem
        in += 160;
        out += 12;
        framecount++;
	  }
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (160*noutput_items/12);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace ambe3000 */
} /* namespace gr */

