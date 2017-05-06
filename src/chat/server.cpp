#include "server.hpp"

Server::Server(asio::io_service* service, short portId) : Client(service) {
	//utworzenie punktu końcowego
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), portId);

    //akceptor
    acceptor = new asio::ip::tcp::acceptor(*service);
    acceptor->open(endpoint.protocol());
    acceptor->bind(endpoint);
    acceptor->listen(asio::socket_base::max_connections);

	room = new Room(&queue);
}

void Server::start(const std::string& name) {
	work = true;
	_worker();
	feedback_thread = new std::thread(&Server::_msg_redirector, this);
	Client::start(name);
}

void Server::stop() {
	Client::stop();
	room->stop();
	delete room;
	feedback_thread->join();
	delete feedback_thread;
}

void Server::sendMessage(Message* message) {
	room->addMessage(message);
}

void Server::_worker() {
    asio::ip::tcp::socket* socket = new asio::ip::tcp::socket(*service);
    acceptor->async_accept(*socket, [socket, this](const system::error_code& ec) {
		if (!ec) {
			debugWrite("nowe polaczenie z " + socket->remote_endpoint().address().to_string());
			room->addMember(socket);
		}
        _worker();
    });
}

void Server::_msg_redirector() {
	while (work) {
		if (queue.size()) {
			_locker.lock();
			Message* msg = queue.front();
			queue.pop();
			_locker.unlock();
			displayMessage(msg);
			delete msg;
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}
