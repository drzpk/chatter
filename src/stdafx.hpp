#ifndef STDAFX_HPP_INCLUDED
#define STDAFX_HPP_INCLUDED

#include <algorithm>
#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <locale.h>
#include <map>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <boost/bind.hpp>

//#define ASIO_STANDALONE
#define _WIN32_WINNT 0x0501
#define BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_ASIO_ENABLE_CANCELIO
#include <boost/asio.hpp>

#define APPLICATION_PORT 9012

using namespace boost;

//zmienne globalne
extern std::mutex _locker;
extern bool _debug;

//sta³e globalne
#define MIN_NICK_LEN	5
#define MAX_NICK_LEN	16

#endif // STDAFX_HPP_INCLUDED
