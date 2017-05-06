#include "message.hpp"

const char* Message::HEADER = "CHATTER";

Message::Message(unsigned int code) {
    this->code = code;
	socket = 0;
}

unsigned int Message::getType() const {
	return code;
}

Message::Message(const std::string& encoded) throw (std::invalid_argument&) {
    /*
    oss << HEADER << DELIMITER
		<< code << DELIMITER
        << id << DELIMITER
        << sender << DELIMITER
        << meta << DELIMITER
        << content << DELIMITER
        << BRAKE;
    */

    std::vector<std::string> segments;
    std::string buffer;
	std::stringstream ss(encoded);
    while (std::getline(ss, buffer, DELIMITER)) {
        segments.push_back(buffer);
    }

    if (segments.size() != 6 && segments.size() != 7)
        throw std::invalid_argument("Nieprawidlowa liczba segmentow w pakiecie.");

	//usuwanie naglowka
	segments.erase(segments.begin());

	//usuwanie znaku końca wiadomości, jeśli istnieje
	if (segments[segments.size() - 1][0] == BRAKE)
		segments.pop_back();

    //błędy będą złapane w celu wygenerowania polskiego
    //opisu błędu
    try {
        using std::stoi;
        code = stoi(segments[0]);
        setId(stoi(segments[1]));
    } catch (...) {
        throw std::invalid_argument("Niepoprawna składnia pakietu.");
    }

    //sprawdzenie, czy kod wiadomosci jest poprawny
    switch (code) {
    case MessageType::CONNECT:
    case MessageType::DISCONNECT:
    case MessageType::ECHO:
    case MessageType::CHAT:
        break;
    default:
        throw std::invalid_argument("Niepoprawny kod wiadomości.");
    }

    setSender(segments[2]);
    setMeta(segments[3]);
    setContent(segments[4]);
}

void Message::setId(unsigned int id) {
    this->id = id;
}

unsigned int Message::getId() {
    return id;
}

void Message::setSender(const std::string& sender) {
    this->sender = sender.substr(0, MAX_SENDER_LENGTH);
}

std::string Message::getSender() {
    return sender;
}

void Message::setContent(std::string content) {
    this->content = content.substr(0, MAX_CONTENT_LENGTH);
}

std::string Message::getContent() {
    return content;
}

void Message::setMeta(const std::string& meta) {
    this->meta = meta.substr(0, MAX_META_LENGTH);
}

std::string Message::getMeta() {
    return meta;
}

std::string Message::encode() const {
    std::stringstream oss;
    oss << HEADER << DELIMITER
		<< code << DELIMITER
        << id << DELIMITER
        << sender << DELIMITER
        << meta << DELIMITER
        << content << DELIMITER
        << BRAKE;

    return oss.str();
}

void Message::setSocket(asio::ip::tcp::socket* socket) {
	this->socket = socket;
}

asio::ip::tcp::socket* Message::getSocket() const {
	return socket;
}
