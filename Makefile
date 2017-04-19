#a simple makefile to support this project

CC=g++
INC=./INC
SRC=./SRC/CMutex.cpp ./SRC/KLogger.cpp ./SRC/MemoryPool.cpp ./SRC/Utility.cpp Main.cpp
OBJS:=$(SRC:.cpp=.o)

CC_FLAG=-Wall
LIB= -lpthread

PRG=KLoggerTest

$(PRG):$(OBJS)
	$(CC) $(LIB) -o $@ $(OBJS)
	
.SUFFIXES: .c .o .cpp
.cpp.o:
	$(CC) $(CC_FLAG) -c $*.cpp -o $*.o

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	rm -f $(OBJS) $(PRG)


