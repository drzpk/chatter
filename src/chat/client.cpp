#include "client.hpp"

const char* Client::PROMPT_PREFIX = ">> ";

Client::Client(asio::io_service* service) {
    this->service = service;

	socket = new asio::ip::tcp::socket(*service);
	worker_thread = 0;

	conn_started = false;
	completed = false;
}

Client::~Client() {
	delete socket;
}

void Client::start(const std::string& ip, short port, const std::string& name) {
    //podłączanie się do zdalnego serwera
	is_remote = true;
	work = true;

    tcp::resolver resolver(*service);
	auto it = resolver.resolve(tcp::resolver::query(ip, lexical_cast<std::string>(port)));
	auto end = tcp::resolver::iterator();

	while (it != end) {
		std::string ip = it->endpoint().address().to_v4().to_string();
		std::string port_s = lexical_cast<std::string>(port);
		printf("Trwa laczenie z %s:%s...\n", ip.c_str(), port_s.c_str());

		socket->async_connect(it->endpoint(), [this](const system::error_code& ec) {
			if (ec)
				return;
			conn_started = true;
		});

		//oczekiwanie na połączenie się z serwerem
		int stime = 500;
		int rounds = TIMEOUT * 1000 / stime;
		while (rounds > 0 && !conn_started) {
			std::this_thread::sleep_for(std::chrono::milliseconds(stime));
			rounds--;
		}

		if (!conn_started) {
			//socket->cancel();
			bool oo = socket->is_open();
			socket->close();
			
			it++;
			if (it != end) {
				std::cout << "Proba polaczenia nie powiodla sie, ponowne laczenie...\n";
			}
			else {
				std::cout << "Nie udalo sie polaczyc z serwerem.\n";
				stop();
				return;
			}
		}
		else {
			//połączenie zostało nawiązane
			//wysłanie nicku do serwera
			Message* msg = new Message(MessageType::CONNECT);
			msg->setContent(name);
			msg->setMeta(Message::MSG_VER);
			//oczekiwanie na serwer
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			sendMessage(msg);

			_read_msg();
			worker_thread = new std::thread(&Client::_worker, this);
			worker_thread->join();
			stop();
			return;
		}
	}
}

void Client::start(const std::string& name) {
	//podłączenie się do lokalnego serwera
	is_remote = false;
	work = true;
	this->name = name;

	//todo: walidacja nazwy użytkownika (długość)
	worker_thread = new std::thread(&Client::_worker, this);
	worker_thread->join();
	stop();
}

void Client::stop() {
	if (!work)
		return;
	work = false;
	
	try {
		//przy zamykaniu gniazda mogą wystąpić wyjątki
		//np. gdy połączenie z serwerem nie zostało ustanowione.

		socket->cancel();
		socket->shutdown(socket->shutdown_both);
		socket->close();
	}
	catch (...) {}
}

void Client::displayMessage(Message* message) {
	switch (message->getType()) {
	case MessageType::CHAT: {
		_locker.lock();
		//todo: dodanie czasu odebrania wiadomości
		std::string sender = message->getMeta() == name ? "Ty" : message->getMeta();
		printf("\r[%s]: %s\n%s%s", sender.c_str(), message->getContent().c_str(), PROMPT_PREFIX, pending_msg.c_str());
		_locker.unlock();
		break;
	}
	case MessageType::DISCONNECT: {
		std::string reason = message->getContent().size() > 0 ? message->getContent() : "brak danych";
		_locker.lock();
		if (is_remote)
			std::cout << "Rozlaczono z serwerem.\nPowod: " << reason << "\n";
		else
			std::cout << "Zamknieto serwer.\n";
		std::cout << "\nNacisnij dowolny klawisz, aby kontynuowac...\n";
		_locker.unlock();
		stop();
		break;
	}
	case MessageType::WRITE:
		write(message->getContent());
		break;
	case MessageType::DEBUG:
		debugWrite(message->getContent());
		break;
	default:
		//todo:
		break;
	}
}

void Client::sendMessage(Message* message) {
	std::string encoded = message->encode();
	asio::write(*socket, asio::buffer(encoded));
	delete message;
}

