#ifndef __LIBUSB_COMMON_H
#define __LIBUSB_COMMON_H

int usb_find_device(libusb_device **dev, int vid, int pid);
libusb_device_handle *usb_open_device(libusb_device *dev, uint8_t *in, uint8_t *out);
void usb_close_device(libusb_device_handle *h);
ssize_t usb_bulk_transfer(void *devh, int addr, void *data, size_t len);
#endif
