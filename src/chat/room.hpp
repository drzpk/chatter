#ifndef ROOM_HPP_INCLUDED
#define ROOM_HPP_INCLUDED

#include "../stdafx.hpp"
#include "member.hpp"

/*
	## Schematy komunikacji
	>> 1. Rozpoczynanie połączenia
	Po udanym połączeniu się z serwerem, klient wysyła do serwera wiadomość CONNECT,
	zawierającą nick klienta (pole content) oraz wersję wiadomości (pole meta).
	W przypadku braku wysłania takiej wiadomości, klient zostanie rozłączony z serwerem
	po upływie określonego czasu.
	----------------------------------

    ## Opisy poszczególnych wiadomości.
    Pole 'sender' będzie zawierać nick wysyłającego wiadomość.

    >> CONNECT
    Wysyłana przez klienta podczas inicjacji połączenia.

    >> DISCONNECT
    Wysyłana przez serwer podczas kończenia połączenia.
    W polu 'content' znajduje się przyczyna odłączenia.

    >> ECHO
    Obecnie brak zastosowania

    >> CHAT
    Zawiera wiadomość czatu (w polu 'content'). Może być
    wysłana zarówno przez klienta jak i przez serwer.
	W przypadku rozsyłania wiadomości do klientów przez 
	serwer, pole 'meta' zawiera nick autora wiadomości.

	>> SERVER
	Może zawierać różne informacje wysyłane przez serwer,
	np. informacje o podłączeniu klienta, wynik jakiegoś
	zapytania wysłanego przez klienta itp. Zawartością
	tej wiadomości jest gotowa do wyświetlenia odpowiedź.
*/

class Room {
private:
    std::queue<Message*> queue;
	std::queue<Message*>* feedback_queue;
	std::vector<Member*> connecting_members;
    std::vector<Member*> members;

	std::thread* worker_thread;

    bool work;
	unsigned int max_clients = 5;
public:
    Room(std::queue<Message*>* feedback_queue);
	~Room();

	// Ustawia maksymalną liczbę podłączonych klientów
	// (włącznie z klientem lokalnym)
	void setMaxClients(unsigned int max_clients);

    // Dodaje nowego członka do czatu.
    void addMember(asio::ip::tcp::socket* socket);

	// Ustawia lokalnego członka czatu
	void setLocalMember(const std::string& name);

	// Dodaje wiadomość do kolejki.
	void addMessage(Message* message);

    // Zatrzymuje pokój czatu.
    void stop();

private:
	void broadcastMessage(Message* message);
	void write(const std::string& content, bool debug = false, asio::ip::tcp::socket* socket = 0);
    void _worker();
	std::vector<Member*>::iterator _get_member_by_socket(std::vector<Member*>& v, asio::ip::tcp::socket* socket);
};

#endif // ROOM_HPP_INCLUDED
