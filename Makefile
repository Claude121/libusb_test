LIBUSBROOT ?= ~/Workdir/libusb/libusb

INCLUDES := \
	-I . \
	-I ../include \
	-I $(LIBUSBROOT)/libusb

CROSS_COMPILE:=arm-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
CC_FLAG=-Wall
LIB:=-L $(LIBUSBROOT)/libusb/.libs -lusb-1.0
  
PRG:=prog
OBJ:=libusb.o
   
$(PRG):$(OBJ)
	$(CC) $(INC) -o $@ $(OBJ) $(LIB)
			
.SUFFIXES: .c .o .cpp
.cpp.o:
	@$(CC) $(CC_FLAG) $(INCLUDES) -c $*.cpp -o $*.o
		 
.c.o:
	@$(CC) $(CC_FLAG) $(INCLUDES) -c $*.c -o $*.o

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	@rm -f $(OBJ) $(PRG)
