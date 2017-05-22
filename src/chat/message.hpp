#ifndef MESSAGE_HPP_INCLUDED
#define MESSAGE_HPP_INCLUDED

#include "../stdafx.hpp"

//Klasa definiuje schemat wiadomości wysyłanych w komunikatorze.
class Message {
private:
    static const char DELIMITER = '\n';
	static const char* HEADER;

	// kod wiadomości oznaczający jej typ
    unsigned int code;
	// identyfikator wiadomości
    unsigned int id;

	// treść wiadomości
	std::string content;
	// dodatkowe dane wiadomości
	std::string meta;

	// gniazdo sieciowe (użytek wewnętrzny)
	asio::ip::tcp::socket* socket;

public:
	static const char* MSG_VER;
    static const int MAX_CONTENT_LENGTH = 128;
    static const int MAX_META_LENGTH = 32;
    static const char BRAKE = '\r';

    Message(unsigned int code);
    Message(const std::string& encoded) /*throw (std::invalid_argument&)*/;

	// Zwraca typ wiadomości
	unsigned int getType() const;

     // Ustawia identyfikator wiadomości
    void setId(unsigned int id);
    unsigned int getId();

    // Ustawia zawartość
    void setContent(std::string content);
    std::string getContent();

    // Ustawia dodatkowe dane wiadomości
    void setMeta(const std::string& meta);
    std::string getMeta();

    // Koduje wiadomość w sposób umożliwiający jej wysłanie.
    std::string encode() const;

	// Ustawia gniazdo sieciowe wiadomości.
	void setSocket(asio::ip::tcp::socket* socket);

	// Zwraca gniazdo wiadomości lub 0, jeśli nie zostało ustawione.
	asio::ip::tcp::socket* getSocket() const;
};

//Typy wiadomości
enum MessageType {
	CONNECT = 1,
	DISCONNECT = 2,
	ECHO = 3,
	CHAT = 4,
	SERVER = 5,
	ONLINE = 6,
	UPTIME = 7,

	//poniższe typy wiadomości są wewnętrzne, tj. nie będą wysyłane przez sieć.
	WRITE = 10,
	DEBUG = 11
};

#endif // MESSAGE_HPP_INCLUDED
