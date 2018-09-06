LIBUSBROOT ?= ~/Workdir/libusb/libusb

INCLUDES := \
	-I . \
	-I ../include \
	-I $(LIBUSBROOT)/libusb

CROSS_COMPILE := arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
CC_FLAG =-Wall -o2
LDFLAGS =-static
LIB:=-L $(LIBUSBROOT)/libusb/.libs -lusb-1.0 -lpthread
  
PRG:=prog
OBJ:=main.o libusb_test.o
   
$(PRG):$(OBJ)
	@echo "Linkind Objects ......"
	@$(CC) $(LDFLAGS) $(INC) -o $@ $(OBJ) $(LIB)
			
.SUFFIXES: .c .o .cpp
.cpp.o:
	@$(CC) $(CC_FLAG) $(INCLUDES) -c $*.cpp -o $*.o
		 
.c.o:
	@$(CC) $(CC_FLAG) $(INCLUDES) -c $*.c -o $*.o

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	@rm -f $(OBJ) $(PRG)
