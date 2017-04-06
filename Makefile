CC= gcc
CFLAGS= -Wall
OBJS= conio.o minesweeper.o
HEADER=conio.h
TARGET=minesweeper

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(OBJS): $(HEADER)

clean:
	rm $(OBJS) $(TARGET)

.PHONY: clean
