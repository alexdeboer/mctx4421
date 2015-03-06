#Make file for AUPlay
DIR = $(shell pwd)

OBJLIB = libM6au.o libM6au-intern.o
OBJTEST = test-libM6au.o
HEADERS = -I$(DIR)
LIBDIR = -L$(DIR)
LIBS = -lm -lM6au -lpthread

MAKE = gcc $(LIBDIR)

all: lib tester

lib: $(OBJLIB)
	ar rcs libM6au.a $(OBJLIB) 
	ranlib libM6au.a

tester: $(OBJTEST)
	$(MAKE) -o test-libM6au $(OBJTEST) $(LIBS)

libM6au.o: libM6au.c
	$(MAKE) -c -o libM6au.o libM6au.c 

libM6au-intern.o: libM6au-intern.c
	$(MAKE) -c -o libM6au-intern.o libM6au-intern.c

test-libM6au.o: test-libM6au.c
	$(MAKE) -c -o test-libM6au.o test-libM6au.c	

clean:
	rm -f $(OBJLIB) $(OBJTEST) libM6au.a test-libM6au
