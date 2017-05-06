#ifndef CLIENT_HPP_INCLUDED
#define CLIENT_HPP_INCLUDED

#include "../stdafx.hpp"
#include "message.hpp"

#include <boost/lexical_cast.hpp>

#ifdef _WIN32
#include <conio.h>
#else
#include <curses.h>
#endif

using asio::ip::tcp;

class Client {
protected:
	asio::io_service* service;
	bool work;
private:
    static const int TIMEOUT = 5;
	static const char* PROMPT_PREFIX;

	asio::ip::tcp::socket* socket;
	std::thread* worker_thread;

	asio::streambuf buffer;

	//nick klienta
	std::string name;
	//aktualnie pisana wiadomosc
	std::string pending_msg;

	bool is_remote;
	bool conn_started;
	bool completed;

public:
    Client(asio::io_service* service);

	~Client();

    // Uruchamia klienta w wątku, w którym została wywyołana
    // ta metoda (połączenie się ze zdalnym serwerem).
    void start(const std::string& ip, short port, const std::string& name);

    // Uruchamia klienta w wątku, w którym została wywyołana
    // ta metoda (połączenie się z lokalnym serwerem).
    virtual void start(const std::string& name);

	// Zatrzymuje klienta
	virtual void stop();

    // Wyświetla wiadomość
    void displayMessage(Message* message);

protected:
	// Wysyła wiadomość na serwer.
	virtual void sendMessage(Message* message);

	// Wyświetla wiadomość w konsoli z zachowaniem aktualnie pisanej wiadomości.
	void write(const std::string& msg);

	// Podobnie jak powyższa metoda, ale wiadomości są wyświetlane tylko w trybie debugowania.
	void debugWrite(const std::string& msg);

private:
	void _read_msg();
	void _worker();
	char _rdchar();
};

#endif // CLIENT_HPP_INCLUDED
