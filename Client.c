#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORTNO 5555
#define BUFSIZE 256

int main()
{
	char filename[256], buffer[BUFSIZE];
	int bytesRead = 0, socket_id;
	struct sockaddr_in serv_addr;
	FILE *file;
	
	if ((socket_id = socket(AF_INET, SOCK_STREAM, 0))==-1) {
		printf("Socket wasn't created.\n");
		return 1;
	}

	memset(buffer, 0, sizeof(buffer));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORTNO);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (connect(socket_id, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        	printf("Connecting to socket failed.\n");
		return 1;
	}
	printf("Input filepath on server:\n");
	scanf("%s",filename);
	if (send(socket_id, filename, strlen(filename), 0) < 0) {
		printf("Filename wasn't sent\n");
		close(socket_id);
		return 1;
	}
	
	file = fopen("output.txt", "wb");
	if (file == NULL) {
		printf("File wasn't opened.\n");
		close(socket_id);
		return 1;
	}

	while ((bytesRead = read(socket_id, buffer, BUFSIZE)) > 0) {
		fwrite(buffer, 1, bytesRead, file);
	}

	if (bytesRead < 0)
		printf("Reading file failed.\n");
	fclose(file);
	close(socket_id);
	return 0;
}
