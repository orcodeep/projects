CC = gcc
CFLAGS = -o snake
LIBS = -lncurses

TARGET = snake
SOURCE = snake.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)

help:
	@echo "Usage:"
	@echo "  make           Build the project"
	@echo "  make clean     Remove the compiled executable"