void Client::write(const std::string& msg) {
	//todo: uwzględnienie wiadomości, które zajmują więcej, niż jedną linię w konsoli

	bool condition = msg.size() && (msg[0] == '\n' || msg[msg.size() - 1] == '\n');
	std::string* msg2;
	if (condition) {
		//wiadomość zawiera znaki nowej linii na początku lub na końcu
		msg2 = new std::string(msg);
		if (msg[0] == '\n')
			msg2->erase(msg2->begin());
		if (msg[msg.size() - 1] == '\n')
			msg2->pop_back();
	}
	else
		msg2 = const_cast<std::string*>(&msg);

	_locker.lock();
	size_t len = strlen(PROMPT_PREFIX) + pending_msg.size();
	if (msg2->size() < len) {
		//rozmiar wiadomości jest za krótki - byłaby widoczna
		//poprzednia zawartość linii, więc trzeba ją najpierw wypełnić spacjami
		std::cout << "\r" << std::setfill(' ') << std::setw(len);
	}
	std::cout << "\r" << *msg2 << "\n"
		<< PROMPT_PREFIX << pending_msg;
	_locker.unlock();

	if (condition)
		delete msg2;
}

void Client::debugWrite(const std::string& msg) {
	if (_debug)
		write("#DEBUG: " + msg);
}

void Client::_read_msg() {
	asio::async_read_until(*socket, buffer, Message::BRAKE, [this](const system::error_code& ec, std::size_t st) {
		//bool finish = ec == asio::error::eof;
		if (ec) {
			if (!work)
				return;

			write("Utracono polaczenie z serwerem.");
			debugWrite("Kod bledu: " + lexical_cast<std::string>(ec.value()));
			debugWrite("Opis: " + ec.message());
			_locker.lock();
			std::cout << "\n\nNacisnij dowolny klawisz, aby kontynuowac...\n";
			_locker.unlock();
			stop();
			return;
		}
		std::string encoded((std::istreambuf_iterator<char>(std::istream(&buffer))), std::istreambuf_iterator<char>());
		try {
			Message message(encoded);
			displayMessage(&message);
		}
		catch (const std::invalid_argument& e) {
			debugWrite("Blad dekodowania wiadomosci: " + std::string(e.what()));
		}

		_read_msg();
	});
}

void Client::_worker() {
	//główna metoda klienta

	_locker.lock();
	if (is_remote)
		std::cout << "Polaczono z serwerem\n";
	else
		std::cout << "Utworzono serwer\n";

	std::cout << "Aby otrzymac pomoc, napisz :help\n"
		<< "Aby wyjsc z programu, napisz :quit\n\n";

	std::cout << PROMPT_PREFIX;
	_locker.unlock();

	while (work) {
		//todo: nieprzyjmowanie nowych znaków, jeśli wiadomość jest za długa
		char c = _rdchar();
		if (!work)
			break;

		//różne platformy mają różne znaki końca linii
		if (c == '\n' || c == '\r') {
			if (pending_msg.length() > 0 && pending_msg[0] == ':') {
				//dekodowanie specjalnych znaków
				_locker.lock();

				std::cout << "\n";
				if (pending_msg == ":help")
					std::cout << "Obecnie nie jest dostepna zadna pomoc";
				else if (pending_msg == ":quit") {
					std::cout << "Zatrzymywanie programu...";
					if (is_remote) {
						Message* disconnect = new Message(MessageType::DISCONNECT);
						sendMessage(disconnect);
					}
					_locker.unlock();

					//klient jest zatrzymywany w metodzie start()
					//stop();
					return;
				}
				else
					std::cout << "\nNie znaleziono polecenia. Napisz ':help', aby uzyskac liste dostepnych polecen";
				std::cout << "\n" << PROMPT_PREFIX;

				_locker.unlock();

				pending_msg.clear();
			}
			else {
				_locker.lock();
				std::cout << '\r' << std::string(pending_msg.size() + strlen(PROMPT_PREFIX), ' ') << '\r' << PROMPT_PREFIX;
				_locker.unlock();

				//wysłanie wiadomości
				Message* message = new Message(MessageType::CHAT);
				message->setMeta(name);
				message->setContent(pending_msg);
				pending_msg.clear();
				sendMessage(message);
			}
		}
		else if (c == '\b') {
			//backspace
			if (pending_msg.size() > 0) {
				pending_msg.pop_back();
				_locker.lock();
				std::cout << '\r' << std::string(pending_msg.size() + 1 + strlen(PROMPT_PREFIX), ' ')
					<< '\r' << PROMPT_PREFIX << pending_msg;
				_locker.unlock();
			}
		}
		else {
			pending_msg += c;
			_locker.lock();
			std::cout << c;
			_locker.unlock();
		}
	}
}

char Client::_rdchar() {
	//nie ma uniwersalnego sposobu na pobieranie pojedynczego znaku (funkcje są zdefiniowane w różnych bibilotekach)
#ifdef _WIN32
	return _getch();
#else
	return getch();
#endif
}

