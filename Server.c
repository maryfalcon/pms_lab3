#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define PORTNO 5555
#define BUFSIZE 256
#define LISTEN_QUEUE_SIZE 5
#define MAX_THREAD_COUNT 5

void* thread_function(void *param)
{
	char filepath[256];
	unsigned char buffer[BUFSIZE];
	int bytesWrite = 0, bytesRead = 0, client_id = (int)param;
	FILE *file;
	memset(buffer, 0, sizeof(buffer));
	bytesRead = read(client_id, filepath, sizeof(filepath) - 1);
	if (bytesRead == -1) {
		printf("Filepath wasn't read.\n");
		close(client_id);
		return 1;
	}
	file = fopen(filepath, "rb");
	if (file == NULL) {
		printf("File wasn't opened.\n");
		close(client_id);
		return 1;
	}
	while (1) {
		bytesRead = fread(buffer, 1, BUFSIZE, file);

		if (bytesRead > 0) {
			bytesWrite = write(client_id, buffer, bytesRead);
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
	fclose(file);
	close(client_id);
}
int main()
{

	int socket_id = 0 , client_id = 0;
	struct sockaddr_in serv_addr;
#ifdef THREAD	
	pthread_t threads[MAX_THREAD_COUNT] = { NULL };
	int i = 0;
#endif

	if ((socket_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket wasn't bind.\n");
		return 1;
	}
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORTNO);

	if ((bind(socket_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1) {
		printf("Socket wasn't bound.\n");
		return 1;
	}

	if (listen(socket_id, LISTEN_QUEUE_SIZE) == -1) {
		printf("Listening failed.\n");
		return 1;
	}

	while (1) {
		client_id = accept(socket_id, NULL, (socklen_t *)SOCK_CLOEXEC);

		if (client_id < 0) {
			printf("Client wasn't accepted.\n");
			continue;
		}

#ifdef THREAD
		for (i = 0; i < MAX_THREAD_COUNT; i++)
			if ((threads[i] == NULL) || (pthread_kill(threads[i], 0) != ESRCH))
				break;

		if (i >= MAX_THREAD_COUNT) {
			printf("No free threads.\n");
			continue;
		}

		if (pthread_create(&threads[i], NULL, thread_function, (void*)client_id)) {
			printf("Thread wasn't created.\n");
			continue;
		}
		printf("Thread was created.\n");
#else
		thread_function((void*)client_id);
#endif		
	}

	return 0;
}
