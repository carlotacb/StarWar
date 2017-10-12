#ifndef Board_hh
#define Board_hh

#include "Utils.hh"
#include "Action.hh"

using namespace std;

/**
 * Enum type for all possible contents of a cell.
 */

enum CType {
  EMPTY,
  ASTEROID,
  STARSHIP,
  MISSILE,
  POINT_BONUS,
  MISSILE_BONUS
};


/**
 * Printing function for CType.
 */

ostream& operator << (ostream& ost, CType t);


/**
 * The description of a cell stores
 * its type (EMPTY | ASTEROID | STARSHIP | MISSILE | POINT_BONUS | MISSILE_BONUS),
 * a starship identifier (if type is STARSHIP, a value >= 0; otherwise, -1), and
 * a missile identifier  (if type is MISSILE,  a value >= 0; otherwise, -1),
 */

struct Cell {
  CType       type;
  Starship_Id  sid;
  Missile_Id   mid;
};


/**
 * The description of a starship stores
 * its starship identifier,
 * its position,
 * the number of available missiles,
 * whether it is alive or dead, and
 * the number of rounds to go before the starship can regenerate.
 */

struct Starship {
  Starship_Id sid;
  Pos         pos;
  int     nb_miss;
  bool      alive;
  int        time;
};


/**
 * The description of a missile stores
 * its missile identifier,
 * the identifier of the starship that shot it, and 
 * its position.
 */

struct Missile {
  Missile_Id  mid;
  Starship_Id sid;
  Pos         pos;
};


/**
 * The board.
 */

class Board {

public:

  /**
   * Empty constructor.
   */
  Board();


  /**
   * Returns the number of players in the game.
   */
  int number_players() const;


  /**
   * Returns the number of rounds in the game.
   */
  int number_rounds() const;


  /**
   * Returns the number of rows of the universe (and of the window).
   */
  int number_rows() const;


  /**
   * Returns the number of columns of the universe.
   */
  int number_universe_columns() const;


  /**
   * Returns the number of columns of the window.
   */
  int number_window_columns() const;


  /**
   * Returns the number of starships for each player.
   */
  int number_starships_per_player() const;


  /**
   * Returns the total number of starships.
   */
  int number_starships() const;

  
  /**
   * Returns the number of rounds to wait before a starship can regenerate.
   */
  int number_rounds_to_regenerate() const;


  /**
   * Returns the number of missile bonuses in the game.
   */
  int number_missile_bonuses() const;


  /**
   * Returns the number of point bonuses in the game.
   */
  int number_point_bonuses() const;


  /**
   * Returns the number of extra missiles obtained when getting a missile bonus.
   */
  int bonus_missiles() const;


  /**
   * Returns the number of extra points obtained when getting a point bonus.
   */
  int bonus_points() const;


  /**
   * Returns the number of points obtained when killing a starship of another player.
   */
  int kill_points() const;




  /**
   * Returns whether p is a valid position within the universe.
   */
  bool within_universe(const Pos& p) const;


  /**
   * Returns whether p is a valid position within the window at round r.
   */
  bool within_window(const Pos& p, int r) const;


  /**
   * Returns whether s is a valid starship identifier.
   */
  bool starship_ok(Starship_Id s) const;

  
  /**
   * Returns whether p is a valid player identifier.
   */
  bool player_ok(Player_Id p) const;


  /**
   * Returns the player that owns a given starship.
   */
  Player_Id player_of(Starship_Id s) const;


  /**
   * Returns the identifier of the first starship of a player.
   */
  Starship_Id begin(Player_Id p) const;

  /**
   * Returns the identifier of the "next" starship after the last
   * starship of a player (following STL iterators style).
   */
  Starship_Id end(Player_Id p) const;




  /**
   * Returns the current round (starting from 0).
   */
  int round() const;


  /**
   * Returns the cell at position p.
   */
  Cell cell(const Pos& p) const;


  /**
   * Returns the starship given its identifier.
   */
  Starship starship(Starship_Id s) const;


  /**
   * Returns the map from identifiers to missiles.
   */
  map<Missile_Id, Missile> missiles() const;


  /**
   * Returns the current score of a player.
   */
  int score(Player_Id p) const;


