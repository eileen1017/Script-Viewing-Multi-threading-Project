# Script-Viewing-Multi-threading-Project

Assignment Instruction：
https://www.cse.wustl.edu/~cdgill/courses/cse532/lab1/
https://www.cse.wustl.edu/~cdgill/courses/cse532/lab1/
https://www.cse.wustl.edu/~cdgill/courses/cse532/lab2/
https://www.cse.wustl.edu/~cdgill/courses/cse532/lab3/

---------- Part 1 -----------   
Overview:   
1. Play class: Quite same as Lab2,   
Things we have changed:   
1). Public Play constructor: takes in a reference to string vector which should be the names of scenes passed in, and initialize line counter as 1, scene_fragment_counter as 0, on_stage should default by 0, pass input parameter to private string vector scene names, and set public string current character as empty(""). Initialize exit_player as 0 to keep track that all the players in the scene has entered and exited. Initialize the iterator to start from the first scene.   
2). Public print_first_scene() method: function call to display the name of the current scene.   
3). Public exit() method: takes in a shared pointer Fragment which we created for fragment of scene structor in common.h, and a boolean value stop if producer asks for stop play   
	a). Make a mutex lock and follow 3 conditions on checking the on_stage variable   
	b). If on_stage is bigger than 1, we decrement on_stage and print who exits and return success   
	c). If on_stage is smaller than 1, we will return exit fail.   
	d). Otherwise, on_stage is exactly 1, we will decrement on_stage, print who exits, increment newly added variable exit_players to indicate the number of players has exited from the scene.   
	e). If the all players exit which indicated by comparing the exit_players variable with a indicator of number of players in the fragment in the current fragment object pointer, increment fragment counter to indicate end of fragment, make current character string as default empty, make line_counter as 1 to switch to new fragment file and reset the exit_players to 0. Also check for iterator member variable to make sure all lines in container has printed out. Notify all and return success.   
4). Public reset() method: This method is called when a play is finished or stopped, resetting everything as default.   
Things we did not change:   
1). Container structor for a line in any character file, which stores the line order, character name and text.   
2). Private member variables: a mutex lock, condition variable, line counter of current fragment, fragment counter, number of characters on stage counter, a string vector for all scene names of current script, a string vector iterator for scene names vector.   
3). Public recite() method:  takes in container vector iterator and a reference to scene_fragment_number: Basically we follow the steps given in the lab2 instruction   
	a). make a unique_lock to avoid race condition;   
	b). Follow the steps given in recite method, and modify the implementation when scene_fragment_counter == scene_fragment_number && line_counter == iter->order, so that to change the character name printed when the passed character is different than current character, and then print out text and increment line_counter. Then, follow the steps given in instruction of lab2 to stress the error when counter is greater than order number.   
	c). Last, make sure to unlock and notify all, increment iteration and return.   
4). Public enter() method: takes in a shared pointer Fragment which we created for fragment of scene structor in common.h   
	a).  Basically, this method is to handle the enter of the character for the fragment,   
	b).  If fragment number is different, either it enters failed when smaller than counter or wait on a condition until they are same when bigger than counter   
	c). Otherwise, it will print out who enters and increment on_stage, unlock and return success with no error code.    


2. Player class: idea is very similar to Lab2, some in method minor changes happened:   
Things we have changed:   
1). Add promise stopSiganl to indicate a stop signal is received and future stopFuture to indicate a stop is performed.   
2). Public Player destructor: Instead of calling the thread to join, since we did detach in Player constructor, it will call the clearQueue() method and clean the fragment queue and notify all.   
3). Public Player constructor: takes in a Play object reference and a default boolean as false, if Producer has quit, set it as true:   
	a). initialize currPlay member variable with input parameter play object, set end as default false.   
	b). construct thread, set future as get future of promise. If isQuit bool input is true, call stop to give the stop signal. Call prepare().   
	c). Move the threads   
	d). Detach the threads.   
