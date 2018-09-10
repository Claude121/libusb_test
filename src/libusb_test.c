#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "libusb_common.h"
#include "libusb_test.h"

#if 1
#define VendorId  0x525
#define ProductId 0xa4a0
#else
#define VendorId  0x03e7
#define ProductId 0x2150
#endif

bool func_trace = 1;
static int dbg_enable=1;
DBG_LEVEL_e debug_level = DBG_ALL;

struct libusb_device *dev;
struct libusb_device_handle *devh = NULL; 

deviceBootInfo_t devinfo;
uint8_t endpoint_d2h = 0, endpoint_h2d = 0;
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
    struct libusb_device **devs;

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
}

/* 
 * mode 1: read event
 * mode 2: write event
 * mode 3: write and read event
 * 
 * attention:
 *      mode 3 may failed if read immediately when write done
 */
#define TRANSFER_TEST_MODE 3
#define SIZE 40960
static void libusb_test_transfer(void)
{
#if (TRANSFER_TEST_MODE == 1)
	char *data_r;
	int i;
    
    data_r = (char*)malloc(SIZE);

	memset(data_r,0x34,4096);
	printf("read ret %d\n",usb_bulk_transfer(devh, endpoint_d2h, data_r, SIZE));

	for(i=0;i<5;i++)
	{
		if(i%10 == 0)
			printf("\n");

		printf(" 0x%x ",data_r[i]);
	}
	printf("\n");
    free(data_r);
#elif (TRANSFER_TEST_MODE == 2)
	char *data_w;
    
    data_w = (char*)malloc(SIZE);

	memset(data_w,0x21,4096);
	printf("write ret %d\n",usb_bulk_transfer(devh, endpoint_h2d, data_w, SIZE));
    free(data_w);
#elif (TRANSFER_TEST_MODE == 3)
	char *data_r, *data_w;
	int i;
    
    data_r = (char*)malloc(SIZE);
    data_w = (char*)malloc(SIZE);

	memset(data_w,0x21,4096);
	memset(data_r,0x34,4096);

	printf("write ret %d\n",usb_bulk_transfer(devh, endpoint_h2d, data_r, SIZE));
    usleep(5 * 1000);
	printf("read ret %d\n",usb_bulk_transfer(devh, endpoint_d2h, data_r, SIZE));

	for(i=0;i<5;i++)
	{
		if(i%10 == 0)
			printf("\n");

		printf(" 0x%x ",data_r[i]);
	}
	printf("\n");
    free(data_w);
    free(data_r);
#endif
}

int libusb_test_device_read(void *data, ssize_t size)
{
    if(!devh || !endpoint_d2h)
        return -1;

    INFO("endpoint_d2h 0x%x\n", endpoint_d2h);
	return usb_bulk_transfer(devh, endpoint_d2h, data, size);
}

ssize_t libusb_test_bulk_write(void *data, size_t size)
{
    if(!devh || !endpoint_h2d)
        return -1;

    INFO("endpoint_d2h 0x%x\n", endpoint_h2d);
	return usb_bulk_transfer(devh, endpoint_h2d, data, size);
}

int libusb_test_device_open(void)
{
	DEBUG("\n");
    int ret = 0;
	devinfo.vid = VendorId;
	devinfo.pid = ProductId;

    ret = libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);
    if(ret < 0)
        return ret;

    ret = usb_find_device(&dev, VendorId, ProductId);
    if(ret < 0)
        return ret;

    devh = usb_open_device(dev, &endpoint_d2h, &endpoint_h2d);
    if(devh == NULL)
        return -EINVAL;

    devh = usb_open_device(dev, &endpoint_d2h, &endpoint_h2d);
    if(devh == NULL) {
		ERR("Can't no Open Dev\n");
        return -1;
    }
	return 0;
}

void libusb_test_device_close(void)
{
	DEBUG("\n");
    if(devh) {
        usb_close_device(devh);
        devh = NULL;
        endpoint_d2h = 0;
        endpoint_h2d = 0;
    }
}


int libusb_test_dummy_api()
{
    return 0;
}
