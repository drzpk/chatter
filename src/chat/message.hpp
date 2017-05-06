#ifndef MESSAGE_HPP_INCLUDED
#define MESSAGE_HPP_INCLUDED

#include "../stdafx.hpp"

//Klasa definiuje schemat wiadomości wysyłanych w komunikatorze.
class Message {
private:
    static const char DELIMITER = '\n';
	static const char* HEADER;

    unsigned int code;
    unsigned int id;
    std::string sender;
    std::string content;
    std::string meta;
	
	asio::ip::tcp::socket* socket;
public:
    static const int MAX_SENDER_LENGTH = 12;
    static const int MAX_CONTENT_LENGTH = 128;
    static const int MAX_META_LENGTH = 32;
    static const char BRAKE = '\r';

    Message(unsigned int code);
    Message(const std::string& encoded) throw (std::invalid_argument&);

	// Zwraca typ wiadomości
	unsigned int getType() const;

     // Ustawia identyfikator wiadomości
    void setId(unsigned int id);
    unsigned int getId();

    // Ustawia nadawcę
    void setSender(const std::string& sender);
    std::string getSender();

    // Ustawia zawartość
    void setContent(std::string content);
    std::string getContent();

    // Ustawia dodatkowe dane wiadomości
    void setMeta(const std::string& meta);
    std::string getMeta();

    //  Koduje wiadomość w sposób umożliwiający jej wysłanie.
    std::string encode() const;

	void setSocket(asio::ip::tcp::socket* socket);

	asio::ip::tcp::socket* getSocket() const;
};

//Typy wiadomości
enum MessageType {
	CONNECT = 1,
	DISCONNECT = 2,
	ECHO = 3,
	CHAT = 4,
	SERVER = 5
};

#endif // MESSAGE_HPP_INCLUDED
