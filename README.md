       LIBUSB SAMPLE TEST APP
1. Download libusb:
    git clone https://github.com/libusb/libusb.git
2. configuration
     ./configure --build=i686-linux --host=arm-linux --prefix=`pwd`/install CC=arm-linux-gcc --disable-udev
     make
     make install
3. make test 
   set CROSS_COMPILE & LIBUSBROOT
4. File System necessary
   suport usb fs system (linux)
