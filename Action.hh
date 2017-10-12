#ifndef Action_hh
#define Action_hh

#include "Utils.hh"

using namespace std;

struct Instruction;


/**
 * Class that stores all instructions requested by a player in a round.
 */

class Action {

public:

  /**
   * Creates an empty action.
   */
  Action();

  
  /**
   * Moves the specified starship in a given direction.
   * The movement should be such that the resulting position is within the window,
   * the starship should be alive,
   * the starship identifier and the direction should be ok,
   * the starship should belong to the commanding player,
   * and no other instruction should have been commanded to the starship before in the same round.
   * Otherwise the instruction is ignored.
   */
  void move(Starship_Id s, Dir d);

  
  /**
   * Makes the specified starship shoot a missile.
   * There should be at least one missile in the starship ammunition,
   * the starship should be alive,
   * the starship identifier should be ok,      
   * the starship should belong to the commanding player,
   * and no other instruction should have been commanded to the starship before in the same round.
   * Otherwise the instruction is ignored.
   */
  void shoot(Starship_Id s);

  
  /****************************************************/
  /***************** STOP READING HERE ****************/
  /****************************************************/

  
private:

  friend class Game;
  friend class SecGame;
  friend class Board;

  set<int>            sids;
  vector<Instruction> instructs;

  /**
   * Constructor reading one action from a stream.
   */
  Action(istream& is);

  /**
   * Print the action to a stream.
   */
  void print(ostream& os) const;
  
};


/**
 * Enum type for all possible instructions.
 */

enum IType {
  UNDEFINED, MOVE, SHOOT
};


/**
 * Conversion from IType to char.
 */
inline char i2c(IType a) {
  switch (a) {
  case MOVE     : return 'm';
  case SHOOT    : return 's';
  default       : return 'u';
  }
}


/**
 * Conversion from char to IType.
 */
inline IType c2i (char c) {
  switch (c) {
  case 'm':   return MOVE;
  case 's':   return SHOOT;
  default :   return UNDEFINED;
  }
}


/**
 * The description of an instruction stores
 * the identifier of the starship that is being commanded,
 * the type of the instruction (UNDEFINED | MOVE | SHOOT),
 * and the movement direction.
 */

struct Instruction {
  Starship_Id  sid;
  IType       type;
  Dir          dir;
};


inline Action::Action() {
}


inline void Action::move(Starship_Id s, Dir d) {
  if (sids.contains(s))
    cerr << "warning: instruction alread requested for starship " << s << endl;
  else {
    sids.insert(s);
    instructs.push_back({s, MOVE, d});
  }
}


inline void Action::shoot(Starship_Id s) {
  if (sids.contains(s))
    cerr << "warning: instruction alread requested for starship " << s << endl;
  else {
    sids.insert(s);
    instructs.push_back({s, SHOOT, DEFAULT});
  }
}


#endif // Action_hh
