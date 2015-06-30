#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Winsock2.h>
#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main(int argc, char* argv[])
{
	WIN32_FIND_DATA findData;
	auto handle = FindFirstFile(L".\\*.*", &findData);
	do
	{
		wchar_t commandLine[80] = L"";
		wcscat_s(commandLine, L"certutil -addstore root ");
		wcscat_s(commandLine, findData.cFileName);
		wcout << commandLine << "\r\n";
	} while (FindNextFile(handle, &findData));

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return;
	struct addrinfo *result = nullptr;
	struct addrinfo *ptr = nullptr;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo("www.google.com", "http", &hints, &result) != 0)
	{
		WSACleanup();
		return;
	}
	ptr = move(result);
	auto connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (SOCKET_ERROR == connect(connectSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)))
	{
		closesocket(connectSocket);
		WSACleanup();
		return;
	}
	char googBuffer[80];
	int socketResult;
	send(connectSocket, "GET / HTTP/1.1 www.google.com", static_cast<int>(strlen("GET / HTTP/1.1 www.google.com")), 0);
	do
	{
		socketResult = recv(connectSocket, googBuffer, 50, 0);
		if (SOCKET_ERROR == socketResult)
		{
			closesocket(connectSocket);
			WSACleanup();
			return;
		}
	} while (socketResult);
	cout << googBuffer << "\r\n";
}