#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORTNO 5555
#define BUFSIZE 256
#define LISTEN_QUEUE_SIZE 5



int main()
{
	char filepath[256];
	unsigned char buffer[BUFSIZE];
	int socketId = 0 , clientId = 0, bytesWrite = 0, bytesRead = 0;
	struct sockaddr_in serv_addr;
	FILE *file;
	
	memset(buffer, 0, sizeof(buffer));
	
	if ((socketId = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket wasn't read.\n");
		return 1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORTNO);

	if ((bind(socketId, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1) {
		printf("Socket wasn't bound.\n");
		return 1;
	}

	if (listen(socketId, LISTEN_QUEUE_SIZE) == -1) {
		printf("Error on listaning.\n");
		return 1;
	}

	while (1) {
		clientId = accept(socketId, NULL, (socklen_t *)SOCK_CLOEXEC);

		if (clientId < 0) {
			printf("Client wasn't accepted.\n");
			continue;
		}

		bytesRead = read(clientId, filepath, sizeof(filepath) - 1);
		if (bytesRead == -1) {
			printf("Filepath wasn't read.\n");
			continue;
		}

		file = fopen(filepath, "rb");
		if (file == NULL) {
			printf("File wasn't opened.\n");
			return 1;
		}

		while (1) {
			bytesRead = fread(buffer, 1, BUFSIZE, file);

			if (bytesRead > 0) {
				bytesWrite = write(clientId, buffer, bytesRead);
				if (bytesWrite < bytesRead)
					printf("File wasn't sent.\n");
			}

			if (bytesRead < BUFSIZE) {
				if (feof(file))
					printf("File was sent.\n");
				if (ferror(file))
					printf("Error reading from file.\n");
				break;
			}
		}
		close(clientId);
	}

	return 0;
}
