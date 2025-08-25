CC = gcc

CFLAGS = -Wall

TARGET = rush

SRC = rush.c

all: $(TARGET)

$(TARGET) : $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)