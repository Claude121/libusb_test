INCLUDES := \
	-I. \
	-I../include \
	-I$(SYSROOT)/usr/include/libusb-1.0 \

CC=gcc
CC_FLAG=-Wall
LIB:=-lusb-1.0
  
PRG:=prog
OBJ:=libusb.o
   
$(PRG):$(OBJ)
	@$(CC) $(INC) -o $@ $(OBJ) $(LIB)
			
.SUFFIXES: .c .o .cpp
.cpp.o:
	@echo "Compiling......"
	@$(CC) $(CC_FLAG) $(INCLUDES) -c $*.cpp -o $*.o
		 
.c.o:
	@echo "Compiling......"
	@$(CC) $(CC_FLAG) $(INCLUDES) -c $*.c -o $*.o

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	@rm -f $(OBJ) $(PRG)
