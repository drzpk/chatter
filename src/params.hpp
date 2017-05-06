#ifndef PARAMS_HPP_INCLUDED
#define PARAMS_HPP_INCLUDED

#include "stdafx.hpp"

// Klasa ma obsługiwać parametry wejściowe programu
class Params {
private:
	bool caseSensitive;
	std::map<int, const char*> shortcuts;
	std::map<int, const char*> names;
	std::map<int, const char*> descriptions;
	std::map<int, std::string> values;
	std::set<int> withValues;
	bool error;
	std::string errorDescription;

public:
	// Tworzy nowy obiekt klasy Params
	// Params:
	// caseSensitive - czy wielkość znaków ma znaczenie
	Params(bool caseSensitive = true) : caseSensitive(caseSensitive), error(false) {}

	// Przetwarza parametry programu. Jeśli przetwarzanie
	// nie powiodło się, zwracana jest wartość 'false'.
	// Opis błędu można uzyskać za pomocą 'getErrorDescription()'.
	// Params:
	// argc - liczba parametrów
	// argv - podwójny wskaĹşnik na parametry
	// Returns:
	// czy przetwarzanie się powiodło
	bool parseParams(int argc, char** argv);

	// Dodaje nowy parametr. Każdy parametr posiada
	// identyfikator używany póĹşniej w innych metodach
	// obiektu. Skrót i nazwa parametru powinny być
	// podane bez wiodących myślników.
	// Params:
	// id - identyfikator parametru
	// shortcut - skrót parametru
	// name - pełna nazwa parametru
	// desc - opis parametru
	// hasValue - czy parametr ma wartość
	void addParameter(int id, const char* shortcut, const char* name, const char* desc, bool hasValue = true);

	// Sprawdza, czy parametr o podanym identyfikatorze istnieje.
	// Params:
	// id - identyfikator parametru
	// czy - parametr istnieje
	bool hasParameter(int id) const;

	// Sprawdza, czy wystąpił błąd
	bool isError() const;

	// Zwraca opis błędu przetwarzania parametrów.
	std::string getErrorDescription() const;

	// Wypisuje w konsoli wszyskie parametry, każdy
	// w osobnej linii. Parametr indent służy do ustawienia
	// wcięcia każdej linii.
	// Params:
	// indent - wcięcie linii
	// Returns:
	// parametry
	std::string printParams(int indent) const;

	// Zwraca całkowitą wartość liczbową parametru.
	// Params:
	// id - identyfikator parametru
	int getInt(int id) const;

	// Zwraca zmiennoprzecinkową wartość liczbową parametru.
	// Params:
	// id - identyfikator parametru
	float getFloat(int id) const;

	// Zwraca wartość tekstową parametru.
	// Params:
	// id - identyfikator parametru
	std::string getString(int id) const;
};

#endif // PARAMS_HPP_INCLUDED
