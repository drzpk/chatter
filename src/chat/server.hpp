#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#include "../stdafx.hpp"
#include "client.hpp"
#include "room.hpp"

class Server : public Client {
private:
    asio::ip::tcp::acceptor* acceptor;
	std::thread* feedback_thread;

	std::queue<Message*> queue;
	Room* room;

public:
    // Tworzy nową instancję serwera. Jeśli dany port
    // jest już zajęty, rzucany jest wyjątek.
    // @arg portId numer portu
    Server(asio::io_service* service, short portId) throw(std::invalid_argument);

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
