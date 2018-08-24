CC=gcc
LIB=
LIBDIR=-lpthread -L/usr/local/lib
INCDIR=-I/usr/local/include
CFLAGS=-ggdb -Wall  $(DBGFLG)
TARGETS=schedtst

SRCS=main.c

OBJS=$(SRCS:.c=.o)

all:	$(TARGETS) $(OBJS)

$(TARGETS):	$(OBJS) 
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(INCDIR) $(LIBDIR) $(LIB)

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCDIR) $(LIBDIR) $(LIB)

.PHONY:	clean
clean:
	$(RM) *~ $(TARGETS) $(OBJS)
