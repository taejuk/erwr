CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = main

SRCS = main.c util.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -lm

main.o: main.c util.h type.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h type.h
	$(CC) $(CFLAGS) -c util.c

clean:
	rm -f $(OBJS) $(TARGET)