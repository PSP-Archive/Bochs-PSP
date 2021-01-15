# Copyright (C) 2001  MandrakeSoft S.A.
#
#   MandrakeSoft S.A.
#   43, rue d'Aboukir
#   75002 Paris - France
#   http://www.linux-mandrake.com/
#   http://www.mandrakesoft.com/
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA



.SUFFIXES: .cc

srcdir = .


SHELL = /bin/sh



CC = psp-gcc
CFLAGS = -I/usr/local/pspdev/psp/sdk/include -I/usr/local/pspdev/psp/sdk/../include/SDL -Dmain=SDL_main -Wall -O2 -G0
CXX = psp-g++
CXXFLAGS = -I/usr/local/pspdev/psp/sdk/include -I/usr/local/pspdev/psp/sdk/../include/SDL -Dmain=SDL_main -Wall -O2 -G0 -fno-exceptions -fno-rtti

LDFLAGS = -L/usr/local/pspdev/psp/sdk/../lib -lSDLmain -lSDL -lm -L/usr/local/pspdev/psp/sdk/lib -lpspdebug -lpspgu -lpspctrl -lpspaudio -lc -lpspglue -lc -lpspkernel -lpspuser -lpspge -lpspdisplay -lpsphprm -L/usr/local/pspdev/psp/sdk/lib -lc -lpspglue -lc -lpspuser
LIBS =  -lm
RANLIB = psp-ranlib


# ===========================================================
# end of configurable options
# ===========================================================


BX_OBJS = \
  instrument.o

BX_INCLUDES =

BX_INCDIRS = -I../.. -I$(srcdir)/../.. -I. -I$(srcdir)/.

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(BX_INCDIRS) $< -o $@


.c.o:
	$(CC) -c $(CFLAGS) $(BX_INCDIRS) $< -o $@



libinstrument.a: $(BX_OBJS)
	rm -f  libinstrument.a
	ar rv $@ $(BX_OBJS)
	$(RANLIB) libinstrument.a

$(BX_OBJS): $(BX_INCLUDES)


clean:
	rm -f  *.o
	rm -f  *.a

dist-clean: clean
	rm -f  Makefile