4). Public prepare() method:   
	a). Basically, it will get one fragment a time from working queue, and call read the fragment by calling the member function read(fragement).   
	b). If we have not finished all the fragment in the queue, the program should make lock and ask the thread to wait until all fragments have been read and proceeded or the scene is finished for 400ms.   
	c). If there is still fragment in the fragment_queue, it shoud pop the fragment, unlock the mutex, notify all and read the fragment. At this point, we should also check whether the read() is finished. If it is, clearQueue() and break.   
	d). Instead, if there is no more fragment in the queue, we will unlock, notify all and break.   
5). Public read() method:   
	a). first thing we need to do is to check the play is called to be stopped, if it is, just return stopped.   
	b). Read lines from each fragment filename, we will skip all empty lines.   
	c). If we did not find a white space in the line, it will be a badly formatted line, we will print the error message but continue to read.   
	d). Otherwise, read the valid lines, cast the string line number to int. Pass the line information and text and character name into the container. Push the container into content which the container vector.   
	e). If we cannot extract two elements from the line, we will print the error message to indicate bad line.   
	f). Then, after all lines have read, we will call play to enter, and act method in the player class to ask the player to recite their parts, and finally ask play to exit.   
	g). Check one more statement, if the play is stopped, we will call all the players to exit.   
	h). Of course, we will print a error message if the fragment file cannot open.   
6). Public isStop() method: check if future status is timeout, if it is, return false. Otherwise, return true.   
7). Public stop() method: set the promise signal, and set play as end, and notify all.   
8). Public clearQueue() method: Make sure that all players exit and fragment is popped.   
Things we did not change:   
1). Private member variables: a Play variable to keep track where this player belong to, corresponding thread of current player, container vector to store all the contents of a fragment in the working queue, a working queue contains shared pointer of all fragments for current thread, a mutex lock and a condition variable.   
2). Public boolean variable end to indicate the end of the fragment.   
3). Public act() method: takes in a reference to the fragment that has just popped from the queue and a vector of container of all the read lines from fragment files:   
	a). initialize the content iterator for it to be passed into recite() method in Play class   
	b). If the content is not empty and play is not stopped, we will call  the recite() method with input parameters, content iterator and the fragment number of the input fragment popped.   
8). Public enter() method: takes in a reference to the fragment that has just popped from the queue: this method is to add given fragment to the working queue with locking scheme.   


3. Director Class: (HS/HA pattern design) we have changed many things here and added new ones.   
---SinglePlay structor---   
1). this structor contains all information for a single play   
2). an int id, a string vector to keep track all scene names in the play, a vector of shared pointer of Fragment object to indicate all fragments included, a play object, and a string name for play name.   

---In Director class:---   
1). Private member variables: a variable to check number of players, an available id to keep track of number of plays current director has, an int max_config_lines to keep track max number of config lines in the script files, a vector of object SinglePlay shared pointer plays to make list of plays.   
2). Public boolean isRunning to check current director is running a play or not, a vector of shared pointer of players to list all players, a shared pointer of SinglePlay object to list information of current play.   
3). Public Director constructor: takes in vector of string for names of the script files, a int min_num_players to indicate min number of players/threads created in the program if flag == false, a bool flag.   
	a). set max_config_lines as default 0, available_id as default 0, isRunning as default false.   
	b). Iterate through names of the script files, call readOneScript() method for each of them.   
	c). Then we check the flag. If true, we use the minimum number of players. Otherwise, just use exactly how many characters in the play.   
