# Makefile Computer Networks (Group 91) 

# Compiler and Flags
CC=gcc
CFLAGS =-W -std=gnu99 -I../

all: client GS

#client compilation
client:  client/commands/commands.o client/player.o
	$(CC) $(CFLAGS) -o player client/commands/commands.o client/player.o

client/commands.o: client/player.h client/commands/commands.c client/commands/commands.h 
	$(CC) $(CFLAGS) -o client/commands/commands.o -c client/commands/commands.c

client/player.o: client/commands/commands.h client/player.c client/player.h
	$(CC) $(CFLAGS) -o client/player.o -c client/player.c


#Server compilation
GS: server/data/data_handler.o server/requests/requests.o  server/GS.o
	$(CC) $(CFLAGS) -o GS server/data/data_handler.o server/requests/requests.o server/GS.o

server/data/data_handler.o: server/data/data_handler.c server/data/data_handler.h
	$(CC) $(CFLAGS) -o server/data/data_handler.o -c server/data/data_handler.c

server/requests.o: server/GS.h server/data/data_handler.data_handler server/requests/requests.c server/requests/requests.h 
	$(CC) $(CFLAGS) -o server/requests/requests.o -c server/requests/requests.c
 
server/GS.o: server/requests/request.h server/GS.c server/GS.h 
	$(CC) $(CFLAGS) -o server/GS.o -c server/GS.c


clean:
	rm -r client/*.o client/commands/*.o server/*.o server/requests/*.o server/data/*.o player GS GAME_DATA/ client/RECEIVED_FILES