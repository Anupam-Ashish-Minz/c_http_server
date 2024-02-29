#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3000
#define READ_BUF_SIZE 1024

const char *http_format = "HTTP/1.1 200 OK\r\n"
						  "Content-Type: text/plain\r\n"
						  "Connection: close\r\n"
						  "Content-Length: %d\r\n"
						  "\r\n %s\r\n";
int digits(int n) {
	if (n == 0) {
		return 1;
	}
	int count = 0;
	while (n != 0) {
		n = n / 10;
		++count;
	}
	return count;
}

void *request_handler(void *vargp) {
	int client_fd = *(int *)vargp;
	const char *content = "fixed message from server";
	int content_len = strlen(content) + 3;
	char *http_msg =
		(char *)malloc(strlen(http_format) + content_len + digits(content_len));
	char buf[READ_BUF_SIZE];
	unsigned int read_len;

	sprintf(http_msg, http_format, content_len, content);

	do {
		read_len = read(client_fd, buf, READ_BUF_SIZE);
		if (read_len < READ_BUF_SIZE) {
			buf[read_len] = '\0';
		}
		printf("%s", buf);
	} while (read_len == READ_BUF_SIZE);
	printf("\n");

	write(client_fd, http_msg, strlen(http_msg));

	return 0;
}

int main() {
	int socket_fd;
	int client_fd;
	int opt = 1;
	struct sockaddr_in socket_addr;
	socklen_t addrlen;

	socket_addr.sin_addr.s_addr = INADDR_ANY;
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(PORT);

	addrlen = sizeof(socket_addr);

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("failed to init socket");
		return -1;
	}
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
			   sizeof(opt));
	if (bind(socket_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) <
		0) {
		perror("failed to bind socket");
		return -1;
	}
	printf("socket binding successful\n");

	if (listen(socket_fd, 10) < 0) {
		perror("listening failed");
		return -1;
	}
	printf("socket listening on http://127.0.0.1:%d\n", PORT);

	pthread_t thread;
	while ((client_fd = accept(socket_fd, (struct sockaddr *)&socket_addr,
							   &addrlen)) > 0) {
		pthread_create(&thread, NULL, &request_handler, (void *)&client_fd);
		// request_handler(client_fd);
	}

	return 0;
}
