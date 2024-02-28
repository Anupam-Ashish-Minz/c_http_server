#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3000

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

int main() {
	int socket_fd;
	int client_fd;
	int opt = 1;
	struct sockaddr_in socket_addr;
	socklen_t addrlen;
	const char *content = "hello world";
	int content_len = strlen(content) + 3;
	char *http_format = "HTTP/1.1 200 OK\r\n"
						"Content-Type: text/plain\r\n"
						"Connection: close\r\n"
						"Content-Length: %d\r\n"
						"\r\n %s\r\n";
	char *http_msg =
		(char *)malloc(strlen(http_format) + content_len + digits(content_len));

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

	while ((client_fd = accept(socket_fd, (struct sockaddr *)&socket_addr,
							   &addrlen)) > 0) {
		sprintf(http_msg, http_format, content_len, content);
		send(client_fd, http_msg, strlen(http_msg), 0);
	}

	return 0;
}
