# Makefile Eyebot RoBIOS System Demos
# Thomas Braunl, 2005

MCINC    = -I$(mc)/include -I/home/pi/demos/M5-demo/include
GCCINC   = -I$(GCCDIR)/include -I$(GCCDIR)/lib/3.3.1/include
HEXDIR   = "$(mc)/hex"

ifeq ($(shell ver),)
 COPY = cp -f
 RM = rm -f
else
 COPY = copy
 RM = del
endif

all:    platform demos.hex

platform:
	@echo Platform: $(PLATFORM)

OBJS = demos.o radio-demo.o bluetooth.o wlan.o sample.o camera-demo.o
COMP = gcc

demos.hex:  $(OBJS)
	$(COMP) -o demos.hex ${OBJS}
	srec2bin -cb $@ demos.hx
	$(COPY) $@ $(HEXDIR)
	$(COPY) demos.hx $(HEXDIR)

.c.o:
	$(COMP) -c -o $*.o $*.c
	
clean:
	$(RM) *.o
	$(RM) map
	$(RM) *~
	$(RM) *.bak
	$(RM) *.hex
	$(RM) *.hx
	$(RM) *.m68k
	$(RM) *.elf
