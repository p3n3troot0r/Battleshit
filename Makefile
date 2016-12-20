OBJS := playgame.o boardwithkb.o player.o ship.o 

EXECUTABLE := boardgen

DEFS := -DPARALLEL
CFLAGS := -g -O3 -std=c99

INCLUDES :=
LDFLAGS := -lm 

CC := mpiicc # gcc

%.o: %.c %.h
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -c $< -o $@

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) $(OBJS) -o $@ $(LDFLAGS)

clean:
	rm -f *.o $(EXECUTABLE)


