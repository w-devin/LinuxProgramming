#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int createListenfd()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int n = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &n, 4);

    struct sockaddr_in sin;
    bzero(&sin, sizeof(sin));

    
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = INADDR_ANY;

    int r = bind(fd, (struct sockaddr *)&sin, sizeof(sin));

    if(r == -1)
    {
        perror("bind errer");
        exit(-1);
    }

    //listen start
    r = listen(fd, 10);
    if(r == -1)
    {
        perror("listen errer");
        exit(-1);
    }

    printf("return fd");

    return fd;
}

void work(int fd, char *s)
{
    //analysis
    char fileName[100] = {0};

    sscanf(s, "GET /%s", fileName);
    if(strcmp(fileName, ""))
        strcpy(fileName, "index.html");
    printf("the file name is %s \n", fileName);

    //mime 
    char *mime;
    if(strstr(fileName, ".html"))
        mime = "text/html";
    else if(strstr(fileName, ".jpg"))
        mime = "image/jpg";

    //response head
    char response[100] = {0};
    sprintf(response, "HTTP/1.1 200 ok\r\nContent-Type:%s\r\n\r\n", mime);
    write(fd, response, strlen(response));

    //send file to client
    int filefd = open(fileName, O_RDONLY);
    bzero(response, sizeof(response));

    while(1)
    {
        int rlen = read(filefd, response, 100);
        if(rlen > 0)
            write(fd, response, rlen);
        else break;
    }
    close(fd);
}

int main()
{
    int sockfd = createListenfd();

    while(1)
    {
        int fd = accept(sockfd, NULL, NULL);

        char buffer[1024] = {0};

        int rlen = read(fd, buffer, sizeof(buffer));
        printf("server request: %s\n", buffer);

        work(fd, buffer);

    }
        close(sockfd);
}
