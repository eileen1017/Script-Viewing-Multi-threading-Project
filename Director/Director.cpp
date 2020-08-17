#include "Director.h"

// name: name of the script file
// min_num_players: min. number of players/threads created in this program if flag=false, number of players/threads created in this program if flag=true
// read each line in the script file, save all scene names and each character's name and corresponding file into each fragments
// create the play and number of players
Director::Director(vector<string> names, int min_num_players, bool flag)
{
	max_config_lines = 0;
	available_id = 0;
	isRunning = false;
	for(string name: names)
	{
		readOneScript(name);
	}
	if (flag)
	{
		num_players = min_num_players;
	}
	else
	{
		num_players = max(max_config_lines, min_num_players);
	}
}

void Director::readOneScript(string scriptName)
{
	ifstream script_file;
	script_file.open(scriptName);
	if (script_file.is_open())
	{
		shared_ptr<SinglePlay> currPlay(new SinglePlay());
		vector<string> scene_names;
		vector<shared_ptr<Fragment>> fragments;
		string line = "";
		size_t firstSpace = 0;
		int curr_config_lines = 0;
		int last_config_lines = 0;
		bool last_is_scene = false;
		unsigned int fragNum = 0;
		int num_players = 0;
		while (getline(script_file, line))
		{
			if (!line.empty())
			{

				firstSpace = line.find_first_of(" ");
				if (firstSpace > 0 && firstSpace < line.size())
				{
					string first = line.substr(0, firstSpace);
					if (first.compare("[scene]") == 0)
					{
						string sceneName = line.substr(firstSpace + 1);
						scene_names.push_back(sceneName);
						last_is_scene = true;
					}
				}
				else // assume config file is correct formatting: (either [scene] or txt)
				{
					ifstream config_file (line);
					string config_line = "";

					if (config_file.is_open())
					{
						num_players = 0;
						while (getline(config_file, config_line))
						{
							if (!line.empty())
							{
								string cName = "";
								string fName = "";
								istringstream iss(config_line);
								if (iss >> cName && iss >> fName)
								{
									shared_ptr<Fragment> f(new Fragment);
									f->character_name = cName;
									f->filename = fName;
									f->fragment_number = fragNum;
									fragments.push_back(f);
									curr_config_lines++;
								}
								num_players++;
							}
						}
						for(int i = 0; i < num_players; i++)
						{
							int last = fragments.size()-one;
							fragments[last-i]->num_players = num_players;
						}
						max_config_lines = max(max_config_lines, curr_config_lines + last_config_lines);
						last_config_lines = curr_config_lines;
						curr_config_lines = 0;

						if (!last_is_scene)
						{
							scene_names.push_back("");
						}
						last_is_scene = false;
						fragNum++;
					}
					else
					{
						cerr << "Can not open config file " << line << endl;
					}
				}
			}
		}
		currPlay->id = available_id;
		available_id++;
		currPlay->scene_names = scene_names;
		currPlay->fragments = fragments;
		currPlay->play = new Play(scene_names);

		// get proper play name from scipt file name
		// find first_
		int pos1 = scriptName.find_first_of('_');
		// find last_
		int pos2 = scriptName.find_last_of('_');
		// substring between is the proper name
		currPlay->name = scriptName.substr(pos1+one, pos2-pos1-1);
		plays.push_back(currPlay);
	}
	else
	{
		cerr << "script file not exist" << endl;
	}
}

void Director::start(int id)
{
	int size = plays.size();
	if(id >= 0 && id < size)	// check if id valid
	{
		play = plays[id];
		// reset play
		play->play->end = false;
		play->play->reset();
		players.clear();
		for (int i = 0; i < num_players; i++)
		{
			shared_ptr<Player> player(new Player(*(play->play)));
			players.push_back(move(player));
		}
		cue();
	}
	else
	{
		cerr << "receving id not valid" << endl;
	}
}

void Director::quit()	// must have!
{
	int size = plays.size();
	if(size>0)
	{
		play = plays[0];
		for (int i = 0; i < num_players; i++)
		{
			shared_ptr<Player> player(new Player(*(play->play), true));
			players.emplace_back(player);
		}
	}

	else
	{
		cerr << "Director::quit() has not plays" << endl;
	}
}

void Director::end(int id)
{
	if(id == play->id || id == -1)
	{
		for (int i = 0; i < (int)players.size(); i++)
		{
			players[i]->stop();
		}
	}
	else
	{
		cerr << "receving id is not playing" << endl;
	}
}

// Equally distributed all fragments tasks among all players/threads
// set thread/player as end if there's no work for it to complete
void Director::cue()
{
	size_t c = 0;
	play->play->print_first_scene();
	for (shared_ptr<Fragment> f : play->fragments)
	{
		players[c%players.size()]->enter(f);	// (try to) equally assign each fragment to different players
		c++;
	}

	if (c < players.size())
	{
		for (size_t i = c; i < players.size(); i++)
		{
			players[i]->end = true;
		}
	}
}

