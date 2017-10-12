
#include "Player.hh"

using namespace std;


/**
 * Write the name of your player and save this file
 * as AI<name>.cc
 */
#define PLAYER_NAME Demo





struct PLAYER_NAME : public Player {


    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }


    /**
     * Attributes for your player can be defined here.
     */

  vector<Dir> all_dirs;

  // If the cell following some direction is within
  // the window in the next round and has a bonus, move there.
  // Returns whether a successful movement could be found.
  bool move_to_bonus(const Starship& s) {
    for (Dir d : all_dirs)
      if (within_window(s.pos + d, round()+1)) {
          CType t = cell(s.pos + d).type;
          if (t == MISSILE_BONUS or t == POINT_BONUS) {
            move(s.sid, d);
            return true;
          }
      }
    return false;
  }

  bool up(Dir d)   { return d == SLOW_UP   or d == UP   or d == FAST_UP;   }
  bool down(Dir d) { return d == SLOW_DOWN or d == DOWN or d == FAST_DOWN; }


    /**
     * Play method.
     *
     * This method will be invoked once per each round.
     * You have to read the board here to place your actions
     * for this round.
     *
     */
  virtual void play () {

    if (round() == 0) {
      // Initialize a vector with all allowed directions.
      all_dirs = {     UP, DEFAULT,      DOWN,
                  SLOW_UP,    SLOW, SLOW_DOWN,
                  FAST_UP,    FAST, FAST_DOWN};
    }

    // For each of my starships
    for (Starship_Id sid = begin(me()); sid != end(me()); ++sid) {

      Starship s = starship(sid);

      if (s.alive) { // if the starship is alive and

        Pos p = s.pos;

        // has missiles and there is an enemy in the cell ahead, shoot
        Cell ahead = cell(p + DEFAULT);
        if (s.nb_miss > 0 and ahead.type == STARSHIP) {
          Player_Id p = player_of(ahead.sid);
          if (p != me()) shoot(sid);
        }

        // otherwise
        else {
          // if a bonus can be reached move there
          bool success = move_to_bonus(s);

          // otherwise take a random direction if current row is not 0
          // nor number_rows()-1.
          if (not success) {
            int row = first(s.pos);
            if (row != 0 and row != number_rows()-1) {
              Dir d = all_dirs[randomize(0, all_dirs.size()-1)];
              if (within_window(s.pos + d, round()+1)) move(sid, d);
            }
          }
        }
      }
    }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
