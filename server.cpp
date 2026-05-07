

#include <iostream>
#include <fstream>
#include <vector>
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

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "socket Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = INADDR_ANY; //설정
	ListenSockAddr.sin_port = htons(1234);

	Result = bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));
	if (Result == SOCKET_ERROR)
	{
		cout << "bind Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	Result = listen(ListenSocket, SOMAXCONN);
	if (Result == SOCKET_ERROR)
	{
		cout << "listen Error " << WSAGetLastError() << endl;
		exit(-1);
	}

	SOCKADDR_IN ClientSockAddr;
	memset(&ClientSockAddr, 0, sizeof(ClientSockAddr));
	int LengthClientSockAddr = sizeof(ClientSockAddr);


	while (true)
	{
		//blocking 함수
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSockAddr, &LengthClientSockAddr);

		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "accept Error " << WSAGetLastError() << endl;
			exit(-1);
		}

		// 1. 사진 크기 받기 (4바이트)
		int FileSize = 0;
		int RecvLength = recv(ClientSocket, (char*)&FileSize, sizeof(int), 0);
		if (RecvLength <= 0)
		{
			cout << "recv size Error " << WSAGetLastError() << endl;
			closesocket(ClientSocket);
			continue;
		}
		cout << "incoming file size : " << FileSize << " bytes" << endl;

		// 2. 그 크기만큼 반복해서 받기
		std::vector<char> FileBuffer(FileSize);
		int TotalRecv = 0;
		while (TotalRecv < FileSize)
		{
			int n = recv(ClientSocket, FileBuffer.data() + TotalRecv, FileSize - TotalRecv, 0);
			if (n <= 0)
			{
				cout << "recv data Error " << WSAGetLastError() << endl;
				break;
			}
			TotalRecv += n;
		}

		// 3. 파일로 저장
		std::ofstream out("recv.png", std::ios::binary);
		out.write(FileBuffer.data(), TotalRecv);
		out.close();
		cout << "save complete : recv.png" << endl;
		system("start recv.png");

		closesocket(ClientSocket);
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
}