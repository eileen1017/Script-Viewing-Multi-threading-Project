all:
	g++ -L$(ACE_ROOT)/ace -I$(ACE_ROOT) -lACE -Wall -std=c++17 -pthread -DTEMPLATE_HEADERS_INCLUDE_SOURCE -o Director Director.cpp Play.cpp Player.cpp
clean:
	rm Director *~
