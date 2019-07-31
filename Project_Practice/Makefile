CC = gcc
CFLAGS = -Wall -std=c99
OBJFILES = gpio-utils.c paul_rqust_linehdl.c
TARGET = paul_rqust_linehdl

all: $(TARGET)
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

clean:
	rm -f paul_rqust_linehdl
