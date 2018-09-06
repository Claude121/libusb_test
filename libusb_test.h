#ifndef __LIBUSB_TEST_H_
#define __LIBUSB_TEST_H_
#include <unistd.h>
#include "libusb_test_datatype.h"

int libusb_test_device_open(void);
void libusb_test_device_close(void);
void libusb_test_inquiry_dev(void);
ssize_t libusb_test_bulk_read(void *data, size_t length);
ssize_t libusb_test_bulk_write(const void *data, size_t length);
void libusb_test_dummy_api(void);
#endif
