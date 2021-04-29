#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFSIZE 20

int main(int argc, char *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    // printf("count of files... %d\n",argc);
    if (argc == -1)
        printf("No arguments\n");
    //int fy;
    //fy=open(argv[1],O_RDWR | O_CREAT | O_TRUNC,0770);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    int count = argc;
    send(sock, &count, sizeof(count), 0);
    printf("sent count  %d\n", count);
    for (int j = 1; j < argc; j++)
    {
        send(sock, argv[j], strlen(argv[j]), 0);
        printf("file sent--> %s\n", argv[j]);
        int c, x;
        int flag;
        recv(sock, &flag, sizeof(flag), 0);
        if (flag == 1)
        {

            int fy;
            fy = open(argv[j], O_RDWR | O_CREAT | O_TRUNC, 0770);
            x = recv(sock, &c, sizeof(c), 0);
            printf("%d\n", c);
            int r;
            x = recv(sock, &r, sizeof(r), 0);
            printf("rr  %d\n", r);
            for (int p = 0; p < c; p++)
            {
                char *arr = (char *)calloc(1000005, sizeof(char));
                x = read(sock, arr, 1000000);
                //printf("%s\n",arr);
                write(fy, arr, strlen(arr));
                free(arr);
            }
            if (r)
            {
                char *arr = (char *)calloc(1000005, sizeof(char));
                x = read(sock, arr, r);
                write(fy, arr, strlen(arr));
                //printf("%s\n",arr );
                free(arr);
            }

            close(fy);
        }

        else
        {
            printf("no such file\n");
        }
    }

    close(sock);
    return 0;
}
