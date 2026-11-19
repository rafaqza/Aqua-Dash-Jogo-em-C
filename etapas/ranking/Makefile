CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = jogo

all:
	$(CC) $(CFLAGS) src/main.c -o $(TARGET) $(LIBS)

run: all
	./$(TARGET)

# testa so o ranking (nao precisa de raylib)
teste_recordes:
	$(CC) $(CFLAGS) src/teste_recordes.c src/recordes.c -o teste_recordes
	./teste_recordes

clean:
	rm -f $(TARGET) teste_recordes
