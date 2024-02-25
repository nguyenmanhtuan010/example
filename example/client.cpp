#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>


#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE] = {0};

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    while (true) {
        std::string message;
        std::cout << "Enter command (create/read/delete/write/tail): ";
        std::getline(std::cin, message);

        if (message.find("write ") != std::string::npos) {
            std::string filename, content;
            std::cout << "Enter filename: ";
            std::getline(std::cin, filename);
            std::cout << "Enter content: ";
		    auto start = std::chrono::high_resolution_clock::now(); // Lấy thời điểm bắt đầu
            std::getline(std::cin, content); // Gọi hàm để lấy dữ liệu từ người dùng
            auto end = std::chrono::high_resolution_clock::now(); // Lấy thời điểm kết thúc
            std::chrono::duration<double> duration = end - start; // Tính thời gian trôi qua
			if(duration.count() > 10){
				std::cout << "Timeout, re-enter: ";
				std::getline(std::cin, content); // Gọi hàm để lấy dữ liệu từ người dùng
			}
            message = "write " + filename + " " + content;
        }

        send(clientSocket, message.c_str(), message.length(), 0);

        if (message.find("tail ") == 0) {
            int valread;
            while ((valread = read(clientSocket, buffer, BUFFER_SIZE)) > 0) {
                buffer[valread] = '\0';
                std::cout << buffer;
            }
        } else {
            int valread = read(clientSocket, buffer, BUFFER_SIZE);
            buffer[valread] = '\0';
            std::cout << buffer << std::endl;
        }
    }

    close(clientSocket);
    return 0;
}
