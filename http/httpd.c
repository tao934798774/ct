#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<assert.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sendfile.h>

#define MAX 1024
#define HOME_PAGE "index.html"
#define PAGE_404 "wwwroot/404.html"

static void usage(const char *proc){
    printf("\nUsage:\n\t%s port\n\n",proc);
}

int get_line(int sock,char line[],int num){//line-> \r,\r\n,\n->\n
    assert(line);
    assert(num > 0);

    int i = 0;
    char c = 'A';
    while(c != '\n' && i < num - 1){
        ssize_t s = recv(sock,&c,1,0);
        if(s > 0){
            //\r,\r\n -> \n
            if(c == '\r'){
                recv(sock,&c,1,MSG_PEEK);
                if(c == '\n'){
                    recv(sock,&c,1,0);
                }
                else{
                    c = '\n';
                }
            }
            //else
            //\n,non \n
            line[i++] = c;
        }
    }
    line[i] = '\0';
    return i;

}

void clear_header(int sock){
    char line[MAX];
    do{
        get_line(sock,line,sizeof(line));
        //printf("%s",line);
    }while(strcmp(line,"\n"));
}

void show_404(int sock){
    char line[1024];
    sprintf(line,"HTTP/1.0 404 Not Found\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"Content-Type:text/html;charset-ISO-8859-1\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);

    struct stat st;
    stat(PAGE_404,&st);
    int fd = open(PAGE_404,O_RDONLY);
    sendfile(sock,fd,NULL,st.st_size);
    close(fd);
}

void echo_error(int sock,int code){
    switch(code){
    case 400:
        //show_400();
        break;
    case 403:
        //show_403();
        break;
    case 404:
        show_404(sock);
        break;
    case 500:
        //show_500();
        break;
    case 503:
        //show_503();
        break;
    default:
        break;
    }
}

int echo_www(int sock,char *path,int size){
    int fd = open(path,O_RDONLY);
    if(fd < 0){
        return 404;
    }
    clear_header(sock);
    char line[MAX];
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"Content-Type:text/html;charset-ISO-8859-1\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);
    
    sendfile(sock,fd,NULL,size);

   close(fd);
   return 200;
}

int exe_cgi(int sock,char *method,char *path,char *query_string){
    char line[MAX];
    char method_env[MAX/10];
    char query_string_env[MAX];
    char content_length_env[MAX/10];
    int content_length = -1;
    if(strcasecmp(method,"GET") == 0){
        clear_header(sock);
    }else{//POST
        do{
            get_line(sock,line,sizeof(line));
            //Content-Length: 12
            if(strncmp(line,"Content-Length: ",16) == 0){
                content_length = atoi(line + 16);
            }
        }while(strcmp(line,"\n"));
        if(content_length == -1){
            return 400;
        }
    }
    
    int input[2];//child 角度
    int output[2];

    pipe(input);
    pipe(output);
    //path
    pid_t id = fork();
    if(id < 0){
        return 503;
    }else if(id == 0){//child
        close(sock);
        close(input[1]);
        close(output[0]);
        
        sprintf(method_env,"METHOD=%s",method);
        putenv(method_env);
        
        if(strcasecmp(method,"GET") == 0){
            sprintf(query_string_env,"QUERY_STRING=%s",query_string);
            putenv(query_string_env);
        }else{
            sprintf(content_length_env,"CONTEBT_LENGTH=%d",content_length);
            putenv(content_length_env);
        }

        dup2(input[0],0);
        dup2(output[1],1);
        //exec*
        execl(path,path,NULL);    
        exit(1);
    }else{
        close(input[0]);
        close(output[1]);

        char c;
        if(strcasecmp(method,"POST") == 0){
            int i = 0;
            for(;i < content_length;i++){
            recv(sock,&c,1,0);
            write(input[1],&c,1);
            }
        }

        sprintf(line,"HTTP/1.0 200 OK\r\n");
        send(sock,line,strlen(line),0);
        sprintf(line,"Content-Type:text/html;charset=ISO-8859-1\r\n");
        send(sock,line,strlen(line),0);
        sprintf(line,"\r\n");
        send(sock,line,strlen(line),0);
        
        while(read(output[0],&c,1) > 0){
            send(sock,&c,1,0);
        }
        waitpid(id,NULL,0);
    }
    return 200;
}

void *handler_request(void *arg){
    int sock = (int)arg;
    char line[MAX];
    char method[MAX/10];
    char url[MAX];
    char path[MAX];
    int i = 0;
    int j = 0;
    int status_code = 200;
    int cgi = 0;
    char *query_string = NULL;

    get_line(sock,line,sizeof(line));//line-> \r,\r\n,\n->\n
    //GET a/b/c?name=zhangsan&sex=man HTTP/1.1
    while(i < sizeof(method) - 1 && j < sizeof(line) && !isspace(line[j])){
        method[i] = line[j];
        i++;
        j++;
    }
    method[i] = '\0';
    if(strcasecmp(method,"GET") == 0){
        
    } 
    else if(strcasecmp(method,"POST") == 0){
        
    }else{
        clear_header(sock);
        status_code = 400;
        goto end;
    }
    
    //GET /a/b/c?name=zhanasan&sex=man HTTP/1.1
    while(j < sizeof(line) && isspace(line[j])){
        j++;
    }
    i = 0;
    while(i < sizeof(url) - 1 && j < sizeof(line) && !isspace(line[j])){
        url[i] = line[j];
        i++,j++;
    }
    url[i] = '\0';

#ifdef DEBUG
    printf("line: %s\n",line);
    printf("method: %s,url: %s\n",method,url);
#endif
    //method,url,cgi
    if(strcasecmp(method,"GET") == 0){
        query_string = url;
        while(*query_string){
            if(*query_string == '?'){
                *query_string = '\0';
                query_string++;
                cgi = 1;
                break;
            }
            query_string++;
        }
    }
    //method GET/POST,url,GET(query_string),cgi
    sprintf(path,"wwwroot%s",url);//path(wwwroot/,wwwroot/a/b/c.html)
    if(path[strlen(path)-1] == '/'){
        strcat(path,HOME_PAGE);
    }

    struct stat st;
    if(stat(path,&st) < 0){
        clear_header(sock);
        status_code = 404;
        goto end;
    }else{
        if(S_ISDIR(st.st_mode)){
            strcat(path,HOME_PAGE);
        }else if((st.st_mode & S_IXUSR) ||\
                 (st.st_mode & S_IXGRP) ||\
                 (st.st_mode & S_IXOTH)){
            cgi = 1;       
        }else{
            //DO NOTHING
        }

        if(cgi){
            status_code = exe_cgi(sock,method,path,query_string);
        }else{
            status_code = echo_www(sock,path,st.st_size);
        }
    }
end:
    if(status_code != 200){
        echo_error(sock,status_code);
    }
    //printf("%s",line);
    close(sock);
}

int startup(int port){
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0){
        perror("socket");
        exit(2);
    }

    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(port);

    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0){
        perror("bind");
        exit(3);
    }
    if(listen(sock,5) < 0){
        perror("listen");
        exit(4);
    }
    return sock;
}

//./httpd 80
int main(int argc,char *argv[]){
    if(argc != 2){
        usage(argv[0]);
        return 1;
    }
    int listen_sock = startup(atoi(argv[1]));

    for(;;){
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int sock = accept(listen_sock,(struct sockaddr*)&client,&len);
        if(sock < 0){
            perror("accept");
            continue;
        }
        printf("get a new client!\n");

        pthread_t tid;
        pthread_create(&tid,NULL,handler_request,(void*)sock);
        pthread_detach(tid);
    }
}
