CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = ergasia2

all: $(TARGET)

$(TARGET): ergasia2.o
	$(CC) $(CFLAGS) -o $(TARGET) ergasia2.o

ergasia2.o: ergasia2.c ergasia2.h
	$(CC) $(CFLAGS) -c ergasia2.c
    
clean:
	rm -f *.o $(TARGET)
