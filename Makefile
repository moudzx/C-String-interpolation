CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = rprintf_demo
OBJS = rprintf.o index.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

rprintf.o: rprintf.c rprintf.h
	$(CC) $(CFLAGS) -c rprintf.c -o rprintf.o

index.o: index.c rprintf.h
	$(CC) $(CFLAGS) -c index.c -o index.o

clean:
	rm -f $(OBJS) $(TARGET)
