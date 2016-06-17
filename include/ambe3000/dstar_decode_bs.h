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


#ifndef INCLUDED_AMBE3000_DSTAR_DECODE_BS_H
#define INCLUDED_AMBE3000_DSTAR_DECODE_BS_H

#include <ambe3000/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ambe3000 {

    /*!
     * \brief This block decodes a DStar voice stream to an 8000
     * sample/second 16-bit (short int) audio stream. All slow data
     * (the trailing 24 bits) in each voice frame is currently ignored.
     * \ingroup ambe3000
     *
     */
    class AMBE3000_API dstar_decode_bs : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dstar_decode_bs> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ambe3000::dstar_decode_bs.
       *
       * To avoid accidental use of raw pointers, ambe3000::dstar_decode_bs's
       * constructor is in a private implementation
       * class. ambe3000::dstar_decode_bs::make is the public interface for
       * creating new instances.
       */
      static sptr make(char *device, int baudrate);
    };

  } // namespace ambe3000
} // namespace gr

#endif /* INCLUDED_AMBE3000_DSTAR_DECODE_BS_H */

