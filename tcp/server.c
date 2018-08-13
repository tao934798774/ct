#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

void ProcessConnect(int new_sock) {
  while (1) {
    // 1. 尝试从客户端读取数据
    char buf[1024] = {0};
    // read/write  recv/send
    ssize_t read_size = read(new_sock, buf, sizeof(buf) - 1);
    if (read_size < 0) {
      perror("read");
      continue;
    }
    if (read_size == 0) {
      // 读到 EOF, 对于 TCP socket 来说, 表示对端主动关闭了链接
      printf("[client %d] disconnect!\n", new_sock);
      close(new_sock);
      return;
    }
    buf[read_size] = '\0';
    printf("[client %d] say %s\n", new_sock, buf);
    // 2. 根据读取到的数据进行计算(由于我们是 echo_server, 此处
    // 的计算就没有了)
    // 3. 把计算生成的响应写回给客户端
    write(new_sock, buf, strlen(buf));
  }
}

// ./server [ip] [port]
int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage ./server [ip] [port]\n");
    return 1;
  }
  // 1. 创建文件描述符
  int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0) {
    perror("socket");
    return 1;
  }
  // 2. 绑定端口号
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = htons(atoi(argv[2]));
  int ret = bind(listen_sock, (sockaddr*)&addr, sizeof(addr));
  if (ret < 0) {
    perror("bind");
    return 1;
  }
  // 3. 监听端口号
  ret = listen(listen_sock, 5);
  if (ret < 0) {
    perror("listen");
    return 1;
  }
  // 到此处, 服务器初始化动作就完成了.
  // 接下来就可以让客户端来链接了
  printf("Server Start OK\n");

  while (1) {
    // 4. 循环的处理连接, 通过 accept 把
    // 内核中已经建立好的连接拿到用户空间代码中进行处理
    sockaddr_in peer;
    socklen_t len = sizeof(peer);
    int new_sock = accept(listen_sock, (sockaddr*)&peer, &len);
    if (new_sock < 0) {
      perror("accept");
      continue;
    }
    printf("[client %d] connect\n", new_sock);

    // 5. 使用 new_sock 完成数据的读写
    ProcessConnect(new_sock);
  }
  close(listen_sock);
  return 0;
}
