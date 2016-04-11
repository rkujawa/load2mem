CC=vc
RMRF=rm -rf

CFLAGS=
LDFLAGS=-lamiga

OBJECTS=load2mem.o
EXECUTABLE=load2mem

all: $(EXECUTABLE) 

9tcfg : $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(LDFLAGS) $(OBJECTS)

%.o : %.c 
	$(CC) $(CFLAGS) -c $<

clean :
	$(RMRF) $(OBJECTS) $(EXECUTABLE) 

