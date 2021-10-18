#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32 // WIN32 ��, Linux�겻����
#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "WSOCK32.LIB")
#endif

#include "tp_protocol.h"

int main(int argc, char** argv) {
	int ret;
	// ����һ��windows socket �汾
	// һ��Ҫ������������ߵͰ汾��socket����ܶ�Ī��������;
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0) {
		printf("WSAStart up failed\n");
		system("pause");
		return -1;
	}
#endif

	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		goto failed;
	}
	// ����һ��Ҫ���ӷ�������socket
	// 127.0.0.1 ����IP��ַ;
	struct sockaddr_in sock_addr;
	sock_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(6080); // ������ϢҪ���͸�����socket;
	// ���������������Ƿ���˵ļ���socket;
	ret = connect((SOCKET)s, (const sockaddr*)&sock_addr, sizeof(sockaddr));
	if (ret != 0) {
		goto failed;
	}

	// ���ӳɹ�, s���������Ӧ��socket�ͻὨ������;
	// �ͻ��������ӵ�ʱ����Ҳ��Ҫһ��IP��ַ+�˿�;
	// �˿��Ƿ������˿ڡ����ǣ��ͻ���һ��û��ʹ�õĶ˿ھͿ�����;
	// �ͻ����Լ�Ҳ�����һ��IP + �˿�(ֻҪ��û��ʹ�õľͿ�����);
	// 
	char buf[32];
	memset(buf, 0, 32);

	int pkg_len = 0;
	unsigned char* data = tp_protocol::package((unsigned char*)"Hello", 5, &pkg_len);
	send(s, (const char*)data, pkg_len, 0);
	tp_protocol::release_package(data);
	recv(s, buf, 7, 0);

	int head_size = 0;
	tp_protocol::read_header((unsigned char*)buf, 7, &pkg_len, &head_size);
	printf("%s\n", buf + head_size);

failed:
	if (s != INVALID_SOCKET) {
		closesocket(s);
		s = INVALID_SOCKET;
	}
	
#ifdef WIN32
	WSACleanup();
#endif

	system("pause");
	return 0;
}

