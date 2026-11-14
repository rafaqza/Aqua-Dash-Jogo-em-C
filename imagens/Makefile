CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = jogo

all:
	$(CC) $(CFLAGS) src/main.c -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) 
