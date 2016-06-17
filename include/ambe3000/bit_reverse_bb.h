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


#ifndef INCLUDED_AMBE3000_BIT_REVERSE_BB_H
#define INCLUDED_AMBE3000_BIT_REVERSE_BB_H

#include <ambe3000/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace ambe3000 {

    /*!
     * \brief Performs a bit reversal of the AMBE3000 data stream.
     * This is needed because the GMSK modulation block start
     * from the least significant bit.
     * \ingroup ambe3000
     *
     */
    class AMBE3000_API bit_reverse_bb : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<bit_reverse_bb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ambe3000::bit_reverse_bb.
       *
       * To avoid accidental use of raw pointers, ambe3000::bit_reverse_bb's
       * constructor is in a private implementation
       * class. ambe3000::bit_reverse_bb::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace ambe3000
} // namespace gr

#endif /* INCLUDED_AMBE3000_BIT_REVERSE_BB_H */

