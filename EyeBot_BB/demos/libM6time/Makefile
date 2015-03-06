# Thomas Braunl, UWA 2007
# -----------------------
# [MODIFIED BY AZMAN!]
# - TO BE USED ON WINDOWS ONLY?!

OLIBS = libM6time.o
TPROG = test-M6time
TDEPS =
FILES =
#CC    = gccarm
CC    = gcc

CFLAGS = -pthread -D_REENTRANT
LFLAGS = -lm ..\libM6os\libM6os.o

libs: $(OLIBS)

prog: $(TPROG)


%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c $<

$(TPROG): $(TPROG).c $(TDEPS) $(OLIBS)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

install: install-lib install-oth

install-lib:
	copy *.o ..\bin

install-oth:
	-copy $(TPROG) ..\bin
	-copy $(FILES) ..\bin

new: clean all

clean:
	rm -f $(OLIBS) $(TDEPS) $(TPROG)




