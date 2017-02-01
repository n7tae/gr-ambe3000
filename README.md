gr-ambe3000
===========
## Information

This will build a gnuradion OOT (out-of-tree) module to encode and decode a DStar audio stream.

It uses the Northwest Digital Radio AMBE-3000 dongle (http://www.nwdigitalradio.com) to encode and decode between audio and DStar data. The encoder builds a DStar data stream from an 8000 sample/second, 16-bit integer audio stream. The decoder converts a DStar data stream to a 8000 sample/second, 16-bit integer audio stream.

Be sure to look at the README file in the "examples" folder for an explanation of the example flow-graphs as well as a description of the blocks in this module.

##Installation

Change to any folder in your home directory and enter following commands in your terminal.
```
git clone https://github.com/ac2ie/gr-ambe3000.git
cd gr-ambe3000/ 
mkdir build
cd build/
cmake -DCMAKE_INSTALL_PREFIX=path_to_gnuradio_base ../
make
sudo make install
```
The "path_to_gnuradio_base" can be found with the command:
```
gnuradio-config-info --prefix
```

Because these ambe devices communicate through a /dev/tty* device, make sure you are in the `dialout` group:
```
sudo adduser <user> dialout
```
where `<user>` is your username. After you logout and log back in, you can use the device without problems.

##Updating

You can see at the github.com repository if anything new has been uploaded. If it has, you can go back to the gr-ambe3000 directory and pull any new stuff with `git pull`. If new files are downloaded, the safest thing to do uninstall, rebuild and reinstall:
```
cd build
sudo make uninstall
cd ..
rm -r build
```
and then complete the rebuild at the `mkdir build` command from the "Installation" section.

##Uninstall

If you don't want the ambe3000 blocks anymore, go back to the build directory and...
```
sudo make uninstall
```
After removal, you can go to the parent directory of gr-ambe3000 and safely remove the gr-ambe3000 directory.

##Other Stuff

I welcome comments and complaints, but moslty I welcome push notifications for improvements.

Thanks to the folks at NorthWest Digital Radio (especially for their ambeserver.c). If you find this software useful, it's because of them. If you don't, you can blame me!

##73

Tom AC2IE - ac2ie(at)arrl(dot)net
