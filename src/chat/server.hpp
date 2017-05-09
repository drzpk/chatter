#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#include "../stdafx.hpp"
#include "client.hpp"
#include "room.hpp"

#define MIN_CLIENTS		2
#define MAX_CLIENTS		20

class Server : public Client {
private:
    asio::ip::tcp::acceptor* acceptor;
	std::thread* feedback_thread;

	std::queue<Message*> queue;
	Room* room;

public:
    // Tworzy nową instancję serwera. Jeśli wystąpił błąd,
	// rzucany jest wyjątek
    Server(asio::io_service* service, short portId) /*throw(std::invalid_argument)*/;

	// Ustawia nową maksymalną liczbę klientów
	void setMaxClients(unsigned int value) /*throw(std::invalid_argument)*/;

	// uruchamia serwer i klienta
	void start(const std::string& name);

	// Zatrzymuje serwer
	void stop();

protected:
	// wysyła wiadomość do wszystkich klientów (włącznie z lokalnym)
	void sendMessage(Message* message);

private:
    void _worker();
	void _msg_redirector();
};


#endif // SERVER_HPP_INCLUDED
