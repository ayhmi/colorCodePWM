PROJECT = colorCodePWM
IDIR =src
SRCDIR=src
CC=gcc
CFLAGS=-Wnonnull -I$(IDIR)

ODIR=src
LDIR=lib

LIBS=-lwiringPi -lpthread

_DEPS = colorControl.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = colorControl.o server.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROJECT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

