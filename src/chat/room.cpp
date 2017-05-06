#include "room.hpp"

Room::Room(std::queue<Message*>* feedback_queue) {
	this->feedback_queue = feedback_queue;
    work = true;
    worker_thread = new std::thread(&Room::_worker, this);
}

Room::~Room() {
	if (work)
		stop();

	Message closeMsg(MessageType::DISCONNECT);
	closeMsg.setContent("serwer zostal zamkniety");
	for (auto it = members.begin(); it != members.end(); it++) {
		Member* member = *it;
		member->sendMessageSync(closeMsg);
		delete member;
	}
}

void Room::addMember(asio::ip::tcp::socket* socket) {
	//todo: wys³anie informacji o po³¹czeniu, np. wiadomoœæ powitalna

	Member* member = new Member(socket, &queue);
	Message connMsg(MessageType::CONNECT);

	//dodanie klienta do listy oczekuj¹cych
	_locker.lock();
	connecting_members.push_back(member);
	_locker.unlock();
	std::thread t([this, member]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		//sprawdzenie, czy klient nadal jest na liœcie (czyli nie odpowiedzia³ na wiadomoœæ)
		_locker.lock();
		auto pos = _get_member_by_socket(connecting_members, member->getSocket());
		if (pos != connecting_members.end()) {
			//klient nie odpowiedzia³, wiêc zostanie usuniêty
			connecting_members.erase(pos);
			_locker.unlock();

			Message closeMsg(MessageType::DISCONNECT);
			closeMsg.setContent("Niepoprawna inicjalizacja polaczenia z serwerem.");
			member->sendMessageSync(closeMsg);
			delete member;
		}
		else {
			//klient zosta³ dodany do g³ównej listy gdzie indziej
			_locker.unlock();
		}
	});
	t.detach();
}

void Room::setLocalMember(const std::string& name) {
	//todo: sprawdzenie, czy lokalny klient nie zosta³ dodany klika razy
	Member* member = new Member(name);
	members.push_back(member);
}

void Room::addMessage(Message* message) {
	queue.push(message);
}

void Room::stop() {
    work = false;
	worker_thread->join();
	delete worker_thread;
}

void Room::broadcastMessage(Message* message) {
	for (auto it = members.cbegin(); it != members.cend(); it++) {
		if ((*it)->isRemote())
			(*it)->sendMessage(*message);
	}

	//wys³anie wiadomoœci do lokalnego serwera
	//wiadomoœci s¹ usuwane z pamiêci podczas usuwania ich z poni¿szej kolejki
	_locker.lock();
	feedback_queue->push(message);
	_locker.unlock();
}

void Room::_worker() {
	//todo: okreœlenie maksymalnej iloœci u¿ytkowników na serwerze
    while (work) {
        if (queue.size()) {
            _locker.lock();
            Message* msg = queue.front();
            queue.pop();
            _locker.unlock();

			bool del = true;

			switch (msg->getType()) {
			case MessageType::CONNECT: {
				//schemat komunikacji 1.

				asio::ip::tcp::socket* socket = msg->getSocket();
				if (!socket) {
					//brak gniazda wymaganego do identyfikacji klienta
					break;
				}
				
				_locker.lock();
				auto pos = _get_member_by_socket(connecting_members, socket);
				if (pos == connecting_members.end()) {
					//b³¹d, nie znaleziono klienta
					_locker.unlock();
					break;
				}

				Member* member = *pos;
				connecting_members.erase(pos);

				if (msg->getContent().size() < 5 || msg->getContent().size() > 16) {
					//todo: dodanie informacji debugowania o nieprawid³owym nicku

					//b³¹d, brak nicka
					_locker.unlock();
					Message disconnectMsg(MessageType::DISCONNECT);
					disconnectMsg.setContent("Nick musi miec od 5 do 16 znakow.");
					member->sendMessageSync(disconnectMsg);
					delete member;
					break;
				}

				//dodanie klienta do listy
				member->setName(msg->getContent());
				members.push_back(member);
				_locker.unlock();
				std::cout << "uzytkownik " << member->getName() << " dolaczyl do serwera.\n";
				//todo: wyslanie wiadomosci o dolaczeniu do serwera do reszty u¿ytkowników

				break;
			}
			case MessageType::DISCONNECT:
				//wiadomoœæ generowana w klasie Member, gdy jakiœ klient
				//roz³¹czy³ siê z serwerem
				_locker.lock();
				if (msg->getContent().size() > 0) {
					//klient ma nazwê, czyli zosta³ poprawnie po³¹czony z serwerem
					//(patrz schematy komunikacji 1.)
					auto pos = _get_member_by_socket(members, msg->getSocket());
					if (pos != members.end()) {
						//todo: wiadomoœæ o roz³¹czeniu siê z serwerem do pozosta³ych klientów
						std::cout << msg->getContent() << " rozlaczyl sie z serwerem\n";
						Member* mem = *pos;
						members.erase(pos);
						delete mem;
					}
				}
				else {
					auto pos = _get_member_by_socket(connecting_members, msg->getSocket());
					if (pos != connecting_members.end()) {
						Member* mem = *pos;
						connecting_members.erase(pos);
						delete mem;
					}
				}
				_locker.unlock();

				break;
			case MessageType::CHAT:
				broadcastMessage(msg);
				del = false;
				break;
			}
			if (del)
				delete msg;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

std::vector<Member*>::iterator Room::_get_member_by_socket(std::vector<Member*>& v, asio::ip::tcp::socket* socket) {
	for (auto it = v.begin(); it != v.end(); it++) {
		if ((*it)->getSocket() == socket)
			return it;
	}

	return v.end();
}