string Director::get_names()
{
	string result = "";
	for(shared_ptr<SinglePlay> currPlay: plays)
	{
		result += currPlay->name + " ";
	}
	return result;
}

Connection::Connection(string addr, shared_ptr<Director> d)
{
	director = d;
	currId = notValid;
	// get play names from d
	string msg = d->get_names();
	msg += "; " + addr;	// also send address
	// sent play list to producer
	sendServer = new ACE_INET_Addr(1025, ACE_LOCALHOST);

	// print ip address
	ACE_TCHAR address[INET6_ADDRSTRLEN];
	sendServer->addr_to_string(address, sizeof(address));
	ACE_OS::printf("sending address: %s\n", address);
	int call = connector.connect(sendStream, *sendServer);
	if (call >= 0) 	// call success
	{
		reactor(ACE_Reactor::instance());
		receiveServer = new ACE_INET_Addr();
		// set up receive acceptor
		receiveServer->string_to_addr(addr.c_str());	// change address
		int result = acceptor.open(*receiveServer, 1);
		if (result >= 0) 	// call success
		{
			sendStream.send_n(msg.c_str(), (int)msg.size());
			sendStream.close();
			// print ip address
			ACE_TCHAR address[INET6_ADDRSTRLEN];
			receiveServer->addr_to_string(address, sizeof(address));
			ACE_OS::printf("receiving address: %s\n", address);
			// register events
			this->reactor()->register_handler(SIGINT, this);
			this->reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
			this->reactor()->run_reactor_event_loop();
		}
		else
		{
			cerr << "address and port already in use" << endl;
		}
	}
	else
	{
		cerr << "Fail to connect producer. Port and address not match." << endl;
	}
}


int Connection::handle_input(ACE_HANDLE h)
{
	char msg[BUFSIZ];
	if(acceptor.accept(receiveStream)>=0)
	{
	if(receiveStream.recv(msg, sizeof(msg)) > 0)
	{
		string s = string(msg);

		// clear msg
		for(int i = 0; i < BUFSIZ; i++)
		{
			msg[i] = '\0';
		}

		istringstream iss(s);
		string action;
		int id;

		if(iss >> action)
		{
			if(action.compare("quit") == 0)
			{
				if (director->isRunning){
					director->end();
				}
				else {
					director->quit();
				}
				handle_close(h, ACE_Reactor::CLR_MASK);
				return success;
			}
			else if(iss>>id)
			{
				if(action.compare("start") == 0)
				{
					director->isRunning = true;
					director->start(id); // returns when play end

					thread t([this, id]()
					{
						while(!this->director->play->play->end)
						{
							continue;
						}
						// send end msg to producer
						string msg = "end " + to_string(id) + " " + to_string(this->currId);	// stop playID directorID
						director->isRunning = false;
						int call = this->connector.connect(this->sendStream, *(this->sendServer));
						if (call >= 0) 	// call success
						{
							this->sendStream.send_n(msg.c_str(), (int)msg.size());
							this->sendStream.close();
						}
					});
					t.detach();
					return success;
				}
				else if(action.compare("stop") == 0)
				{
					director->end(id);
					return success;
				}
			}
		}
		istringstream getId(s);
		if(getId >> id)
		{
			currId = id;
			return success;
		}
		cerr << "invalid msg receiving" << endl;
		return invalid_input;
	}
	}
	return invalid_input;
}

int Connection::handle_signal(int signum, siginfo_t*,ucontext_t*)	// get rid of virtual and change last parameter from siginfo_t to ucontext_t
{
	director->quit();
	// send id to Producer
	string msg = to_string(currId);
	int call = connector.connect(sendStream, *sendServer);
	if (call >= 0) 	// call success
	{
		sendStream.send_n(msg.c_str(), (int)msg.size());
		sendStream.close();
	}
	this->reactor ()->end_reactor_event_loop();
	this->reactor ()->close();
	return success;
};

int Connection::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{

	this->reactor ()->end_reactor_event_loop();
	this->reactor ()->close();
	return success;
}

ACE_HANDLE Connection::get_handle() const
{
	return acceptor.get_handle();
}

Connection::~Connection(){
	delete sendServer;
	delete receiveServer;
	delete react;
};

// main method of lab3 Director
int main(int argc, char** argv)
{
	if(argc >= rightNumberArgument)
	{
		ACE_INET_Addr addr;
		string a = string(argv[addr_pos]);
		a.append(":");
		a.append(string(argv[port_pos]));
		unsigned int num_players;
		istringstream iss(argv[min_players_pos]);
		if (iss >> num_players)
		{
			// construct director
			vector<string> script_files;
			for(int i = script_file_pos; i < argc; i++)
			{
				script_files.push_back(argv[i]);
			}
			shared_ptr<Director> d(new Director(script_files, num_players, true));
			// construct Connection
			Connection c(a, d);	// make connection with producer
		}
		return success;
	}
	else
	{
		cout << "usage: " << argv[programName] << "<port> <ip_address> <min_threads> <script_file>+" << endl;
		return inputNotCorrect;
	}
}
