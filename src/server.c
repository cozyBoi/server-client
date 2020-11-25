#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "threadpool.h"

#define PORT 8080
#define QUEUE  256

threadpool_t *pool;
int tasks = 0, done = 0;
pthread_mutex_t lock;

void *workFunc(void *argu);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int workerNum;
    
    pthread_mutex_init(&lock, NULL);
    printf("workerNum : ");
    scanf("%d", &workerNum);
    
    //thread pool 할당
    assert((pool = threadpool_create(workerNum, QUEUE, 0)) != NULL);
    fprintf(stderr, "Pool started with %d threads and queue size of %d\n", workerNum, QUEUE);
    
    //소켓 할당
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    //소켓 바인딩 (소켓에 주소를 할당하는 작업)
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    
    //클라이언트와 연결하는 거 같음.
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        //만약 쓸수있으면 워커에 넣음 아니면 wait
        while(threadpool_add(pool, workFunc, &new_socket, 0) == 0) {
            pthread_mutex_lock(&lock);
            tasks++;
            pthread_mutex_unlock(&lock);
        }
    }
    return 0;
}

void *workFunc(void *argu){
    pthread_t id = pthread_self();
    //printf("thread ID (TID) :: %lu\n", id);
    int new_socket = *(int*)argu;
    char buffer[2048] = {0};
    read( new_socket , buffer, 2048);
    printf("TID %lu : get msg %s\n", id, buffer);
    close(new_socket);
    return 0;
}
