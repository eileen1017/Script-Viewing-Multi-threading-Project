// header file of play class

#if ! defined (PLAY_H)
#define PLAY_H
#include "../common.h"

struct container
{
	unsigned int order;
	string characterName;
	string text;

	bool operator<(const container& c);
};

class Play
{
private:
	mutex m;
	condition_variable cv;
	unsigned int line_counter;	// line counter of current fragment
	unsigned int scene_fragment_counter;	// fragment counter
	unsigned int on_stage;	// # of characters currently on stage
	vector<string> names;	// all scene names of current script
	vector<string>::iterator it;	// iterator for the scene names vector
public:
	int exit_players;	// number of players exist in one fragment
	static bool end;	// true if entire play is end
	string currCharacter;	// current active character of the play, "" if none
	Play(vector<string>& n);
	void print_first_scene();
	void recite(vector<container>::iterator& iter, unsigned int& scene_fragment_number);
	int enter(shared_ptr<Fragment> f, bool stop = false);
	int exit(shared_ptr<Fragment> f, bool stop = false);
	void reset();
};

#endif /* defined PLAY_H */
