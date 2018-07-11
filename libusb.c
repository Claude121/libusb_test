#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>

static int dbg_enable=1;
#define DBG(fmt, args...) \
    ({do{if(dbg_enable){printf("[%s,%d]: "fmt,__func__,__LINE__,##args);}}while(0);})

#define LOG(fmt, args...) \
    ({do{if(dbg_enable){printf(fmt,##args);}}while(0);})

typedef struct {
    int pid;
    char name[10];
} deviceBootInfo_t;

static int initialized;
void printdev(libusb_device *dev);

void __attribute__((constructor))test_init()
{
    DBG("\n");
    initialized = !libusb_init(NULL);
    if(initialized<0){
        DBG(" error\n");
        exit(0);
    }
}
void __attribute__((destructor))test_exit()
{
    DBG("\n");
    if(initialized)
        libusb_exit(NULL);
}

int main(int argc,char **argv)
{
    DBG("Enter LibUSB Test APP\n");
    ssize_t i;
    ssize_t cnt;
    libusb_device **devs;
    libusb_context *ctx = NULL;

    libusb_set_debug(ctx, 3);
    cnt = libusb_get_device_list(ctx, &devs);
    if(cnt < 0) {  
        DBG("Get Device Error %s\n",libusb_strerror(cnt)); 
    }  
    DBG(" %ld Devices in list.",cnt);
   
    for(i = 0; i < cnt; i++) {  
        printdev(devs[i]);
    }  
    libusb_free_device_list(devs, 1);
    return 0;  
}

void printdev(libusb_device *dev)
{
    int s32Ret = 0;
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor *config;
    const struct libusb_interface *inter;
    const struct libusb_interface_descriptor *interdesc;
    const struct libusb_endpoint_descriptor *epdesc;

    s32Ret = libusb_get_device_descriptor(dev, &desc);
    if (s32Ret < 0) {
        DBG("failed to get device descriptor:%s\n",libusb_strerror(s32Ret));
        return;
    }

    LOG("Device Class:  %4d \n",(int)desc.bDeviceClass);
    LOG("VendorID:      %4d \n",desc.idVendor);
    LOG("ProductID:     %4d\n",desc.idProduct);
    LOG("Num of possible configs: %4d \n",(int)desc.bNumConfigurations);

    libusb_get_config_descriptor(dev, 0, &config);
    LOG("Interfaces: %d ||| ",(int)config->bNumInterfaces);

    for(int i=0; i<(int)config->bNumInterfaces; i++) {
        inter = &config->interface[i];
        LOG("Num of alt settings: %d | ",inter->num_altsetting);
        for(int j=0; j<inter->num_altsetting; j++) {
            interdesc = &inter->altsetting[j];
            LOG("Interface Numer: %d | ",(int)interdesc->bInterfaceNumber);
            LOG("Number of endpoints: %d | ",(int)interdesc->bNumEndpoints);
 
            for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
                epdesc = &interdesc->endpoint[k];
                LOG("Descriptor Type: %d | ",(int)epdesc->bDescriptorType);
                LOG("EP Address: %d | ",(int)epdesc->bEndpointAddress);
            }
        }
        LOG("\n");
    }
    LOG("\n");
    libusb_free_config_descriptor(config);
}
