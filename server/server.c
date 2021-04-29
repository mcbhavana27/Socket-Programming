#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> // for close
#define PORT 8080
#define BUFSIZE 20
#define SIZE_BUFF 10000
int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char status[15] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server Waiting\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int count;
    int val1 = recv(new_socket, &count, sizeof(count), 0);
    printf("count %d\n", count);
    int j = 1;
    while (j < count)
    {
        j++;

        char xx[1024] = {0};
        int val;
        val = read(new_socket, xx, 1024);
        printf("file received %s\n", xx);
        int fd = open(xx, O_RDONLY);
        int flag;

        if (fd == -1)
        {
            flag = 0;
            perror(" ");
            send(new_socket, &flag, sizeof(flag), 0);
        }
        else
        {
            flag = 1;
            send(new_socket, &flag, sizeof(flag), 0);
            struct stat s;
            stat(xx, &s);
            int cnt = s.st_size;
            int n = cnt / SIZE_BUFF;
            //printf("%d\n",n);
            int pe = 0;
            send(new_socket, &n, sizeof(n), 0);
            int m = cnt % SIZE_BUFF;
            send(new_socket, &m, sizeof(m), 0);
            printf("%d\n", m);
            lseek(fd, 0, SEEK_SET);
            for (int p = 0; p < n; p++)
            {
                //cnt = lseek(fd, p * SIZE_BUFF, SEEK_SET);
                char *arr = (char *)calloc(1000005, sizeof(char));
                read(fd, arr, SIZE_BUFF);
                //printf("%s\n", arr);
                // pe++;
                send(new_socket, arr, strlen(arr), 0);
                free(arr);
            }
            if (m)
            {
                char *arr = (char *)calloc(1000005, sizeof(char));
                read(fd, arr, m);
                //printf("%s\n", arr);
                send(new_socket, arr, strlen(arr), 0);
                free(arr);
            }
        }
    }

    close(new_socket);
    return 0;
}
