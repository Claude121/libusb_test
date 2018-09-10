#ifndef __LIBUSB_TEST_H_
#define __LIBUSB_TEST_H_
#include <unistd.h>
#include "libusb_test_datatype.h"

void libusb_test_inquiry_dev(void);
int libusb_test_device_open(void);
void libusb_test_device_close(void);
int libusb_test_device_read(void *data, ssize_t size);
int libusb_test_device_write(void *data, ssize_t size);
int libusb_test_dummy_api();
#endif
