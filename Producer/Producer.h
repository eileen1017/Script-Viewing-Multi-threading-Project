#include "../common.h"

// contains all info for one client / director
struct connection	
{
	map<int, string> plays;	// int is producer global ID
	int factor;	// global ID - factor = ID in director, also indicates id of current connection
	int currentRunID = notRunning;
	ACE_INET_Addr* server;
	ACE_SOCK_Connector* connector;
	ACE_SOCK_Stream* stream;
};

// server side 
class Producer: public ACE_Event_Handler
{
	ACE_INET_Addr* server;
	ACE_SOCK_Acceptor acceptor;
	ACE_Reactor* react;
	ACE_SOCK_Stream stream;
	vector<shared_ptr<connection>> connections;	// list of clients
	int global_last_id;	// the id for next join director / client
	void refresh_list();
public:
	Producer();
	virtual int handle_input(ACE_HANDLE h = ACE_INVALID_HANDLE);
	virtual int handle_signal(int signum, siginfo_t*,ucontext_t*);
	virtual ACE_HANDLE get_handle() const;
	virtual int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask mask);
};
