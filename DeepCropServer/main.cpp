#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

#define HOME getenv("HOME")
#define PORT 8080

int main()
{
    int server, client, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    const char* hello = "Hello from server";

    char path[4096];
    char* res = realpath(".", path);
    std::cout << path << std::endl;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Failed to set socket options" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind on port " + PORT << std::endl;
        return -1;
    }

    if (listen(server, 3) < 0) {
        std::cerr << "Failed to listen on port " + PORT << std::endl;
        return -1;
    }

    if ((client = accept(server, (struct sockaddr*)&address, (socklen_t *)&addrlen)) < 0) {
        std::cerr << "Failed to accept client socket" << std::endl;
        return -1;
    }

    int bufferLength = 0;
    char *buffer = nullptr;

    // 1. 버퍼 크기 수신 후 메모리 할당
    read(client, (char*)&bufferLength, sizeof(int));
    buffer = new char[bufferLength + 1];

    // 2. 버퍼에 이미지 데이터 수신
    valread = read(client, buffer, bufferLength);
    
    // 3. 버퍼 데이터를 파일로 저장
    char filePath[128] = { 0, };
    sprintf(filePath, "%s/data/test.png", HOME);
    std::ofstream fout(filePath, std::ios_base::out | std::ios_base::binary);
    if (!fout.is_open()) {
        std::cerr << "Failed to accept client socket" << std::endl;
        return -1;
    }   

    fout.write(buffer, bufferLength);
    fout.close();

    send(client, hello, strlen(hello), 0);
    std::cout << "Hello message sent" << std::endl;

    return 0;
}