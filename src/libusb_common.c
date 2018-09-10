#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>
#include <errno.h>
#include "libusb_common.h"

#define USB_TIMEOUT (10 * 1000)
#define USB_MAX_PACKET_SIZE (40 * 1024)

#define USB_DIR_IN   0x80
#define USB_DIR_OUT  0x00

static int inited;

void __attribute__ ((constructor)) usb_library_load()
{
    inited = !libusb_init(NULL);
}

void __attribute__ ((destructor)) usb_library_unload()
{
    if (inited)
		libusb_exit(NULL);
}

int usb_find_device(libusb_device **device, int vid, int pid)
{
	int ret = 0, i = 0;;
	libusb_device **dev_list;
	libusb_device *dev;
	struct libusb_device_descriptor desc;

	if(!inited || !vid || !pid)
		return -EINVAL;

	if ((ret =libusb_get_device_list(NULL, &dev_list)) < 0)
		return ret;
	
	while((dev = dev_list[i++]) != NULL)
	{
		if((ret = libusb_get_device_descriptor(dev, &desc)) < 0)
            continue;

		if(vid != desc.idVendor || pid != desc.idProduct)
			continue;
		
		*device = dev;
		libusb_free_device_list(dev_list, 1);
		return 0;
	}
	libusb_free_device_list(dev_list, 1);
	return -1;
}

libusb_device_handle *usb_open_device(libusb_device *dev, uint8_t *endpoint_in, uint8_t *endpoint_out)
{
	struct libusb_config_descriptor *cdesc;
	const struct libusb_interface_descriptor *ifdesc;
	libusb_device_handle *h = NULL;
	int ret, i;
	
	if(!dev || !endpoint_in || !endpoint_out)
		goto done;

	if ((ret = libusb_get_config_descriptor(dev, 0, &cdesc)) < 0)
		goto done;

	ifdesc = cdesc->interface->altsetting;
	for (i = 0; i < ifdesc->bNumEndpoints; i++) {
		if ((ifdesc->endpoint[i].bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) !=
		    LIBUSB_TRANSFER_TYPE_BULK)
			continue;

		if (ifdesc->endpoint[i].bEndpointAddress & USB_DIR_IN) {
			*endpoint_in = ifdesc->endpoint[i].bEndpointAddress;
		} else {
			*endpoint_out = ifdesc->endpoint[i].bEndpointAddress;
		}
		printf( "Found %s EP 0x%02x : max packet size is %u bytes\n",
                ifdesc->endpoint[i].bEndpointAddress & USB_DIR_IN ?"IN ":"OUT",
			    ifdesc->endpoint[i].bEndpointAddress,
			    ifdesc->endpoint[i].wMaxPacketSize);
	}
	libusb_free_config_descriptor(cdesc);

	if ((ret = libusb_open(dev, &h)) < 0)
		goto done;

	libusb_set_auto_detach_kernel_driver(h, 1);

//    libusb_set_configuration();

	if ((ret = libusb_claim_interface(h, 0)) < 0)
		goto close_dev;

	goto done;

close_dev:
	libusb_close(h);
done:
	return h;
}

void usb_close_device(libusb_device_handle *h)
{
    if(!inited || !h)
        return;

    libusb_release_interface(h, 0);
    libusb_close(h);
}

ssize_t usb_bulk_transfer(void *devh, int endpoint_addr, void *data, size_t length)
{
	int tt = 0, ret = 0;

    if(!inited || !endpoint_addr || !data)
        return -EINVAL;

    printf("%s ep 0x%x \n",__func__,endpoint_addr);
    while(length > 0) {
        int sz = length, bt;

        if (sz > USB_MAX_PACKET_SIZE)
            sz = USB_MAX_PACKET_SIZE;
	    ret = libusb_bulk_transfer(devh, endpoint_addr, 
                        (unsigned char *)data, length, &bt, USB_TIMEOUT);
	    if(ret < 0)
		    return ret;

        tt += bt;
        length -= bt;
        data = (char*)data + bt;
    }
	return tt;
}
