title: The AMBE3000 OOT Module
brief: Uses the NW Digital Radio AMBE USB Dongle to encode/decode audio
tags:
  - sdr
  - DStar
  - AMBE3000
author:
  - Thomas Early <ac2ie (at) arrl (dot) net>
copyright_owner:
  - Thomas A. Early
#license:
repo: http://github.com/ac2ie/gr-ambe3000
website: http://nwdigitalradio.com
---
Uses the Northwest Digital Radio USB AMBE3000 dongle to encode and decode between audio and DStar data. The encoder builds a DStar data stream from an 8000 sample/second, 16-bit integer audio stream. The decoder converts a DStar data stream to a 8000 sample/second, 16-bit integer audio stream.

Three example flowgraphs are include (1) encoding an audio source to DStar stream captured by a file, (2) reading a DStar stream from a file and decoding it to audio, and (3) writing the dstar file to a UHD sink (transmitting!).

If you have a different TX-ing device, you'll need to use it instead of the UHD. If your device doesn't support a 480 kHz input rate, you'll need to fiddle with the parameters in this flow graph. To transmit RF, you will also need an amateur radio license.

The DStar stream uses 12 byte frames with 20 milliseconds of audio encoded in the first 9 bytes (72 bits) of the frame. Currently he remaining 3 bytes have no useful content, except that every 21st frame is a sync flag.

The data stream has no DStar header and no terminating flag, so if you try the transmitter flowgraph, you'll see no "slow data" text on your DStar receiver and your recevier will beep at the end of the transmission becasuse the data streem is not properly terminated.

You can't have both the encoder and the decoder in a single flowgraph, so make a transmitter or a receiver, but not a transceiver. The gnuradio GMSK modulators start from the least significant byte, so you'll need to use the included bit reverser before the modulator for any transmission that is going to be received by a commercial receiver. See the flowgraphs in the examples folder.

**Install guide**  

Change to any folder in your home directory and enter following commands in your terminal.

`git clone https://github.com/ac2ie/gr-ambe3000.git` // clone this repository  
`cd gr-ambe3000/`  
`mkdir build` // make build folder  
`cd build/`  
`cmake ../` // build makefiles (possible include -DCMAKE_INSTALL_PREFIX=<path_to_gnuradio>, see the gnuradio WIKI)
`make` // build toolbox (There may be a few warngings, mostly about doxygen, but no errors.)
`sudo make install` // to make the ambe3000 blocks available to gnuradio-companion.

And if you don't want the ambe3000 blocks anymore...

`sudo make uninstall` // (in the build directory) to remove the ambe 3000 blocks from grc

73 Tom AC2IE
