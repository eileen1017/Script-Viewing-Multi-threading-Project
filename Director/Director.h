// header file of director class

#if ! defined (DIRECTOR_H)
#define DIRECTOR_H
#include "Player.h"

struct SinglePlay	// contains all info for a single play
{
	int id;
	vector<string> scene_names;
	vector<shared_ptr<Fragment>> fragments;
	Play* play;
	string name;
};

class Director
{
	int num_players;	// number of players/ threads 
	int available_id;	// keep track of number of plays current director have
	int max_config_lines;	// max number of config lines among all script files
	vector<shared_ptr<SinglePlay>> plays;	// list of plays
public:
	bool isRunning;	// true if current director is running a play, false otherwise
	vector<shared_ptr<Player>> players;	// all players
	shared_ptr<SinglePlay> play;	// current play
	Director(vector<string> names, int min_num_players = 0, bool flag = false);
	void readOneScript(string scriptName);
	void cue();
	void start(int id);
	void end(int id = notValid);
	string get_names();
	void quit();
};

// connects director and producer, also holds n of directors
class Connection: public ACE_Event_Handler
{
	ACE_INET_Addr* sendServer;
	ACE_INET_Addr* receiveServer;
	ACE_SOCK_Acceptor acceptor;	// for receive
	ACE_SOCK_Stream sendStream;
	ACE_SOCK_Stream receiveStream;
	ACE_SOCK_Connector connector;	// for send
	ACE_Reactor* react;
	shared_ptr<Director> director;
	int currId;	// the id of current client
public:
	Connection(string addr, shared_ptr<Director> d);
	~Connection();
	virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);
	virtual ACE_HANDLE get_handle() const;
	virtual int handle_signal(int signum, siginfo_t*,ucontext_t*);
	virtual int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask mask);
};

#endif
