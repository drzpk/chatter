#include "member.hpp"

Member::Member(asio::ip::tcp::socket* socket, std::queue<Message*>* msgQueue) {
    this->socket = socket;
    this->msgQueue = msgQueue;
    remote = true;
	is_sending = false;

	_reader();
}

Member::Member(const std::string& name) {
    this->name = name;
    remote = false;
	is_sending = false;
}

Member::~Member() {
	socket->cancel();
	socket->shutdown(socket->shutdown_receive);
    socket->close();
    delete socket;
}

void Member::sendMessage(Message& message) {
	if (!remote)
		throw std::runtime_error("nie mozna wyslac wiadomosci przez siec do klienta lokalnego");

	out_buffer_data = message.encode();
    asio::async_write(*socket, asio::buffer(out_buffer_data), [this](const system::error_code& ec, std::size_t tr) {
		if (ec)
			check_if_disconnected(ec);
	});
}

void Member::sendMessageSync(Message& message) {
	if (!remote)
		throw std::runtime_error("nie mozna wyslac wiadomosci przez siec do klienta lokalnego");

	std::string data = message.encode();
	system::error_code ec;
	asio::write(*socket, asio::buffer(data), ec);
	if (ec)
		check_if_disconnected(ec);
}

asio::ip::tcp::socket* Member::getSocket() const {
    if (!remote)
        throw std::bad_exception();

    return socket;
}

void Member::setName(const std::string& name) {
    this->name = name;
}

std::string Member::getName() const {
    return name;
}

bool Member::isRemote() const {
    return remote;
}

void Member::_reader() {
	try {
		asio::async_read_until(*socket, buffer, Message::BRAKE, bind(&Member::_read_handler, this, _1, _2));
	}
	catch (...) {
		std::cout << "fuck";
	}
}

void Member::_read_handler(const system::error_code& ec, std::size_t tr) {
	if (ec) {
		check_if_disconnected(ec);
		return;
	}
	std::istream is(&buffer);
	std::string data;
	//bufor może zawierać dodatkowe znaki
	std::getline(is, data, Message::BRAKE);

	try {
		Message* msg = new Message(data);
		msg->setSocket(socket);
		msg->setSender(name);
		//dodaj wiadomość do kolejki
		_locker.lock();
		msgQueue->push(msg);
		_locker.unlock();
	}
	catch (const std::invalid_argument& e) {
		//wiadomosc jest niepoprawna
		std::cout << "blaad: " << e.what();
	}

	_reader();
}

void Member::check_if_disconnected(const system::error_code& ec) {
	if (ec == asio::error::connection_reset || ec == asio::error::eof) {
		//wiadomość nie jest nigdzie wysyłana, tylko przekazywana do
		//kolejki wiadomości serwera
		Message* msg = new Message(MessageType::DISCONNECT);
		msg->setContent(name);
		msg->setSocket(socket);
		_locker.lock();
		msgQueue->push(msg);
		_locker.unlock();
	}
}
