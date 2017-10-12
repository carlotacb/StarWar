#ifndef Player_hh
#define Player_hh

#include "Utils.hh"
#include "Board.hh"
#include "Action.hh"
#include "Registry.hh"

using namespace std;


/***
 * Abstract base class for players.
 * *
 * This class uses multiple inheritance from Board
 * and Action, so that their public operations can been used
 * from within Player.
 *
 * In order to create new players, inherit from this class
 * and register them. See the examples AINull.cc and AIDemo.cc.
 */

class Player : public Board, public Action {

public:

  /**
   * Return the number of my player.
   */
  int me ();


  /****************************************************/
  /***************** STOP READING HERE ****************/
  /****************************************************/

private:  
  
  friend class Game;
  friend class SecGame;

  int me_;

  inline void reset (int me, const Board& board, const Action& action) {
    me_ = me;
    *(Board*)this = board;
    *(Action*)this = action;
  }


public:

  void move(Starship_Id s, Dir d) {
    if (not starship_ok(s))
      cerr << "warning: invalid starship identifier" << s << endl;
    else if (not dir_ok(d))
      cerr << "warning: invalid direction " << d << " for starship " << s << endl;
    else if (player_of(s) != me())  {
      cerr << "warning: starship does not belong to commanding player" << endl;
    }
    else {
      Action::move(s, d);
    }
  }

  void shoot(Starship_Id s) {
    if (not starship_ok(s))
      cerr << "warning: invalid starship identifier" << s << endl;
    else if (player_of(s) != me())  {
      cerr << "warning: starship does not belong to commanding player" << endl;
    }
    else {
      Action::shoot(s);
    }
  }

  /**
   * My play intelligence.
   * Will have to be overwritten, thus declared virtual.
   */
  virtual void play () {
  };

};

inline int Player::me () {
  return me_;
}

#endif
