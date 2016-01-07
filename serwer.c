#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>

int indexOf_shift(char* base, char* str, int startIndex)
{
	int result;
	int baselen = strlen(base);
	if (strlen(str) > baselen || startIndex > baselen)
	{
		result = -1;
	}
	else
	{
		if (startIndex < 0)
		{
			startIndex = 0;
		}
		char* pos = strstr(base+startIndex, str);
		if (pos == NULL)
		{
			result = -1;
		}
		else
		{
			result = pos - base;
		}
	}
	return result;
}

void childend(int signo)
{
	wait(NULL);
	printf("*** End of child process\n");
}


struct cln
{
	int cfd;
	struct sockaddr_in caddr;
};

struct clients
{
	struct cln* c1;
	struct cln* c2;
};


void* transmitter(void* arg)
{
	struct clients* cs = (struct clients*)arg;
	printf("new connection: %s\n", inet_ntoa((struct in_addr)cs->c1->caddr.sin_addr));
	char buf[256];
	while(1)
	{
		read(cs->c1->cfd, buf, sizeof(buf));
		write(cs->c2->cfd, buf, sizeof(buf));
	}
	//close(c->cfd);
	//free(c);
	return 0;
}

void* messageResolver(void* arg)
{
	struct cln* client = (struct cln*)arg;
	printf("new connection: %s\n", inet_ntoa((struct in_addr)client->caddr.sin_addr));
	char message[256];
	while(1)
	{
		read(client->cfd, message, sizeof(message));
		printf("%s\n", message);
		int firstIndex = indexOf_shift(message, ":", 0);
		int lastIndex = indexOf_shift(message, ":", firstIndex+1);
		char code[10];
		memcpy(code, &message[firstIndex+1], lastIndex-firstIndex);
		code[lastIndex-firstIndex-1] = '\0';
		printf("%s\n", code);	//Tutaj będziemy odpalać odpowiednie czynności w zależności od zmiennej code
	}
}

int main()
{
	pthread_t tid1;
	pthread_t tid2;
	socklen_t slt1, slt2;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int on = 1;

	signal(SIGCHLD, childend);

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1234);
	sa.sin_addr.s_addr = INADDR_ANY;
	
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));	//wazna linia!
	bind(fd, (struct sockaddr*)&sa, sizeof(sa));
	listen(fd,10);
	
	while(1)
	{
		struct cln* c1 = malloc(sizeof(struct cln));
		slt1 = sizeof(c1->caddr);
		//struct cln* c2 = malloc(sizeof(struct cln));
		//slt2 = sizeof(c2->caddr);
		c1->cfd = accept(fd, (struct sockaddr*)&c1->caddr, &slt1);
		//c2->cfd = accept(fd, (struct sockaddr*)&c2->caddr, &slt2);
		//struct clients* cs1 = malloc(sizeof(struct clients));
		//cs1->c1 = c1;
		//cs1->c2 = c2;
		//struct clients* cs2 = malloc(sizeof(struct clients));
		//cs2->c1 = c2;
		//cs2->c2 = c1;
		pthread_create(&tid1, NULL, messageResolver, c1);
		pthread_detach(tid1);
		//pthread_create(&tid2, NULL, transmitter, cs2);
		//pthread_detach(tid2);
	}
	close(fd);
	return 0;
}
