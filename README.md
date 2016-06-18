gr-ambe3000
===========
## Information

This will build a gnuradion OOT (out-of-tree) module to encode and decode a DStar audio stream.

It uses the Northwest Digital Radio USB AMBE3000 (http://www.nwdigitalradio.com) dongle to encode and decode between audio and DStar data. The encoder builds a DStar data stream from an 8000 sample/second, 16-bit integer audio stream. The decoder converts a DStar data stream to a 8000 sample/second, 16-bit integer audio stream.

See the README file in the "examples" folder for an explanation of the example flow-graphs.

##Install guide 

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

##Other Stuff

I welcome comments and complaints, but moslty I welcome push notifications for improvements.

Thanks to the folks at NorthWest Digital Radio (especially for their ambeserver.c). If you find this software useful, it's because of them. If you don't, it's probably my fault!

73 Tom AC2IE  <ac2ie (at) arrl (dot) net
