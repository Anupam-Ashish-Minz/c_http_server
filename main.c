#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3000
#define READ_BUF_SIZE 65535 // as 64K is the max size of tcp packet

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

void parse_http_request(char *http_request, size_t len) {
	int prev_split = 0;
	char **splits = (char **)malloc(100);
	int split_index = 0;

	for (int i = 0; i < len; i++) {
		if (http_request[i] == ' ') {
			splits[split_index] = (char *)malloc(i + 1);
			strncpy(splits[split_index], &http_request[prev_split], i);
			split_index++;
			prev_split = i + 1;
		}
	}
	char *http_method;
	http_method = splits[0];
	printf("http method is : %s\n", http_method);
	if (strlen(http_method) == 3 && strncmp(http_method, "GET", 3)) {
		// get_request_handler();
	} else if (strlen(http_method) == 4 && strncmp(http_method, "POST", 4)) {
		// post_request_handler();
	}
}

void *request_handler(void *vargp) {
	int client_fd = *(int *)vargp;
	const char *content = "previous message";
	int content_len = strlen(content) + 3;
	char *http_msg =
		(char *)malloc(strlen(http_format) + content_len + digits(content_len));
	char buf[READ_BUF_SIZE];
	unsigned int read_len;

	sprintf(http_msg, http_format, content_len, content);

	char *http_request = (char *)malloc(sizeof(char) * READ_BUF_SIZE);
	do {
		read_len = read(client_fd, buf, READ_BUF_SIZE);
		strncpy(http_request, buf, read_len);
	} while (read_len == READ_BUF_SIZE);
	// printf("%s\n", http_request);
	parse_http_request(http_request, strlen(http_request));

	write(client_fd, http_msg, strlen(http_msg));
	close(client_fd);

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
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
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

	// assume max concurrent requsets
	pthread_t threads[100];
	int count = 0;
	while (1) {
		client_fd =
			accept(socket_fd, (struct sockaddr *)&socket_addr, &addrlen);
		// TODO check for thread limits later
		pthread_create(&threads[count++ % 100], NULL, &request_handler,
					   (void *)&client_fd);
		// pthread_join(threads[count % 100], NULL);
	}

	return 0;
}
