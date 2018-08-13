#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage ./client [ip] [port]\n");
    return 1;
  }
  // 1. 创建 socket 
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }
  // 2. 让客户端和服务器建立链接
  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));
  int ret = connect(sock, (sockaddr*)&server_addr,
              sizeof(server_addr));
  if (ret < 0) {
    perror("connect");
    return 1;
  }
  while (1) {
    // 3. 循环的尝试从标准输入读数据
    printf("> ");
    fflush(stdout);
    char buf[1024] = {0};
    ssize_t read_size = read(0, buf, sizeof(buf) - 1);
    if (read_size < 0) {
      perror("read");
      return 1;
    }
    if (read_size == 0) {
      printf("read done\n");
      return 0;
    }
    buf[read_size] = '\0';

    // 4. 把数据发送给服务器
    write(sock, buf, strlen(buf));

    // 5. 尝试从服务器读取数据
    char buf_output[1024] = {0};
    read_size = read(sock, buf_output, sizeof(buf_output) - 1);
    if (read_size < 0) {
      perror("read");
      return 1;
    }
    if (read_size == 0) {
      printf("read done\n");
      return 0;
    }
    buf_output[read_size] = '\0';

    // 6. 把读取到的结果写到标准输出上
    printf("server resp %s\n", buf_output);
  }
  close(sock);
  return 0;
}

