#include "stdafx.hpp"
#include "params.hpp"
#include "chat/server.hpp"
#include "chat/client.hpp"

#define PARAM_SERVER    1
#define PARAM_CONNECT   2
#define PARAM_NICK      3
#define PARAM_DEBUG		9
#define PARAM_HELP		10

#define VERSION "1.0a"

#define PORT 9000

//zmienne globalne
std::mutex _locker;
bool _debug = false;

void print_usage(Params& params) {
    std::cout << "Chatter - program do czatowania w sieciach LAN\n"
            << "  Dostępne parametry:\n"
            << params.printParams(4);
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "");

	if (argc < 2) {
		std::cerr << "Nie podano zadnych parametrow. Napisz " << argv[0] << " -h, aby uzyskac pomoc";
		return 1;
	}

    //ustawienie parametrów
    Params params(false);
    params.addParameter(PARAM_SERVER, "s", "server", "startuje serwer czatu na komputerze", false);
    params.addParameter(PARAM_CONNECT, "c", "connect", "podłącza się do istniejącego serwera", true);
    params.addParameter(PARAM_NICK, "n", "nick", "ustawia nazwę użytkownika", true);
	params.addParameter(PARAM_DEBUG, "d", "debug", "wlacza tryb debugowania (rozszerzonych wiadomosci)", false);
	params.addParameter(PARAM_HELP, "h", "help", "wyswietla ten tekst pomocy", false);

    //parsowanie parametrów
    if (!params.parseParams(argc, argv)) {
        std::cerr << "Blad: " << params.getErrorDescription() << "\n";
        return 1;
    }
	else if (params.hasParameter(PARAM_HELP)) {
		print_usage(params);
		return 0;
	}
	else if (!params.hasParameter(PARAM_NICK)) {
		std::cerr << "Nie podano nicka\n";
		return 1;
	}
	else if (!params.hasParameter(PARAM_SERVER) && !params.hasParameter(PARAM_CONNECT)) {
		std::cerr << "Nie wybrano zadnego trybu (klient lub serwer)\n";
		return 1;
	}
	else if (params.hasParameter(PARAM_SERVER) && params.hasParameter(PARAM_CONNECT)) {
		std::cerr << "Mozna wybrac tylko jeden tryb.";
		return 1;
	}

	//ustawienie trybu debugowania
	_debug = params.hasParameter(PARAM_DEBUG);

    asio::io_service service;
	std::thread thread([&service]() {
		//uworzenie jakiegoś zajęcia, aby obiekt działał w tle
		asio::io_service::work work(service);
		service.run();
	});

	if (params.hasParameter(PARAM_SERVER)) {
		Server* server = new Server(&service, PORT);
		server->start(params.getString(PARAM_NICK));
		delete server;
	}
	else {
		Client* client = new Client(&service);
		client->start(params.getString(PARAM_CONNECT), PORT, params.getString(PARAM_NICK));
		delete client;
	}

	service.stop();
	thread.join();

    return 0;
}