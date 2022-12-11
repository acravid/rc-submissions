# Makefile Computer Networks (Group 91) 

# Compiler and Flags
CC=gcc
CFLAGS=


.PHONY: all clean 

#GServer
all: GPlayer 
GPlayer:client/player.c client/commands_tcp.o client/commands_udp.o 

GPlayer: 
	$(CC) $(CFLAGS) $^ -o $@


clean: 
	@rm -rf  */.o GPlayer


