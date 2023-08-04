#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void forward2server(int fd, int sendfd);
void forward2client(int fd, int sendfd);
void read_requesthdrs(rio_t *rio);
int parse_uri(char **uri, char *host);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);
void *proxythread(void *fd);

int main(int argc, char **argv)
{
    int listenfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    /* Check command line args */
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // proxy本地监听
    listenfd = Open_listenfd(argv[1]);
    clientlen = sizeof(clientaddr);
    while (1)
    {
        memset(&clientaddr, 0, sizeof(clientaddr));
        int *connfd = (int *)Malloc(sizeof(int));
        *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // line:netp:Proxy:accept

        // create threads
        if (connfd)
            Pthread_create(&tid, NULL, proxythread, connfd);
    }
}
/* $end Proxymain */

/*
 * proxythread - thread routine
 */
void *proxythread(void *fd)
{
    int connfd = *((int *)fd);
    free(fd);
    int proxysocket;
    struct sockaddr_in server_address;

    Pthread_detach(pthread_self());
    // connect to the tiny server
    proxysocket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxysocket == -1)
    {
        printf("Fail to create socket\n");
        exit(1);
    }

    // 设置服务器地址
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(4500);

    // 连接到服务器
    if (connect(proxysocket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Error connecting to server");
        close(proxysocket);
        exit(EXIT_FAILURE);
    }

    // Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE,
    //             port, MAXLINE, 0);
    // printf("Accepted connection from (%s, %s)\n", hostname, port);
    forward2server(connfd, proxysocket); // line:netp:Proxy:forward2server
    forward2client(proxysocket, connfd);
    Close(connfd); // line:netp:Proxy:close
    Close(proxysocket);
    return NULL;
}

/*
 * forward2server - handle one HTTP request/response transaction
 */
/* $begin forward2server */
void forward2server(int fd, int sendfd)
{
    int if_host;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], hostname[MAXLINE - 10], *newuri = uri;
    int host_flag = 0, user_agent_flag = 0, connection_flag = 0, proxy_connection_flag = 0;

    rio_t rio;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE)) // line:netp:forward2server:readrequest
        return;
    /* separate hostname from uri*/
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version); // line:netp:forward2server:parserequest
    if_host = parse_uri(&newuri, hostname);        // line:netp:forward2server:staticcheck
    if (if_host == -1)
        return;
    sprintf(buf, "%s %s %s\r\n", method, newuri, "HTTP/1.0");
    printf("alter:%s", buf);
    Rio_writen(sendfd, buf, strlen(buf));

    Rio_readlineb(&rio, buf, MAXLINE);

    while (strcmp(buf, "\r\n"))
    {
        printf("%s", buf);
        if (!host_flag && strstr(buf, "Host:"))
        {
            host_flag = 1;
            Rio_writen(sendfd, buf, strlen(buf));
        }
        else if (!user_agent_flag && strstr(buf, "User-Agent:"))
        {
            user_agent_flag = 1;
            printf("alter:%s", user_agent_hdr);
            Rio_writen(sendfd, user_agent_hdr, strlen(user_agent_hdr));
        }
        else if (!proxy_connection_flag && strstr(buf, "Proxy-Connection:"))
        {
            proxy_connection_flag = 1;
            sprintf(buf, "Proxy-Connection: close\r\n");
            printf("alter:%s", buf);
            Rio_writen(sendfd, buf, strlen(buf));
        }
        else if (!connection_flag && strstr(buf, "Connection:"))
        {
            connection_flag = 1;
            sprintf(buf, "Connection: close\r\n");
            printf("alter:%s", buf);
            Rio_writen(sendfd, buf, strlen(buf));
        }
        else
        {
            Rio_writen(sendfd, buf, strlen(buf));
        }

        Rio_readlineb(&rio, buf, MAXLINE);
    }

    // 检查是否有字段没有出现
    if (!host_flag)
    {
        sprintf(buf, "Host: %s\r\n", hostname);
        printf("add:%s", buf);
        Rio_writen(sendfd, buf, strlen(buf));
    }
    if (!user_agent_flag)
    {
        printf("add:%s", user_agent_flag);
        Rio_writen(sendfd, user_agent_hdr, strlen(user_agent_hdr));
    }
    if (!connection_flag)
    {
        sprintf(buf, "Connection: close\r\n");
        printf("add:%s", buf);
        Rio_writen(sendfd, buf, strlen(buf));
    }
    if (!proxy_connection_flag)
    {
        sprintf(buf, "Proxy-Connection: close\r\n");
        printf("add:%s", buf);
        Rio_writen(sendfd, buf, strlen(buf));
    }
    sprintf(buf, "\r\n");
    printf("%s", buf);
    Rio_writen(sendfd, buf, strlen(buf));
}
/* $end forward2server */

/*
 * forward2client - handle one HTTP request/response transaction
 */
/* $begin forward2client */
void forward2client(int fd, int sendfd)
{
    char buf[MAXLINE];
    rio_t rio;
    int n;
    Rio_readinitb(&rio, fd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
    {
        Rio_writen(sendfd, buf, n);
    }
    Rio_writen(sendfd, buf, n);
}

/*
 * parse_uri - parse URI into filename and CGI args
 *             return 0 if dynamic content, 1 if static
 */
/* $begin parse_uri */
int parse_uri(char **uri, char *host)
{
    if ((*uri)[0] == '/')
        return 1;
    else
    {
        char *ptr = strstr(*uri, "http://");
        if (ptr)
        {
            ptr += 7;
            char *ptr2 = strstr(ptr, "/");
            *uri = ptr2;
            strncpy(host, ptr, ptr2 - ptr);
            host[ptr2 - ptr] = '\0';
            return 0;
        }
        else
        {
            unix_error("parse_uri error");
            return -1;
        }
    }
}
/* $end parse_uri */

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg)
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Proxy Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor="
                 "ffffff"
                 ">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Proxy Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */
