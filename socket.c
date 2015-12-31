#include "socket.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*将本机字节序转换为网络*/
uint32_t htonl(uint32_t host)
{
	return htonl(host);
}

/*将网络转本机*/
uint32_t ntohl(uint32_t net)
{
	return ntohl(net);
}

/*将本机字节序转换为网络*/
uint16_t htons(uint16_t host)
{
	return htons(host);
}

/*将网络转本机*/
uint16_t ntohs(uint16_t net)
{
	return ntohs(net);
}

/*创建socket*/
int socket(int domain, int socktype, int protocol)
{
	return socket(domain, socktype, protocol);
}

/*将ip转换为网路字节序列*/
int iptonet(int domain, const char *ip, void *addr)
{
	return inet_pton(domain, ip, addr);
}

/*将网路序列转换为ip*/
const char *nettoip(int domain, const void *addr, char *desaddr, size_t len)
{
	return inet_ntop(domain, addr, desaddr, len);
}

int setsockaddrin(sockaddr_in *sockaddr, sa_family_t family, uint16_t port, char *ip)
{
	sockaddr->sin_family = family;
	sockaddr->sin_port = htons(port);
	if (iptonet(family, ip, &sockaddr->sin_addr) <= 0)
	{
		return FAILED;
	}

	return SUCCESS;
}

int getipaddrinfo(const char *host, const char *server,
				 	const struct addrinfo *hint, struct addrinfo **result)
{
	return getaddrinfo(host, server, hint, result);
}


