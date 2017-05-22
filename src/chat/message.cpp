#include "message.hpp"

const char* Message::HEADER = "CHATTER";
const char* Message::MSG_VER = "1";

Message::Message(unsigned int code) {
    this->code = code;
	socket = 0;
}

unsigned int Message::getType() const {
	return code;
}

Message::Message(const std::string& encoded) /*throw (std::invalid_argument&)*/ {
    /*
     oss << HEADER << DELIMITER
		<< code << DELIMITER
        << id << DELIMITER
        << content << DELIMITER
		<< meta << DELIMITER
        << BRAKE;
    */

    std::vector<std::string> segments;
    std::string buffer;
	std::stringstream ss(encoded);
    while (std::getline(ss, buffer, DELIMITER)) {
		if (buffer.empty() || buffer[0] != BRAKE)
			segments.push_back(buffer);
    }

    if (segments.size() != 5)
        throw std::invalid_argument("Nieprawidlowa liczba segmentow w pakiecie.");
	if (strcmp(segments[0].c_str(), HEADER))
		throw std::invalid_argument("Niepoprawny naglowek wiadomosci");

    //błędy będą złapane w celu wygenerowania polskiego opisu błędu
    try {
        using std::stoi;
        code = stoi(segments[1]);
        setId(stoi(segments[2]));
    } catch (...) {
        throw std::invalid_argument("Niepoprawna składnia pakietu.");
    }

    //sprawdzenie, czy kod wiadomosci jest poprawny
	if (code < 1 || code > 7)
        throw std::invalid_argument("Niepoprawny kod wiadomości.");
    

	setContent(segments[3]);
    setMeta(segments[4]);
}

void Message::setId(unsigned int id) {
    this->id = id;
}

unsigned int Message::getId() {
    return id;
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
        << content << DELIMITER
		<< meta << DELIMITER
        << BRAKE;

    return oss.str();
}

void Message::setSocket(asio::ip::tcp::socket* socket) {
	this->socket = socket;
}

asio::ip::tcp::socket* Message::getSocket() const {
	return socket;
}
