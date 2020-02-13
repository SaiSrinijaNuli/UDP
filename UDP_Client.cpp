#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}

int main(int argc,char *argv[])
{
	WSADATA WinSockData;
	int iWsaStartup;
	int iWsaCleanup;

	SOCKET UDPSocketCient;
	struct sockaddr_in UDPServer;

	char Buffer[512] = "[client] : PING";
	char Buffer1[512];
	int iSendto;
	int iReceiveFrom;
	int portno;
	int iBufferLen = strlen(Buffer) + 1;
	int iBuffer1Len = strlen(Buffer1) + 1;
	int iUDPServerLen = sizeof(UDPServer);
	int iCloseSocket;
	struct timeval start, end;
	double t1, t2;

	if (argc < 2) {
		printf("ERROR, no sufficient arguments\n");
		return 0;
	}

	// Step-1 initialization of winsock

	iWsaStartup = WSAStartup(MAKEWORD(2, 2), &WinSockData);
	if (iWsaStartup != 0)
	{
		printf(" WSAStartup failed\n");
		return 0;
	}

	// Step-2 Fill the UDPServer Structure

	UDPServer.sin_family = AF_INET;
	UDPServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	portno = atoi(argv[1]);
	UDPServer.sin_port = htons(portno);
	UDPServer.sin_port = htons(8001);

	// Step-3 Socket Creation

	UDPSocketCient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (UDPSocketCient == INVALID_SOCKET) {
		printf(" UDP Socket creation failed\n");
		return 0;
	}

	// Step-4 Sendto
	int i;
	int n;
	int stop = 0;
	while(!stop)
	{
		t1 = 0.0; t2 = 0.0;
		if (gettimeofday(&start, NULL)) {
			printf("time failed\n");
			return 0;
		}
		iSendto = sendto(UDPSocketCient, Buffer, iBufferLen, MSG_DONTROUTE, (SOCKADDR*)&UDPServer, sizeof(UDPServer));
		if (iSendto == SOCKET_ERROR) {
			printf("Sending data failed\n");
			return 0;
		}

		Sleep(1);

		iReceiveFrom = recvfrom(UDPSocketCient, Buffer1, iBuffer1Len, MSG_PEEK, (SOCKADDR*)&UDPServer, &iUDPServerLen);
		if (iReceiveFrom == SOCKET_ERROR)
		{
			printf(" Receiving failed\n");
			return 0;
		}
		Buffer1[iReceiveFrom] = '\0';
		if (strcmp(Buffer1,"end") == 0)
		{
			stop = 1;
			break;
		}
			printf("received %s\n", Buffer1);
			
			if (gettimeofday(&end, NULL)) {
				printf("time failed\n");
				return 0;
			}
			t1 += start.tv_sec + (start.tv_usec / 1000000.0);
			t2 += end.tv_sec + (end.tv_usec / 1000000.0);

			printf(" Sent... RTT = %g ms\n", (t2 - t1) / 100);
	}

	// Step-5 CloseSocket Function

	iCloseSocket = closesocket(UDPSocketCient);
	if (iCloseSocket == SOCKET_ERROR)
	{
		printf("Close Socket failed\n");
		return 0;
	}

	// Step-6 WSACleanUp

	iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR)
	{
		printf("Cleanup failed\n");
		return 0;
	}

	return 0;
}