.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra -g

all: client serveur

client: client.c protocole.h
	$(CC) $(CFLAGS) client.c -o client

serveur: serveur.c protocole.h
	$(CC) $(CFLAGS) serveur.c -o serveur

clean:
	rm -f client serveur *.o
