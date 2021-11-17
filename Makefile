CC=gcc
CFLAGS=-g
LIBS=-lz
TARGET=rfcc
OBJS=main.o rfc822.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) *~ *core
