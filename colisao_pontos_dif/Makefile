CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = jogo
SRCS = src/colisoes_pontuacao.c

all:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
