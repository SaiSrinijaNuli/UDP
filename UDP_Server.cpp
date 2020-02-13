#include <Windows.h>
#include <stdio.h>
#include <winsock.h>
#include <signal.h>
//#include <unistd.h>

volatile sig_atomic_t stop;

char Buffer1[512] = "Hello from server";
int iBuffer1Len = strlen(Buffer1) + 1;

void inthand(int signum) {
	stop = 1;
	strcpy_s(Buffer1, 4,"end");
}

int main(int argc,char *argv[])
{
	WSADATA WinSockData;
	int iWsaStartup;
	int iWsaCleanup;

	SOCKET UDPSocketServer;
	struct sockaddr_in UDPClient;

	char Buffer[512];
	int iBufferLen = strlen(Buffer) + 1;

	int iBind;
	int iReceiveFrom;
	int iSendto;
	int portno;

	int iUDPClientLen = sizeof(UDPClient);
	int iCloseSocket;

	if (argc < 2) {
		printf("ERROR, no sufficient arguments\n");
		return 0;
	}

	// Step-1 Initialization of Winsock

	iWsaStartup = WSAStartup(MAKEWORD(2, 2), &WinSockData);
	if (iWsaStartup != 0)
	{
		printf("WSAStartUp failed\n");
		return 0;
	}

	// Step-2 Fill the UDPClient(Socket Address) Structure

	UDPClient.sin_family = AF_INET;
	UDPClient.sin_addr.s_addr = inet_addr("127.0.0.1");
	portno = atoi(argv[1]);
	//UDPClient.sin_port = htons(portno);
	UDPClient.sin_port = htons(8001);

	// Step-3 Socket Creation

	UDPSocketServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (UDPSocketServer == INVALID_SOCKET)
	{
		printf("Socket creation failed\n");
		return 0;
	}

	// Step-4 Bind the Server

	iBind = bind(UDPSocketServer, (SOCKADDR*)&UDPClient, sizeof(UDPClient));
	if (iBind == SOCKET_ERROR)
	{
		printf("Binding failed\n");
		return 0;
	}

	// Step-5 RecvFrom from client
	int i;
	signal(SIGINT, inthand);
	stop = 0;
	while(!stop)
	{
		iReceiveFrom = recvfrom(UDPSocketServer, Buffer, iBufferLen, MSG_PEEK, (SOCKADDR*)&UDPClient, &iUDPClientLen);
		if (iReceiveFrom == SOCKET_ERROR)
		{
			printf(" Receiving failed\n");
			return 0;
		}

		printf("%s\n", Buffer);

		iSendto = sendto(UDPSocketServer, Buffer1, iBuffer1Len, MSG_DONTROUTE, (SOCKADDR*)&UDPClient, sizeof(UDPClient));
		if (iSendto == SOCKET_ERROR) {
			printf("Sending data failed\n");
			return 0;
		}
	}

	printf("came out of loop %s \n",Buffer1);
	iSendto = sendto(UDPSocketServer, Buffer1, iBuffer1Len, MSG_DONTROUTE, (SOCKADDR*)&UDPClient, sizeof(UDPClient));
	if (iSendto == SOCKET_ERROR) {
		printf("Sending data failed\n");
		return 0;
	}

	// Step-6 CloseSocket Function

	iCloseSocket = closesocket(UDPSocketServer);
	if (iCloseSocket == SOCKET_ERROR)
	{
		printf(" Socket closing failed\n");
		return 0;
	}

	// Step-7 WSACleanup for terminating from DLL

	iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR)
	{
		printf("WSA Cleanup failed\n");
		return 0;
	}
	return 0;






}