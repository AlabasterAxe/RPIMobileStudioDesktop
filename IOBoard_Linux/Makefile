#
# This Makefile was generated automatically by lvmkmf.
#
CC=gcc
LD=gcc
LDFLAGS=-shared
XLDFLAGS=-L/home/temp/IOBoard/lib/rpims_v2/.libs/ -L/home/temp/IOBoard/lib/libusb-0.1.12/.libs/ -lrpims -lusb
#XLDFLAGS=-L/home/temp/IOBoard/rpims_v2/.libs/ -L/home/kleinm4/Desktop/lib/libusb-0.1.12/.libs/ -lrpims -lusb
XFLAGS=-fPIC -O
CINDIR=/usr/local/lv86/cintools
INCDIR=/home/kleinm4/Desktop/rpims_v2
CFLAGS=-I$(CINDIR) -I$(INCDIR) $(XFLAGS)
CINLIB=$(CINDIR)/libcin.a
MAKEGLUE=$(CINDIR)/makeglueLinux.awk
AS=gcc -fPIC -c

.SUFFIXES: .lsb .lsb~ $(SUFFIXES)

# Default rule to create an lsb from a C source file
.c.lsb:	; make $*.o
	$(LD) $(LDFLAGS) -o $*.tmp \
		$(CINDIR)/cin.o $(XLDFLAGS) $*.o $(CINLIB)
	$(CINDIR)/lvsbutil -c $* -t CIN -d "`pwd`"
	@rm -f $*.tmp


open_board.lsb:	open_board.o
	$(LD) $(LDFLAGS) -o open_board.tmp \
		$(CINDIR)/cin.o $(XLDFLAGS) open_board.o  $(CINLIB)
	$(CINDIR)/lvsbutil -c open_board -t CIN -d "`pwd`"
	@rm -f open_board.tmp

clean:
	$(RM) -f open_board.o open_board.tmp

spotless: clean
	$(RM) -f open_board.lsb

