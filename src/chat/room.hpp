#ifndef ROOM_HPP_INCLUDED
#define ROOM_HPP_INCLUDED

#include "../stdafx.hpp"
#include "member.hpp"

/*
	## Schematy komunikacji
	>> 1. Rozpoczynanie połączenia
	Po udanym połączeniu się z serwerem, klient wysyła do serwera wiadomość CONNECT,
	zawierającą nick klienta (pole content). W przypadku braku wysłania takiej
	wiadomości, klient zostanie rozłączony z serwerem po upływie określonego czasu.
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
public:
    Room(std::queue<Message*>* feedback_queue);
	~Room();

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
    void _worker();
	std::vector<Member*>::iterator _get_member_by_socket(std::vector<Member*>& v, asio::ip::tcp::socket* socket);
};

#endif // ROOM_HPP_INCLUDED