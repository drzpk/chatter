#ifndef MEMBER_HPP_INCLUDED
#define MEMBER_HPP_INCLUDED

#include "../stdafx.hpp"
#include "message.hpp"

class Member {
private:
    asio::ip::tcp::socket* socket;
    std::queue<Message*>* msgQueue;
    std::string name;

    bool remote;
	bool is_sending;

    asio::streambuf buffer;
	std::string out_buffer_data;

public:
    /**
     * Tworzy zdalną instancję członka czatu.
     */
    Member(asio::ip::tcp::socket* socket, std::queue<Message*>* msgQueue);

    /**
     * Tworzy lokalną instancję członka czatu (członek znajdujący się na
     * maszynie, na której uruchomiono serwer).
     */
    explicit Member(const std::string& name);

    ~Member();

    /**
     * Wysyła nową wiadomość do klienta.
     */
    void sendMessage(Message& message);

	// Wysyła wiadomość do klienta, synchronicznie.
	void sendMessageSync(Message& message);

    /**
     * Zwraca gniazdo sieciowe.
     */
    asio::ip::tcp::socket* getSocket() const;

    /**
     * Ustawia nazwę klienta.
     */
    void setName(const std::string& name);

    /**
     * Zwraca nazwę członka czatu.
     */
    std::string getName() const;

    /**
     * Podaje, czy członek jest lokalny, czy zdalny.
     */
    bool isRemote() const;

private:
    void _reader();
	void _read_handler(const system::error_code& ec, std::size_t tr);
	void check_if_disconnected(const system::error_code& ec);
};


#endif // MEMBER_HPP_INCLUDED
