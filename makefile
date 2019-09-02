CC=g++
CFLAGS=-std=c++11 -lSDL2
DEPS=chip8.h window.h
OBJ=main.o chip8.o window.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm *.o	
