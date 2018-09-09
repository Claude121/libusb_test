#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <errno.h>
#include "libusb_boot.h"

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
int usb_find_device(void **device, int vid, int pid)
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

static libusb_device_handle *usb_open_device(libusb_device *dev, uint8_t *endpoint_in, uint8_t *endpoint_out)
{
	struct libusb_config_descriptor *cdesc;
	const struct libusb_interface_descriptor *ifdesc;
	libusb_device_handle *h = NULL;
	int ret, i;
	
	if(!dev || !endpoint_in || !endpoint_out)
		goto done;

	if ((ret = libusb_open(dev, &h)) < 0)
		goto done;

	if ((ret = libusb_set_configuration(h, 1)) < 0)
		goto close_dev;

	if ((ret = libusb_claim_interface(h, 0)) < 0)
		goto close_dev;

	if ((ret = libusb_get_config_descriptor(dev, 0, &cdesc)) < 0)
		goto close_dev;

	ifdesc = cdesc->interface->altsetting;
	for (i = 0; i < ifdesc->bNumEndpoints; i++) {
		if ((ifdesc->endpoint[i].bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) !=
		    LIBUSB_TRANSFER_TYPE_BULK)
			continue;

		if (ifdesc->endpoint[i].bEndpointAddress & 0x80) {
			*endpoint_out = ifdesc->endpoint[i].bEndpointAddress;
		} else {
			*endpoint_out = ifdesc->endpoint[i].bEndpointAddress;
		}
		printf( "Found EP 0x%02x : max packet size is %u bytes\n",
			    ifdesc->endpoint[i].bEndpointAddress,
			    ifdesc->endpoint[i].wMaxPacketSize);
	}
	libusb_free_config_descriptor(cdesc);
	goto done;

free_config:
	libusb_free_config_descriptor(cdesc);
close_dev:
	libusb_close(h);
done:
	return h;
}

