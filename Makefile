all:
	gcc Client.c -o Client.bin
	gcc Server.c -o Server.bin
clean:
	rm -rf *.bin
