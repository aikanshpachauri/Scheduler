CC = g++

CFLAGS  = -g

TARGET = scheduler

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp -L/usr/local/ssl/lib -lssl -lcrypto

