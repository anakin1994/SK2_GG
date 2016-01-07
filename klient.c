#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


struct cln
{
	int cfd;
	struct sockaddr_in caddr;
};

void* reader(void* arg)
{
	struct cln* c = (struct cln*)arg;
	char rbuf[256];
	while(1)
	{
		read(c->cfd, rbuf, sizeof(rbuf));
		printf("%s\n", rbuf);
	}
}

int main(int argc, char** argv)
{
	pthread_t tid;
	socklen_t slt;
	char buf[256];
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sa;
	sa.sin_family = PF_INET;
	sa.sin_port = htons(1234);
	char addr[14]  = "0.0.0.0";
	sa.sin_addr.s_addr = inet_addr(addr);
	connect(fd, (struct sockaddr*)&sa, sizeof(sa));
	
	struct cln* c = malloc(sizeof(struct cln));
	slt = sizeof(c->caddr);
	c->cfd = fd;
	pthread_create(&tid, NULL, reader, c);
	pthread_detach(tid);
	while(1)
	{
		scanf("%s", &buf);
		write(fd, buf, sizeof(buf));
	}
	close(fd);

}
