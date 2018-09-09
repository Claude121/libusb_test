#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "libusb_test.h"
#if 0
#define VendorId  0x525
#define ProductId 0xa4a0
#else
#define VendorId  0x03e7
#define ProductId 0x2150
#endif

#define USB_TIMEOUT (10 * 1000)
#define USB_MAX_PACKET_SIZE (10 * 1024 * 1024)

bool func_trace = 1;
static int dbg_enable=1;
DBG_LEVEL_e debug_level = DBG_ALL;

struct libusb_device **devs;
struct libusb_context *ctx = NULL;
struct libusb_device_handle *devh = NULL; 

deviceBootInfo_t devinfo;
int endpoint_d2h = 0, endpoint_h2d = 0;
int payload_d2h = 0, payload_h2d = 0;

static void libusb_test_inquiry_per_dev(libusb_device *dev)
{
	int s32Ret = 0,n = 0,i = 0,j = 0,k = 0;
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor *config;
	const struct libusb_interface *inter;
	const struct libusb_interface_descriptor *interdesc;
	const struct libusb_endpoint_descriptor *epdesc;

	s32Ret = libusb_get_device_descriptor(dev, &desc);
	if (s32Ret < 0) {
		DEBUG("failed to get device descriptor:%s\n",libusb_strerror(s32Ret));
		return;
	}

	LOG("\n------------ Device Specific ------------ \n\n");
	LOG(" %-9s	  %-8s	  %-9s	 %-9s\n",
				"Dev Class","VendorId","ProductId","ConfigNum");
	LOG(" 0x%-9x  0x%-8x  0x%-9x 0x%-9x\n",
			(int)desc.bDeviceClass,desc.idVendor,desc.idProduct,(int)desc.bNumConfigurations);

	LOG("\n------------ Class Specific ------------- \n\n");
	for(n=0; n<desc.bNumConfigurations; n++)
	{
		libusb_get_config_descriptor(dev, n, &config);
		LOG(" config%d , Interfaces: %d \n", n, (int)config->bNumInterfaces);

		for(i=0; i<(int)config->bNumInterfaces; i++) {
			inter = &config->interface[i];
			LOG("  Num of alt settings:   %d \n",inter->num_altsetting);
			for(j=0; j<inter->num_altsetting; j++) {
				interdesc = &inter->altsetting[j];
				LOG("		Interface Numer    :  %d \n",(int)interdesc->bInterfaceNumber);
				LOG("		Number of endpoints:  %d \n",(int)interdesc->bNumEndpoints);
 
				for(k=0; k<(int)interdesc->bNumEndpoints; k++) {
					epdesc = &interdesc->endpoint[k];
					LOG("			EP Address	   :  0x%x \n",(int)epdesc->bEndpointAddress);
					LOG("			Descriptor Type:  0x%x \n",(int)epdesc->bDescriptorType);
				}
			}
			LOG("\n");
		}
		libusb_free_config_descriptor(config);
	}
}

void libusb_test_inquiry_dev(void)
{
	DEBUG("\n");
	int i;
	long cnt;
libusb_init(NULL);
	cnt = libusb_get_device_list(NULL, &devs);
	if(cnt < 0) {
		ERR("Get Device Error %s\n",libusb_strerror(cnt));
	}
	LOG("\n========================================================== \n\n");
	LOG(" Total %ld Devices in list.\n\n",cnt);

	for(i = 0; i < cnt; i++) {
		libusb_test_inquiry_per_dev(devs[i]);
	}
	LOG("\n========================================================== \n\n");
	libusb_free_device_list(devs, 1);
libusb_exit(NULL);
}

