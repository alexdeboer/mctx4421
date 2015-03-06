# Thomas Braunl, UWA 2007
# -----------------------
# [MODIFIED BY AZMAN!]
# - TO BE USED ON WINDOWS ONLY?!
# -----------------------
# [MODIFIED BY STUART 2014]
# - Modified to run on UNIX only

OLIBS = libM6ip.o
TPROG =
TDEPS =
FILES =
#CC    = gccarm
CC    = gcc

CFLAGS =
LFLAGS = -lm    ..\libM6time\libM6time.o \
	     	..\libM6key\libM6key.o \
	     	..\libM6lcd\libM6lcd.o ..\libM6lcd\libM6lcd_font.o \

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

