#ifndef __LIBUSB_TEST_DATATYPE_H_
#define __LIBUSB_TEST_DATATYPE_H_

typedef enum
{
    DBG_NONE = 0,
    DBG_ERR,
    DBG_WRN,
    DBG_INFO,
    DBG_DEBUG,
    DBG_ALL
} DBG_LEVEL_e;

#define ASCII_COLOR_RED                          "\033[1;31m"
#define ASCII_COLOR_WHITE                        "\033[1;37m"
#define ASCII_COLOR_YELLOW                       "\033[1;33m"
#define ASCII_COLOR_BLUE                         "\033[1;36m"
#define ASCII_COLOR_GREEN                        "\033[1;32m"
#define ASCII_COLOR_END                          "\033[0m"

#define DEBUG(fmt, args...) ({do{if(dbg_enable && debug_level>=DBG_DEBUG)\
    {printf(ASCII_COLOR_GREEN"[APP DBG]:%s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__,##args);}}while(0);})
#define INFO(fmt, args...)     ({do{if(dbg_enable && debug_level>=DBG_INFO)\
    {printf(ASCII_COLOR_GREEN"[APP INFO]:%s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__,##args);}}while(0);})
#define WRN(fmt, args...)      ({do{if(dbg_enable && debug_level>=DBG_WRN)\
    {printf(ASCII_COLOR_YELLOW"[APP WRN ]: %s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__, ##args);}}while(0);})
#define ERR(fmt, args...)      ({do{if(dbg_enable && debug_level>=DBG_ERR)\
    {printf(ASCII_COLOR_RED"[APP ERR ]: %s[%d]: " fmt ASCII_COLOR_END, __FUNCTION__,__LINE__, ##args);}}while(0);})

#define EXIT_ERR(fmt, args...) ({do\
    {printf(ASCII_COLOR_RED"<<<%s[%d] " fmt ASCII_COLOR_END,__FUNCTION__,__LINE__,##args);}while(0);})
#define ENTER()                ({do{if(func_trace)\
    {printf(ASCII_COLOR_BLUE">>>%s[%d] \n" ASCII_COLOR_END,__FUNCTION__,__LINE__);}}while(0);})
#define EXIT_OK()              ({do{if(func_trace)\
    {printf(ASCII_COLOR_BLUE"<<<%s[%d] \n" ASCII_COLOR_END,__FUNCTION__,__LINE__);}}while(0);})

#define LOG(fmt, args...) \
    ({do{if(dbg_enable){printf(fmt,##args);}}while(0);})

#define LIBUSB_CHECK(action) \
do { \
  int ret = (action); \
  if (ret != LIBUSB_SUCCESS) \
  { \
    fprintf(stderr, "%s\n", libusb_strerror(ret)); \
    return -1; \
  } \
} while(0)

typedef struct {
    int  vid;
    int  pid;
    char name[10];
} deviceBootInfo_t;
#endif
