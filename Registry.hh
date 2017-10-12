
#ifndef Registry_hh
#define Registry_hh

#include "Utils.hh"

using namespace std;


class Player;

/**
 * Since the main program does not know how much players will be inherited
 * from the Player class, we use a registration and factory pattern.
 * Quite magic for beginners.
 */

class Registry {

public:

    typedef Player* (*Factory)();

    static int Register (const char* name, Factory fact);

    static Player* new_player (string name);

    static void print_players (ostream& os);
};



#define stringification(s) #s
#define RegisterPlayer(x) static int registration = Registry::Register(stringification(x), x::factory)


#endif
