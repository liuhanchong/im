#ifndef DATAINFACE_H
#define DATAINFACE_H

typedef struct im 
{
	int servfd;/*保存服务器套接字*/
	struct reactor *reactor;/*反应堆模型*/
	struct thread *acceptthread;//接收sock线程
} im;

#ifdef __cplusplus
extern "C"
{
#endif

void *acceptconn(void *uev, void *data);
void *readwrite(void *event, void *arg);
 
#ifdef __cplusplus
}
#endif

#endif