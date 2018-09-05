#include <stdio.h>

#include "libusb_test.h"

#define ExecFunc(_func_, _ret_) \
    if (_func_ != _ret_)\
    {\
        printf("[%s %d]exec function failed\n", __FUNCTION__,__LINE__);\
        return -1;\
    }

int main(int argc,char **argv)
{
	ExecFunc(libusb_test_device_open(),0);

	libusb_test_device_close();
    return 0;  
}
