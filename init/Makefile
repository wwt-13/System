# 还得好好学学makefile文件格式的书写
INCLUDES := -I../include

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: init.o main.o

clean:
	rm -rf *~ *.o

include ../include.mk
