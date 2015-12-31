#ifndef SOCKET_H
#define SOCKET_H

/*创建socket*/
int socket(int domain, int socktype, int protocol);

/*将本机字节序转换为网络*/
uint32_t htonl(uint32_t host);

/*将网络转本机*/
uint32_t ntohl(uint32_t net);

/*将本机字节序转换为网络*/
uint16_t htons(uint16_t host);

/*将网络转本机*/
uint16_t ntohs(uint16_t net);

/*将ip转换为网路字节序列*/
int iptonet(int domain, const char *ip, void *addr);

/*将网路序列转换为ip*/
const char *nettoip(int domain, const void *addr, char *desaddr, size_t len);

/*设置addr*/
int setsockaddrin(sockaddr_in *sockaddr, sa_family_t family, uint16_t port, char *ip);

int getipaddrinfo(const char *host, const char *server,
					 const struct addrinfo *hint, struct addrinfo **result);

#endif