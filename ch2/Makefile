CC=gcc 
CFLAGS= -Werror -Wall -g -ggdb
LDFLAGS=

MAIN=main.exe

.PHONY: all clean debug run

all: $(MAIN)
	@echo "Build $(MAIN)"

%.exe: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -c

run: all
	./$(MAIN)

debug: all
	gdb ./$(MAIN) -q

clean:
	rm $(MAIN)
