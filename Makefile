override CFLAGS := -Wall -Werror -std=gnu99 -O0 -g $(CFLAGS) -I.

# you can uncomment following to add sanitizer for development; note make sure to not have this for submissions
#override CFLAGS :=  -fsanitize=undefined $(CFLAGS)
#override LDFLAGS := -fsanitize=undefined -fsanitize=leak $(LDLAGS)

CC = gcc

# Build the threads.o file
fs.o: fs.c fs.h

.PHONY: clean

clean:
	rm -f *.o
