#pragma once

// Predefined macros hell:
//
// __GNUC__           Compiler is gcc, Clang or Intel on Linux
// __INTEL_COMPILER   Compiler is Intel
// _MSC_VER           Compiler is MSVC or Intel on Windows
// _WIN32             Building on Windows (any)
// _WIN64             Building on Windows 64 bit

#if defined(_WIN32)
	#pragma comment (lib, "Ws2_32.lib")
	#pragma comment (lib, "Mswsock.lib")
	#pragma comment (lib, "AdvApi32.lib")
	#include <WinSock2.h>
	#include <Ws2tcpip.h>
	#include <Windows.h>
#elif defined(__GNUC__)
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <unistd.h>
#else
	#error compiler or platform not supported
#endif

#include <cstring>
#include <chrono>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <thread>
#include <atomic>

#if defined(_WIN32)
	inline int write(SOCKET s, const void * buf, int len) { return send(s, (char*)buf, len, 0); }
	inline int read (SOCKET s, const void * buf, int len) { return recv(s, (char*)buf, len, 0); }
	inline int close(SOCKET s) { return closesocket(s); }
#elif defined(__GNUC__)
    const int INVALID_SOCKET = -1;
#endif

template <class Problem_t, class Solution_t>
class CWorker
{
public:
    uint64_t usr_id;
    uint64_t keep_alive_time;
    std::chrono::high_resolution_clock::time_point LastCommunication;
	bool verbose = false;
private:
    std::atomic_bool alive;
    sockaddr_in serv_addr;
    std::mutex mtx;
    std::thread t;

    int ConnectSock()
    {
         // create socket
        int sock = INVALID_SOCKET;
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
        {
            std::cerr << "ERROR: Could not create socket. " << std::endl;
			return INVALID_SOCKET;
        }

		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(5000));
				if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
				{
					std::cerr << "ERROR: Connect failed" << std::endl;
					return INVALID_SOCKET;
				}
			}
		}

        return sock;
    }

    void SendSignOfLife()
    {
        std::lock_guard<std::mutex> lock(mtx);

        int sock = ConnectSock();
        char id = 5;

        write(sock, &id    , sizeof(char));
        write(sock, &usr_id, sizeof(uint64_t));
        close(sock);
        LastCommunication = std::chrono::high_resolution_clock::now();
    }

    void Watchdog()
    {
        while (alive.load(std::memory_order_consume))
        {
            if (std::chrono::high_resolution_clock::now() - LastCommunication > 0.5 * std::chrono::seconds(keep_alive_time))
                SendSignOfLife();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

public:
    CWorker() {}
    ~CWorker() { Terminate(); }

	void Initialize(const char * const IP_of_server, const uint16_t port, bool verbose = false)
    {
        alive.store(false, std::memory_order_release);
        memset(&serv_addr, '0', sizeof(sockaddr_in));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        if(inet_pton(AF_INET, IP_of_server, &serv_addr.sin_addr) < 0)
        {
            std::cerr << "ERROR: inet_pton error occured" << std::endl;
            throw "inet_pton error occured";
        }
#ifdef _WIN32
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0)
			printf("WSAStartup failed with error: %d\n", iResult);
#endif

        int sock = ConnectSock();
        char id = 1;

        write(sock, &id             , sizeof(char));
        read (sock, &usr_id         , sizeof(uint64_t));
        read (sock, &keep_alive_time, sizeof(uint64_t));
        close(sock);
        LastCommunication = std::chrono::high_resolution_clock::now();
        alive.store(true, std::memory_order_release);

        t = std::thread(&CWorker::Watchdog, this);
    }

    void Terminate()
    {
        alive.store(false, std::memory_order_release);
        if (t.joinable()) t.join();
    }

    bool RequestProblem(Problem_t& problem, uint64_t & WU_id)
    {
        std::lock_guard<std::mutex> lock(mtx);
        int sock = ConnectSock();
        char id = 2;

		if (verbose) std::cout << "Sending id=2 to request work...";
        write(sock, &id    , sizeof(char));
		if (verbose) std::cout << "done." << std::endl;
		if (verbose) std::cout << "Sending usr_id...";
        write(sock, &usr_id, sizeof(uint64_t));
		if (verbose) std::cout << "done." << std::endl;

		if (verbose) std::cout << "Waiting for answer...";
        read (sock, &id    , sizeof(char));
		if (verbose) std::cout << "done. id=" << id << std::endl;

        if (id == 3)
        {
			if (verbose) std::cout << "Reading WU_id...";
            read(sock, &WU_id  , sizeof(uint64_t));
			if (verbose) std::cout << "done. WU_id=" << WU_id << std::endl;
			if (verbose) std::cout << "Reading problem...";
            read(sock, &problem, sizeof(Problem_t));
			if (verbose) std::cout << "done." << std::endl;
        }

        close(sock);
        LastCommunication = std::chrono::high_resolution_clock::now();
        return (id == 3);
    }

    void ReportSolution(const Solution_t & solution, const uint64_t WU_id)
    {
        std::lock_guard<std::mutex> lock(mtx);
        int sock = ConnectSock();
        char id = 4;

		if (verbose) std::cout << "Sending id=4 to report solution...";
        write(sock, &id      , sizeof(char));
		if (verbose) std::cout << "done." << std::endl;
		if (verbose) std::cout << "Sending usr_id...";
        write(sock, &usr_id  , sizeof(uint64_t));
		if (verbose) std::cout << "done." << std::endl;
		if (verbose) std::cout << "Sending WU_id...";
        write(sock, &WU_id   , sizeof(uint64_t));
		if (verbose) std::cout << "done." << std::endl;
		if (verbose) std::cout << "Sending solution...";
        write(sock, &solution, sizeof(Solution_t));
		if (verbose) std::cout << "done." << std::endl;
        close(sock);
		if (verbose) std::cout << "Sock closed." << std::endl;
        LastCommunication = std::chrono::high_resolution_clock::now();
    }
};