4). Public readOneScript() method: takes in script name:   
	a). Pass the name of scene to input file stream and read from it. If it is open and line is not empty, we will check the first string is or is not [Scene], if yes, we push its scene name to scene name member variable, and we set our bool last_is_scene to indicate previous read line is a scene.   
	b). If we did not find a space in this read line from input file stream, we will open the config file read from this line and read each non-empty line, pass the information of character name, config file name and fragment number into our created Fragment structor, and push back the Fragment in the corresponding member variable. Then, we increment num_players when each line of the config file read.   
	c). Iterate through number of players, pass the num_players to the each fragment.   
	d). Then, keep track of max config lines, in compare of max_config_lines and current config lines and previous config lines, update last_config_line to current config lines and reset current config line.   
	e). If previous line in partial config file is not scene, we will push a empty string to member variable, and set it as false, increment fragement number.   
	f). Otherwise, if the file cannot open, just print out error message.   
	g). Give each SinglePlay an id. Set scene name, fragments and initialize a play with scene name for each SinglePlay.   
	h). Parse the play name and set it to SinglePlay.   
	i). Push each SinglePlay to the plays vector.   
	j). If we cannot open partial scene file, we will print error message script file not exist.   
5). Public cue() method:   
	a). Call print_first_scene method in the play.   
	b). for each fragment in the fragments variables, we will equally assign it with different players and ask it to enter.   
	c). If the entered player number is smaller than actual players should enter, it will iterate through players, and set extra players as end.   
6). Public get_names() method: simply returns all the play names as a single string output.    
7). Public start() method: takes in a play id, it is called when an input starts with "start" from Producer:    
	a). Check if input id is valid, if it is, reset play and clear players. Then reconstruct the player object, and push each player into players again.   
	b). call cue().   
	c). if id not valid, print error message.   
8). Public quit() method: it is called when an input starts with "quit" from Producer when a director has just connected to producer and haven't run any play.   
9). Public stop() method: it is called when an input starts with stop or an input starts with "quit" from Producer when a director has run any play:   
	a). If an id is same as play id or is -1 when a quit is called, ask each players to stop by calling stop() in player class.   
	b). Otherwise, there is no play is running. Print error message.    

---In Connection class:---    
1). Private member variables: ACE_INET_Addr, ACE_SOCK_Stream for send server and receive server respectively, an ACE_SOCK_Acceptor for receiving message, a ACE_SOCK_Connector for sending message, a reactor, and Director share pointer and a currId for current client id.    
2). Public Connection() constructor: takes in an address and a Director share pointer:   
	a). Initialize member variable director and current client id.   
	b). Get play names from director.   
	c). Start a sending server to send play list to producer and print IP address.   
	d). Connect to send stream, if call success, set up a receive server and open acceptor, if success, use send stream to send message and size and then close it.   
	e). Print IP address of the receiver.   
	f). register the events by calling reactor's register_handler and run_reactor_event_loop.   
	g). If the acceptor is not open, print error message address and port already in use.   
	h). If the connector fails, print error message that fail to connect to producer.   
3). Public handle_input() method: takes in an ACE_HANDLE:   
	a). If acceptor succeed and receiver stream received any message:   
		1). if message start with "quit": if any play is running, called end() method in Director class. Otherwise, call quit(). Handle close and return success.   
		2). if message start with "start": call start(id) method, and start a new thread listens to a command from producer while a play is running. Detach the thread and return success.   
		3). if message start with "stop": call end(id) method and return success.   
	b). Set current ID and return success.   
	c). If all above failed, print error message invalid message receiving and return error code.   
4). Public handle_signal() method: takes in signum, siginfo_t and ucontext_t:   
	a). call director to quit().   
	b). Send its id to Producer server, and then handle end_reactor_event_loop and clsoe().   
	c). return success.   
5). Public handle_close() method: call acceptor get_handle();   
6). Public Connection destructor: delete all servers and react.   

4. Main function in Director.cpp:   
1). Check for right number of arguments at least 4, if failed, print usage message and return error code.   
2). If yes, parse each input, get the number of players, push back the input partial script file names.   
3). Initialize Director and construct connection.   
4). Return success.   

