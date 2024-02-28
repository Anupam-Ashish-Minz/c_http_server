#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

#define PORT 3000

int main() {
	int socket_fd;
	int recv_fd;
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

	if ((recv_fd = accept(socket_fd, (struct sockaddr *)&socket_addr,
						  &addrlen)) < 0) {
		perror("failed to accept connect from client");
		return -1;
	}

	return 0;
}
