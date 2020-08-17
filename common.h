// common enums and struct
#if ! defined (COMMON_H)
#define COMMON_H
#pragma once
#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <cctype>
#include <locale>
#include <queue>
#include <map>
#include <chrono>
#include <future>
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Event_Handler.h"
#include "ace/Reactor.h"
#include "ace/SOCK_Connector.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"
#include "ace/Service_Object.h"
#include "ace/Connector.h"
using namespace std;

enum returnValues // return label values
{
	success,
	inputNotCorrect,
	threadingError,
	FileNotExist,
	noValidInfo,
	enter_fail,
	exit_fail,
	play_end,
	invalidPort,
	invalid_input,
	stopped,
	fail_connect
};

enum constants
{
	one = 1,
	notRunning = -1,
	notValid = -1
};

enum arg // argument position
{
	programName = 0,
	rightNumberArgument = 4,
	script_file_pos = 4,
	min_players_pos = 3,
	override_pos = 3,
	port_pos = 1,
	addr_pos = 2,
	correct_num_args = 2
};

struct Fragment // a fragment of the scene
{
	unsigned int fragment_number = 0;
	string character_name = "";
	string filename = "";	// file contains line number of the text of the character in current fragment
	int num_players = 0;
};

#endif