  /**
   * Returns the current cpu status of a player.
   * This is a value from 0 (used 0%) to 1 (used 100%), or -1 if player is frozen.
   * It only works in executions on Jutge.org:
   * in local executions 0 is always returned.
   */
  double status(Player_Id p) const;

  
  /**
   * Returns random number.
   */
  int randomize(void);

  
  /**
   * Returns a random number between l and u.  
   */
  int randomize(int l, int u);

  
  /****************************************************/
  /***************** STOP READING HERE ****************/
  /****************************************************/


private:

  // Allow access to the private part of Board.
  friend class Game;
  friend class SecGame;

  Random_generator rgen;

  // Game settings
  int nb_players;
  int nb_rnds;

  int nb_rows;
  int nb_uni_cols;
  int nb_win_cols;

  int nb_ships_x_player;
  int nb_rnds_regen;
  int nb_miss_bonuses;
  int nb_point_bonuses;
  int bonus_miss;
  int bonus_pnts;
  int kill_pnts;

  vector<string> names;


  // Game state
  int rnd;

  vector< vector<Cell> >    cells;
  vector<Starship>          ships;
  map<Missile_Id, Missile>  miss;
  vector<int>               scores;
  vector<double>            statuses;


  /**
   * Construct a board by reading from a stream.
   */
  Board(istream& is);


  /**
   * Returns the cell at position p.
   */
  Cell& cellPriv(const Pos& p);


  /**
   * Print the board settings to a stream.
   */
  void print_settings(ostream& os, bool secgame) const;


  /**
   * Print the board state to a stream.
   */
  void print_state(ostream& os) const;

  /**
   * Computes the next board after applying the given actions to the
   * current board. It also returns the actual actions performed.
   */
  Board next(const vector<Action>& asked, vector<Action>& done);

  /**
   * Applies an instruction.
   * Returns whether the instruction could be successfully applied.
   */
  bool apply(const Instruction& ins);

  /**
   * Moves a starship.
   * Returns whether the instruction could be successfully applied.
   */
  bool move_starship(Starship& s, Dir d);

  /**
   * Shoots a missile from the starship.
   * Returns whether the instruction could be successfully applied.
   */
  bool shoot(Starship& s);

  /**
   * Moves a missile.
   * Returns whether the instruction could be successfully applied.   
   */
  bool move_missile(Missile& m);

  /**
   * Places a new bonus at a random position.
   * Returns true if successful.
   */
  bool place_new_bonus(CType bonus);

  /**
   * Regenerates a dead starship and places it at a random position within the window.
   * Returns true if successful.
   */
  bool regenerate_starship(Starship& s);

  /**
   * Generates a random EMPTY position within the specified frame which has a square ball around it
   * of size n x n not containing cells of any type in s.
   * Returns true if successful.
   */
  bool random_empty_position(Pos up_left, Pos down_right, int n, const set<CType>& s, Pos& p);

  /**
   * Returns whether all cells within a square ball of size n x n centered at p
   * do not contain cells of any type in s.
   */
  bool is_free(Pos p, int n, const set<CType>& s);
  
  /**
   * Kills a starship.
   */
  void kill_starship(Starship& s);

  /**
   * Kills a missile.
   */
  void kill_missile(Missile& m);

  /**
   * Returns a new missile identifier.
   */
  Missile_Id new_missile_identifier() const;
  
  /**
   * Asserts invariants of the board. For debugging.
   */
  bool ok() const;
  
  // Returns a random permutation of {0, 1, ..., n-1}.
  vector<int> random_permutation(int n);

  /**
   * Sets random seed.
   */
  void srandomize(int s);
  

  /**
   * Returns a reference to the current cpu status of a player.
   */
  double& statusPriv(int p) {
    my_assert(player_ok(p));
    return statuses[p];
  }


  /**
   * Returns the name of a player.
   */
  string name(int p) const {
    my_assert(player_ok(p));
    return names[p];
  }

  int normalize(int j) const {
    j %= nb_uni_cols;
    if (j < 0) j += nb_uni_cols;
    return j;
  }

};


inline ostream& operator << (ostream& ost, CType t) {
  switch (t) {
  case EMPTY:           ost << "EMPTY";         return ost;
  case ASTEROID:        ost << "ASTEROID";      return ost;
  case STARSHIP:        ost << "STARSHIP";      return ost;
  case MISSILE:         ost << "MISSILE";       return ost;
  case POINT_BONUS:     ost << "POINT_BONUS";   return ost;
  case MISSILE_BONUS:   ost << "MISSILE_BONUS"; return ost;
  }
  return ost;
}