5. Producer Class:   
1). a connection structor: contains all info for one client or director:   
	a). a map of int as key to indicate the producer global ID and string as value   
	b). a factor as global id in director and also as the id of current connection   
	c). a currentRunID as default not running -1.   
	d). An ACE_INET_Addr as server, an ACE_SOCK_Stream as stream, and an ACE_SOCK_Connector as connector.   
2). Private member variable: ACE_INET_Addr, ACE_SOCK_Acceptor, ACE_Reactor, ACE_SOCK_Stream for connection purpose, and a list if connection which indicates the list of clients, an ID to indicate the last client that the producer has communicated with.    
3). Private refresh_list() method: a method to display the play list to the users and show the status of each play that is stored in every connection.   
4). Public Producer() constructor:   
	a). set global_last_id as default 0, set server with port 1025 and localhost address of ACE.    
	b). Set up call as acceptor.open() the server. Set up reactor.    
	c). If call to acceptor success, print IP address and register the event.   
	d). Otherwise, print error message fail to connect.   
5). Public virtual handle_input() method: called when user input or one of clients send message   
	a). If user inputs:   
		1). check if input message is valid. If not, print error message.   
		2). Either start/stop corresponding play or global quit   
			- if input is "start <valid ID>", send to corresponding director the start command.    
			- if input is "stop <valid ID>", send to director the stop command.    
			- if input is "quit", send quit message to director.   
		3). Handle close and return success when quit sent.    
		4). Handle close and refresh play list and then return success.    
	b). If exit director send message for quit:    
		1). if msg is finish one play: update status and refresh list     
		2). if director quit: Erase corresponding plays from list that stored in connections and refresh list   
	c). If a new director join with msg contains its address, port and play list:   
		1). issue a unique id to new director start stored as a connection structor and pushed to connections which Producer has kept track of.   
		2). Add its plays to list and refresh list   
6). Public virtual handle_close() method: called when user hits Ctrl-C. Deregister all events and close reactor    
7). Public virtual handle_signal() method: called when user Ctrl-C, send quit message to all clients, and deregister events and close reactor.   
8). Public virtual get_handle() method: return default acceptor get_handle() method.   

6. Main() function in Producer.cpp:   
1). Check for right number of arguments, if not, print usage message.   
2). Otherwise, construct the Producer object and return success.    

Wrapper Facedes:   
1. Classes are cohensive:   
class Director   
{
public:   
	vector<shared_ptr<Player>> players;	// all players   
	shared_ptr<SinglePlay> play;	// current play   
	Director(string name, int min_num_players = 0, bool flag = false);    
	void cue();   
};    

class Player {   
public:   
	bool end;   
	Player(Play& p, bool isQuit = false);   
	~Player();   
	void prepare();   
	int read(shared_ptr<Fragment>& f);   
	void act(shared_ptr<Fragment>& f, vector<container>& content);   
	void enter(shared_ptr<Fragment>& fragment);   
	bool isStop();   
	void stop();   
	void clearQueue();   
};   

class Play   
{   
public:   
	string currCharacter;   
	Play(vector<string>& n);   
	void print_first_scene();   
	void recite(vector<container>::iterator& iter, unsigned int& scene_fragment_number);   
	int enter(shared_ptr<Fragment> f, bool stop = false);    
	int exit(shared_ptr<Fragment> f, bool stop = false);   
	void reset();   
};   
In our main method, we called Director constructor, then when preparing, we call SinglePlay structor and Player constructor with input player object. Since we used Play object in player class, we will include Play class in Player class and have a neat heirachy.   

2. We have made a Fragment object structor in common.h which stores all our common variables. This Fragment structor has been used in many places to indicate the current processing fragment contents. In Play class, we used it in our enter() and exit() method to compare them with the scene fragment counter and also check the names of characters to indicate their enters and exits. In Player class, fragment object is crucial as it made up of our working queue. We will read fragment by fragment as long as it is popped. See details in description of Player class in the overview section. Also in Director class, this is where we construct the Fragment and store it in our working queue for later usage.   

