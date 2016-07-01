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

#ifndef INCLUDED_AMBE3000_DSTAR_DECODE_BS_IMPL_H
#define INCLUDED_AMBE3000_DSTAR_DECODE_BS_IMPL_H

#include <ambe3000/dstar_decode_bs.h>
#include "DStarDecode.h"

namespace gr {
  namespace ambe3000 {

    class dstar_decode_bs_impl : public dstar_decode_bs
    {
     private:
      CDStarDecode dstarDecode;
      bool device_is_closed;

     public:
      dstar_decode_bs_impl(char *device, int baudrate);
      ~dstar_decode_bs_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace ambe3000
} // namespace gr

#endif /* INCLUDED_AMBE3000_DSTAR_DECODE_BS_IMPL_H */