inline Board::Board() { }

inline int Board::number_players() const {
  return nb_players;
}

inline int Board::number_rounds() const {
  return nb_rnds;
}

inline int Board::number_rows() const {
  return nb_rows;
}

inline int Board::number_universe_columns() const {
  return nb_uni_cols;
}

inline int Board::number_window_columns() const {
  return nb_win_cols;
}

inline int Board::number_starships_per_player() const {
  return nb_ships_x_player;
}

inline int Board::number_starships() const {
  return number_starships_per_player() * number_players();
}

inline int Board::number_rounds_to_regenerate() const {
  return nb_rnds_regen;
}

inline int Board::number_missile_bonuses() const {
  return nb_miss_bonuses;
}

inline int Board::number_point_bonuses() const {
  return nb_point_bonuses;
}

inline int Board::bonus_missiles() const {
  return bonus_miss;
}

inline int Board::bonus_points() const {
  return bonus_pnts;
}

inline int Board::kill_points() const {
  return kill_pnts;
}

inline bool Board::within_universe(const Pos& p) const {
  int i = first(p);
  return 0 <= i and i < number_rows();
}

inline bool Board::within_window(const Pos& p, int r) const {

  int i = first(p);
  if (not(0 <= i and i < number_rows())) return false;

  int u = number_universe_columns();
  int w = number_window_columns();
  r = r % u;
  int j = second(p) % u;
  if (j < 0) j += u;
  if (r + w <= u)
    return r <= j and j < r + w;
  else
    return r <= j or j < r + w - u;
}

inline bool Board::starship_ok(Starship_Id s) const {
  return s >= 0 and s < number_starships();
}

inline bool Board::player_ok(Player_Id p) const {
  return p >= 0 and p < number_players();
}

inline Player_Id Board::player_of(Starship_Id s) const {
  my_assert(starship_ok(s));
  return s / number_starships_per_player();
}

inline Starship_Id Board::begin(Player_Id p) const {
  my_assert(player_ok(p));
  return p*number_starships_per_player();
}

inline Starship_Id Board::end(Player_Id p) const {
  my_assert(player_ok(p));
  return (p+1)*number_starships_per_player();
}

inline int Board::round() const {
  return rnd;
}

inline Cell Board::cell(const Pos& p) const {
  my_assert(within_universe(p));
  int i = first(p);
  int j = normalize(second(p));
  return cells[i][j];
}

inline Cell& Board::cellPriv(const Pos& p) {
  my_assert(within_universe(p));
  int i = first(p);
  int j = normalize(second(p));  
  return cells[i][j];
}

inline Starship Board::starship(Starship_Id s) const {
  my_assert(starship_ok(s));
  return ships[s];
}

inline map<Missile_Id, Missile> Board::missiles() const {
  return miss;
}

inline int Board::score(Player_Id p) const{
  my_assert(player_ok(p));
  return scores[p];
}

inline double Board::status(Player_Id p) const {
  my_assert(player_ok(p));
 return statuses[p];
}

inline bool Board::apply(const Instruction& ins) {
  my_assert(ships[ins.sid].alive);
  if (ins.type == MOVE)
    return move_starship(ships[ins.sid], ins.dir);
  else {
    my_assert(ins.type == SHOOT);
    my_assert(ins.dir  == DEFAULT);
    if (shoot(ships[ins.sid])) {
      bool res  = move_starship(ships[ins.sid], DEFAULT);
      my_assert(res);
      return true;
    }
    else return false;
  }
}

inline Missile_Id Board::new_missile_identifier() const {
  static int cnt = 0;
  return cnt++;
}

inline vector<int> Board::random_permutation(int n) {
  vector<int> v(n);
  for (int i = 0; i < n; ++i) v[i] = i;
  for (int i = 0; i < n; ++i) swap(v[i], v[randomize(i, n-1)]);
  return v;
}

inline int Board::randomize(void) {
  return rgen.randomize();
}

inline int Board::randomize(int l, int u) {
  return rgen.randomize(l, u);
}

inline void Board::srandomize(int s) {
  return rgen.srandomize(s);
}

#endif // Board_hh
