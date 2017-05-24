title: The AMBE3000 OOT Module
brief: Uses the NW Digital Radio AMBE USB Dongle to encode/decode audio
tags:
  - sdr
  - DStar
  - AMBE3000
author:
  - Thomas Early <n7tae (at) arrl (dot) net>
copyright_owner:
  - Thomas A. Early
repo: http://github.com/n7tae/gr-ambe3000
website: http://nwdigitalradio.com
---
Uses the Northwest Digital Radio USB AMBE-3000 dongle to encode and decode between audio and DStar data. The encoder builds a DStar data stream from an 8000 sample/second, 16-bit integer audio stream. The decoder converts a DStar data stream to a 8000 sample/second, 16-bit integer audio stream.

See the README file in the examples directory for an explanation of the included flow-graphs.

**Install guide**  

Change to any folder in your home directory and enter following commands in your terminal.

`git clone https://github.com/n7tae/gr-ambe3000.git` // clone this repository  
`cd gr-ambe3000/`  
`mkdir build` // make build folder  
`cd build/`  
`cmake ../` // build makefiles (possible include -DCMAKE_INSTALL_PREFIX=<path_to_gnuradio>, see the gnuradio WIKI)
`make` // build toolbox (There may be a few warngings, mostly about doxygen, but no errors.)
`sudo make install` // to make the ambe3000 blocks available to gnuradio-companion.

And if you don't want the ambe3000 blocks anymore...

`sudo make uninstall` // (in the build directory) to remove the ambe 3000 blocks from grc

I welcome comments and complaints, but moslty I welcome push notifications for improvements.

Thanks to the folks at NorthWest Digital Radio, especially ambeserver.c. If you find this software useful, it's because of them. If you don't, it's probably my fault!

73 Tom <n7tae (at) arrl (dot) net
