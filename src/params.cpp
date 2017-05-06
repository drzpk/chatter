#include "params.hpp"


bool Params::parseParams(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);
		if (!caseSensitive)
			std::transform(arg.begin(), arg.end(), arg.begin(), tolower);
		if (!arg.substr(0, 2).compare("--"))
			arg.erase(0, 2);
		else if (!arg.substr(0, 1).compare("-"))
			arg.erase(0, 1);
		else {
			error = true;
			errorDescription = "nieznany parametr \"" + arg + "\"";
			return false;
		}

		for (auto p : shortcuts) {
			std::string pshort(p.second);
			std::string pfull(names.at(p.first));
			if (!caseSensitive) {
				std::transform(pshort.begin(), pshort.end(), pshort.begin(), tolower);
				std::transform(pfull.begin(), pfull.end(), pfull.begin(), tolower);
			}
			if (arg.compare(pshort) && arg.compare(pfull))
				continue;
			std::string value = "";
			if (withValues.count(p.first)) {
				if (pshort != "f" && (i + 1 == argc || std::string(argv[i + 1])[0] == '-')) {
					error = true;
					errorDescription = "brak wartości dla parametru " + pshort + "(" + pfull + ")";
					return false;
				}
				value = std::string(argv[++i]);
				std::replace(value.begin(), value.end(), ',', '.');
			}
			values.insert(std::pair<int, std::string>(p.first, value));
			break;
		}
	}
	return true;
}

void Params::addParameter(int id, const char* shortcut, const char* name, const char* desc, bool hasValue) {
	shortcuts.insert(std::pair<int, const char*>(id, shortcut));
	names.insert(std::pair<int, const char*>(id, name));
	descriptions.insert(std::pair<int, const char*>(id, desc));
	if (hasValue)
		withValues.insert(id);
}

bool Params::hasParameter(int id) const {
	return values.count(id);
}

bool Params::isError() const {
	return error;
}

std::string Params::getErrorDescription() const {
	return errorDescription;
}

std::string Params::printParams(int indent) const {
	if (error)
		return "";
	std::ostringstream stream;
	for (auto it = names.cbegin(); it != names.cend(); it++) {
		std::string shortcut = shortcuts.at((*it).first);
		std::string desc = descriptions.at((*it).first);
		stream << std::setw(indent) << std::setfill(' ')
			<< '-' << shortcut << " --" << (*it).second
			<< "  " << desc << std::endl;
	}
	return stream.str();
}

int Params::getInt(int id) const {
	if (error || !hasParameter(id) || !withValues.count(id))
		return 0;
	return atoi(values.at(id).c_str());
}

float Params::getFloat(int id) const {
	if (error || !hasParameter(id) || !withValues.count(id))
		return 0.f;
	return atof(values.at(id).c_str());
}

std::string Params::getString(int id) const {
	if (error || !hasParameter(id) || !withValues.count(id))
		return "";
	return values.at(id);
}
