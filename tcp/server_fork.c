//server.c 只能连一个客户端，serverfork可以连接多个客户端
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

int ServerInit(char* ip, short port) {
  int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0) {
    perror("socket");
    return -1;
  }
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);
  int ret = bind(listen_sock, (sockaddr*)&addr, sizeof(addr));
  if (ret < 0) {
    perror("bind");
    return -1;
  }
  ret = listen(listen_sock, 5);
  if (ret < 0) {
    perror("listen");
    return -1;
  }
  return listen_sock;
}

void ProcessConnect(int new_sock) {
  // 1. 创建子进程
  pid_t ret = fork();
  if (ret < 0) {
    perror("fork");
    return;
  }
  if (ret == 0) {
    // 2. 子进程进行循环读写 socket 数据
    while (1) {
      char buf[1024] = {0};
      ssize_t read_size = read(new_sock, buf, sizeof(buf) - 1);
      if (read_size < 0) {
        perror("read");
        exit(1);
      }
      if (read_size == 0) {
        printf("[client %d] disconnected!\n", new_sock);
        close(new_sock);
        exit(0);
      }
      buf[read_size] = '\0';

      printf("[client %d] say %s\n", new_sock, buf);

      // 把数据写回给客户端
      write(new_sock, buf, strlen(buf));
    }
    exit(0);
  }
  // 3. 父进程能够快速再次调用到 accept
  //  a) 关闭 new_sock
  close(new_sock);
  //  b) 回收子进程, 使用忽略信号的方式来完成.
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage ./server [ip] [port]\n");
    return 1;
  }
  signal(SIGCHLD, SIG_IGN);
  // 1. 服务器初始化
  int listen_sock = ServerInit(argv[1], atoi(argv[2]));
  if (listen_sock < 0) {
    printf("ServerInit failed\n");
    return 1;
  }
  printf("ServerInit OK\n");
  while (1) {
    // 2. 循环进行 accept
    sockaddr_in peer;
    socklen_t len = sizeof(peer);
    int new_sock = accept(listen_sock, (sockaddr*)&peer, &len);
    if (new_sock < 0) {
      perror("accept");
      continue;
    }
    printf("[client %d] connect!\n", new_sock);
    ProcessConnect(new_sock);
  }
  return 0;
}