3. We have used ACE package, and have some virtual method in Producer class and Connection class in director.cpp.   

Insights, Observations & Questions:   
1. Since this lab is to handle multiple plays, instead of using many vectors, we have constructed a SinglePlay structor to store information of each play with an ID, which is easier for commuication between director and producer.   
2. When we first run our program, we have encounter segmentation fault error, we realize it is the problem with freeing memory in the program before it exits. Thus, we have made many resetting commands in the program such as reset() in Play class, and clearQueue() in Player class and several clearing of data in Director class.   
3. If a stop signal is received, we will make sure all players exit and then reset all variables included in Play, Player classes and Fragment structor.   
4. Each Producer and Director has two socket connection: one for sending message(connector) and one for receiving message(acceptor). The producer's receiving socket and all director's sending socket is at local address with port 1025. Director's receiving address is specified by user, and it will sending its receiving address along with its play list to producer when it established, as well as open the acceptor at corresponding receiving address. The producer will then set up connection on the address and port that director specified, and save all connection information for each director in the Connection struct. One connection represents one director.   
   
---------- Part 2 ------------   
Unpacking Instructions:   
1. Log in into remote desktop and download the Lab3_Full zip file in the email.   
2. Open the downloaded and extracted Lab3_Full directory and find lab3.zip, and right click on the zip file, select: 7-zip -> Extract Here.    
3. Open the terminal and log in to remote linux lab, by using "ssh [username]@linuxlab009.engr.wustl.edu" and inputting your password for wustl key.   
----- Producer Server Setup -----   
4. Direct to \lab3_Full\lab3\Producer directory and make sure you can see the "Makefile", then followed either by 4a) or 4b) below to enable -std=c++17 worked.   
a). Type in "module add gcc-8.3.0" in command line.   
b). Open ~/.bash_profile and ~/.bashrc files, add line "module add gcc-8.3.0", save and exit.   
5. Before running "make", make sure to install ACE package, following Step 2 in https://www.cse.wustl.edu/~cdgill/courses/cse532/networked_concurrency.html, and copy lab3 folder to the same folder as your $ACE_ROOT.   
6. Now, type "make", you should see "g++ -L/<YOUR ACE ROOT>/ace -I/<YOUR ACE ROOT> -lACE -Wall -std=c++17 -pthread -DTEMPLATE_HEADERS_INCLUDE_SOURCE -o Producer Producer.cpp" formed automatically.   
----- Director Servers Setup -----   
7. Open other terminals as many as you want for the amount of Directors you want, Do Step 3 and make sure you have done Step 5.   
8. Direct to \lab3_Full\lab3\Director directory and make sure you can see the "Makefile", then followed either by 8a) or 8b) below to enable -std=c++17 worked.   
a). Type in "module add gcc-8.3.0" in command line.   
b). Open ~/.bash_profile and ~/.bashrc files, add line "module add gcc-8.3.0", save and exit.   
9. Now, type "make" for each terminal, you should see "g++ -L/<YOUR ACE ROOT>/ace -I/<YOUR ACE ROOT> -lACE -Wall -std=c++17 -pthread -DTEMPLATE_HEADERS_INCLUDE_SOURCE -o Director Director.cpp Play.cpp Player.cpp" formed automatically.   
10. After successfully make, type in "./Producer" in Producer server terminal window.   
11. Type in "./Director <port> <ip_address> <min_threads> <script_file>..." in Director server terminal windows.   
eg: ./Director 2000 127.0.0.1 10 partial_hamlet_act_ii_script.txt partial_macbeth_act_i_script.txt   
(make sure don't put 127.0.0.1:1025 as address since this is for producer's receiving address)   
12. Then, you can type in "start <id>", "stop <id>", and "quit" in Producer window.   
13. Or, you can Ctrl-C in any of the terminal windows created above.   
