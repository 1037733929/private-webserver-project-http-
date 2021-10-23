#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>


class tcpserver
{
public:
	int m_listenfd;
	int m_clienfd;

	tcpserver(){
		m_listenfd = 0;
		m_clienfd = 0;
	};

	bool initserver(int port);

	bool Accept();

	int Send(const void *buf,const int buflen);

	int Recv(void *buf,const int buflen);
	
	~tcpserver();
};

int main()
{
	tcpserver server;
	
	if(server.initserver(5051) == false)
	{
		std::cout<<"\ntcpserver(5051) was fail -> exit..\n" << std::endl;
	}

	if(server.Accept() == false)
	{
		std::cout << "\nAccept was fail -> exit.\n" << std::endl;
		return -1;
	}
	
	std::cout << "\nConnecting!\n" << std::endl;

	char strbuffer[1024];	

	while(1)
	{
		memset(strbuffer,0,sizeof(strbuffer));
		if(server.Recv(strbuffer,sizeof(strbuffer))<=0) break;
		std::cout << "Recv OK!\n" << std::endl;

		strcpy(strbuffer,"ok");
		if(server.Send(strbuffer,sizeof(strbuffer))<=0) break;
		std::cout << "Send OK!\n" << std::endl;
	}	

	std::cout << "Connected!" << std::endl;

	return 0;
}

tcpserver::~tcpserver()
{
	if (m_listenfd!=0) close(m_listenfd);  // 关闭监听socket，释放资源
	if (m_clienfd!=0) close(m_clienfd);  // 关闭客户连接socket，释放资源
}

bool tcpserver::initserver(int port)
{
	m_listenfd = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	//绑定监听socket
	if(bind(m_listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
	{
		close(m_listenfd);
		m_listenfd = 0;
		return false;
	}
	//监听socket
	if (listen(m_listenfd,5) != 0 )
	{ 
		close(m_listenfd); 
		m_listenfd=0; 
		return false; 
	}
	return true;
}


bool tcpserver::Accept()
{	
	if((m_clienfd = accept(m_listenfd,0,0)) <=0) return false;
	return true;
}

int tcpserver::Recv(void *buf,const int buflen)
{
	return recv(m_clienfd,buf,buflen,0);	 
}

int tcpserver::Send(const void *buf,const int buflen)
{
	return send(m_clienfd,buf,buflen,0);
}
