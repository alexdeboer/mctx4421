# Thomas Braunl, UWA 2007
# -----------------------
# [MODIFIED BY AZMAN!]
# - TO BE USED ON WINDOWS ONLY?!

OLIBS = libM6au.o libM6au-intern.o
#TPROG = test-libM6au
#TPROG = test-tone
TPROG = test-audiorec
TDEPS =
FILES =
CC    = gccarm

CFLAGS = -pthread -D_REENTRANT
LFLAGS = -lm 	..\libM6thread\libM6thread.o \
		..\libM6time\libM6time.o \
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
	-del $(OLIBS) $(TDEPS) $(TPROG)

