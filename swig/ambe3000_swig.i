/* -*- c++ -*- */

#define AMBE3000_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ambe3000_swig_doc.i"

%{
#include "ambe3000/dstar_encode_sb.h"
#include "ambe3000/dstar_decode_bs.h"
#include "ambe3000/bit_reverse_bb.h"
%}

%include "ambe3000/dstar_encode_sb.h"
GR_SWIG_BLOCK_MAGIC2(ambe3000, dstar_encode_sb);
%include "ambe3000/dstar_decode_bs.h"
GR_SWIG_BLOCK_MAGIC2(ambe3000, dstar_decode_bs);
%include "ambe3000/bit_reverse_bb.h"
GR_SWIG_BLOCK_MAGIC2(ambe3000, bit_reverse_bb);
