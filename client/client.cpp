#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

using namespace std;

int main()
{
	//ws2_32.dll 로딩, winsock -> bsd socket 윈도우에서 구현체
	WSAData  wsaData;

	int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (Result != 0)
	{
		cout << "WSAStartup Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "socket Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ServerSockAddr;
	memset(&ServerSockAddr, 0, sizeof(ServerSockAddr));
	ServerSockAddr.sin_family = AF_INET;
	ServerSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //설정
	ServerSockAddr.sin_port = htons(1234);

	//blocking
	Result = connect(ServerSocket, (SOCKADDR*)&ServerSockAddr, sizeof(ServerSockAddr));
	if (Result == SOCKET_ERROR)
	{
		cout << "bind Error " << WSAGetLastError() << endl;
		exit(-1);
	}



	std::ifstream file("recv.png", std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		cout << "file open Error" << endl;
		exit(-1);
	}
	int FileSize = (int)file.tellg();
	file.seekg(0);

	// 2. 파일 내용 전부 읽기
	std::vector<char> FileBuffer(FileSize);
	file.read(FileBuffer.data(), FileSize);
	file.close();

	// 3. 크기 먼저 보내기 (4바이트)
	int SentLength = send(ServerSocket, (char*)&FileSize, sizeof(int), 0);
	if (SentLength <= 0)
	{
		cout << "send size Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	// 4. 실제 데이터 반복해서 보내기
	int TotalSent = 0;
	while (TotalSent < FileSize)
	{
		int n = send(ServerSocket, FileBuffer.data() + TotalSent, FileSize - TotalSent, 0);
		if (n <= 0)
		{
			cout << "send data Error " << WSAGetLastError() << endl;
			exit(-1);
		}
		TotalSent += n;
	}
	cout << "send complete : " << FileSize << " bytes" << endl;




	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}