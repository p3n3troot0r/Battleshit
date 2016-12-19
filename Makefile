OBJS := playgame.o boardwithkb.o player.o ship.o 

EXECUTABLE := boardgen

DEFS := 
CFLAGS := -g -O3 -std=c99

INCLUDES :=
LDFLAGS := -lm 

CC := gcc #mpiicc

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $@ $(LDFLAGS)

clean:
	rm -f *.o $(EXECUTABLE)


