override CFLAGS := -Wall -Werror -std=gnu99 -O0 -g $(CFLAGS) -I.

# you can uncomment following to add sanitizer for development; note make sure to not have this for submissions
#override CFLAGS :=  -fsanitize=undefined $(CFLAGS)
#override LDFLAGS := -fsanitize=undefined -fsanitize=leak $(LDLAGS)

CC = gcc
test_files = ./test_fs ./test_bit_ops ./test_disk ./test_fs1

# Build the threads.o file
test_fs: test_fs.o fs.o disk.o

fs1.o: fs1.c

test_fs.o: test_fs.c

fs.o: fs.c fs.h

disk.o: disk.c disk.h

test_find_bit: test_find_bit.o

test_find_bit.o: test_find_bit.c

.PHONY: clean

clean:
	rm -f *.o *~ $(test_files)