static int libusb_test_get_endpoint_info()
{
	DEBUG("\n");
	int n;
	long cnt;

	if(devinfo.vid == 0 ||devinfo.pid == 0)
	{
		ERR("Please Exec libusb_test_device_open First\n");
		return -1;
	}

	cnt = libusb_get_device_list(NULL, &devs);
	if(cnt < 0) {
		ERR("Get Device Error %s\n",libusb_strerror(cnt));
		return -1;
	}

	for(n = 0; n < cnt; n++)
	{
		int s32Ret = 0,i = 0,j = 0,k = 0;
		struct libusb_device_descriptor desc;
		struct libusb_config_descriptor *config;
		const struct libusb_interface *inter;
		const struct libusb_interface_descriptor *interdesc;
		const struct libusb_endpoint_descriptor *epdesc;

		s32Ret = libusb_get_device_descriptor(devs[n], &desc);
		if (s32Ret < 0) {
			DEBUG("failed to get device descriptor:%s\n",libusb_strerror(s32Ret));
			return -1;
		}

		if(desc.idVendor != devinfo.vid || desc.idProduct != devinfo.pid)
			continue;

		libusb_get_config_descriptor(devs[n], 0, &config);

		for(i=0; i<(int)config->bNumInterfaces; i++) {
			inter = &config->interface[i];
			for(j=0; j<inter->num_altsetting; j++) {
				interdesc = &inter->altsetting[j];
				for(k=0; k<(int)interdesc->bNumEndpoints; k++) {
					epdesc = &interdesc->endpoint[k];
					if ((epdesc->bmAttributes & 0x03) != LIBUSB_TRANSFER_TYPE_BULK)
						continue;

					if (epdesc->bEndpointAddress & 0x80)
					{
						endpoint_d2h = epdesc->bEndpointAddress;
						payload_d2h = epdesc->wMaxPacketSize;
					}
					else
					{
						endpoint_h2d = epdesc->bEndpointAddress;
						payload_h2d = epdesc->wMaxPacketSize;
					}
				}
			}
		}
		libusb_free_config_descriptor(config);
	}

	if (endpoint_d2h == 0 || endpoint_h2d == 0)
	{
		ERR("Interface is not complete.\n");
		return -1;
	}

	INFO("bulk in_ep addr: 0x%x packetsize: %d\n",endpoint_d2h,payload_d2h);
	INFO("bulk out_ep addr: 0x%x packetsize: %d\n",endpoint_h2d,payload_h2d);
	libusb_free_device_list(devs, 1);
	return 0;
}

void libusb_test_dummy_api(void)
{
//	libusb_set_configuration(devh,0);
#if 1
	char data[4096];
	int i;
	memset(data,0x66,4096);

	printf("write ret %d\n",libusb_test_bulk_write(devh, data, 4096));

	printf("read ret %d\n",libusb_test_bulk_read(devh, data, 4096));

	for(i=0;i<512;i++)
	{
		if(i%10 == 0)
			printf("\n");

		printf(" 0x%x ",data[i]);
	}
	printf("\n");
#else
	libusb_set_interface_alt_setting(devh,0,0);
#endif
}

ssize_t libusb_test_bulk_read(void *devh, void *data, size_t length)
{
	int tt = 0, ret = 0;

    while(length > 0) {
        int sz = length, bt;

        if (sz > USB_MAX_PACKET_SIZE)
            sz = USB_MAX_PACKET_SIZE;
	    ret = libusb_bulk_transfer(devh, endpoint_d2h, 
                        (unsigned char *)data, sz, &bt, USB_TIMEOUT);
	    if(ret < 0)
		    return ret;

        tt += bt;
        length -= bt;
        data = (char*)data + bt;
    }
	return tt;
}

ssize_t libusb_test_bulk_write(void *devh, const void *data, size_t length)
{
	int tt = 0, ret = 0;

    while(length > 0) {
        int sz = length, bt;

        if (sz > USB_MAX_PACKET_SIZE)
            sz = USB_MAX_PACKET_SIZE;
	    ret = libusb_bulk_transfer(devh, endpoint_h2d, 
                        (unsigned char *)data, length, &bt, USB_TIMEOUT);
	    if(ret < 0)
		    return ret;

        tt += bt;
        length -= bt;
        data = (char*)data + bt;
    }
	return tt;
}

int libusb_test_device_open(void)
{
	DEBUG("\n");
	
	LIBUSB_CHECK(libusb_init(&ctx));

	libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);

	devinfo.vid = VendorId;
	devinfo.pid = ProductId;

	if(libusb_test_get_endpoint_info() < 0) {
		return -1;
	}

	devh = libusb_open_device_with_vid_pid(ctx, devinfo.vid, devinfo.pid);
	if(devh==NULL) {
		ERR("Failed to get a device handle\n");
		return -1;
	}

	libusb_set_auto_detach_kernel_driver(devh, 1);
	libusb_claim_interface(devh, 0);

	return 0;
}

void libusb_test_device_close(void)
{
	DEBUG("\n");

	libusb_release_interface(devh, 0);

	if(devh)
		libusb_close(devh);
	if(ctx)
		libusb_exit(ctx);
}

