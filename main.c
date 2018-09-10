#include <stdio.h>
#include <string.h>
#include "libusb_test.h"

#define ExecFunc(_func_, _ret_) \
	if (_func_ != _ret_)\
	{\
		printf("[%s %d]exec function failed\n", __FUNCTION__,__LINE__);\
		return -1;\
	}

int main(int argc,char **argv)
{
#if 0
    libusb_test_inquiry_dev();
#else
	ExecFunc(libusb_test_device_open(),0);

	libusb_test_dummy_api();

	libusb_test_device_close();
#endif
	return 0;
}
