#include <algorithm>
#include <iomanip>

#include "Board.hh"
#include "Action.hh"

#include <cctype>

using namespace std;


Board::Board (istream& is) {

  string s, version;

  // Game settings.
  is >> s >> version;
  my_assert(s == "starwar");
  my_assert(version == "v1");

  is >> s;
  my_assert(s == "secgame");
  is >> s;

  is >> s;
  my_assert(s == "seed");
  is >> s;
  
  is >> s >> nb_players;
  my_assert(s == "nb_players");
  my_assert(nb_players == 4);

  is >> s >> nb_rnds;
  my_assert(s == "nb_rnds");
  my_assert(nb_rnds >= 1);

  is >> s >> nb_rows;
  my_assert(s == "nb_rows");
  my_assert(nb_rows >= 10);

  is >> s >> nb_uni_cols;
  my_assert(s == "nb_uni_cols");
  my_assert(nb_uni_cols >= 10);

  is >> s >> nb_win_cols;
  my_assert(s == "nb_win_cols");
  my_assert(nb_win_cols >= 10);
  my_assert(nb_win_cols <= nb_uni_cols);

  is >> s >> nb_ships_x_player;
  my_assert(s == "nb_ships_x_player");
  my_assert(nb_ships_x_player >= 1);

  is >> s >> nb_rnds_regen;
  my_assert(s == "nb_rnds_regen");
  my_assert(nb_rnds_regen >= 1);

  is >> s >> nb_miss_bonuses;
  my_assert(s == "nb_miss_bonuses");
  my_assert(nb_miss_bonuses >= 0);

  is >> s >> nb_point_bonuses;
  my_assert(s == "nb_point_bonuses");
  my_assert(nb_point_bonuses >= 0);

  is >> s >> bonus_miss;
  my_assert(s == "bonus_miss");
  my_assert(bonus_miss >= 0);

  is >> s >> bonus_pnts;
  my_assert(s == "bonus_pnts");
  my_assert(bonus_pnts >= 0);

  is >> s >> kill_pnts;
  my_assert(s == "kill_pnts");
  my_assert(kill_pnts >= 0);

  names = vector<string>(nb_players);
  is >> s;
  my_assert(s == "names");
  for (Player_Id p = 0; p < nb_players; ++p) {
    is >> names[p];
    my_assert(names[p] != "");
  }


  // Game state.
  is >> s >> rnd;
  my_assert(s == "rnd");
  my_assert(rnd >= 0);
  my_assert(rnd < nb_rnds);

  // Reading auxiliary lines labelling columns (tens and units).
  is >> s;
  is >> s;

  cells = vector< vector<Cell> >(number_rows(), vector<Cell>(number_universe_columns()));

  // The map shows empty cells, asteroids and bonuses.
  // Starships and missiles are not drawn on the map but listed later.

  for (int i = 0; i < number_rows(); ++i) {

    // Reading auxiliary row label.
    is >> s;

    for (int j = 0; j < number_universe_columns(); ++j) {
      cellPriv({i, j}).sid = cellPriv({i, j}).mid = -1;
      char c;
      is >> c;
      switch (c) {
      case '.': cellPriv({i, j}).type = EMPTY;         break;
      case 'X': cellPriv({i, j}).type = ASTEROID;      break;
      case 'P': cellPriv({i, j}).type = POINT_BONUS;   break;
      case 'M': cellPriv({i, j}).type = MISSILE_BONUS; break;
      default:  my_assert(false);
      }
    }
  }


  is >> s;
  my_assert(s == "starships");

  // Reading auxiliary line.
  is >> s;  my_assert(s == "sid");
  is >> s;  my_assert(s == "row");
  is >> s;  my_assert(s == "column");
  is >> s;  my_assert(s == "nb_miss");
  is >> s;  my_assert(s == "alive");
  is >> s;  my_assert(s == "time");

  int n_all_ships = number_starships();
  ships = vector<Starship>(n_all_ships);
  for (int k = 0; k < n_all_ships; ++k) {

    int sid, i, j, m, t;
    char a;
    is >> sid >> i >> j >> m >> a >> t;

    j = normalize(j);
    
    my_assert(sid == k);
    my_assert(within_universe({i, j}));
    my_assert(m >= 0);
    my_assert(a == 'y' or a == 'n');
    my_assert(t >= 0);
    my_assert(a == 'n' or t == 0);
    my_assert(a == 'n' or cellPriv({i, j}).type == EMPTY);
    my_assert(cellPriv({i, j}).type != ASTEROID);

    ships[k].sid     = sid;
    ships[k].pos     = {i, j};
    ships[k].nb_miss = m;
    ships[k].alive   = (a == 'y');
    ships[k].time    = t;
    if (ships[k].alive)
      cellPriv({i, j}) = {STARSHIP, sid, -1};
  }


  is >> s;
  my_assert(s == "missiles");
  int n_missiles;
  is >> n_missiles;

  // Reading auxiliary line.
  is >> s;  my_assert(s == "mid");
  is >> s;  my_assert(s == "sid");
  is >> s;  my_assert(s == "row");
  is >> s;  my_assert(s == "column");

  for (int k = 0; k < n_missiles; ++k) {

    int mid, sid, i, j;
    is >> mid >> sid >> i >> j;

    j = normalize(j);
    
    my_assert(starship_ok(sid));
    my_assert(within_universe({i, j}));
    my_assert(cellPriv({i, j}).type == EMPTY);

    miss[mid] = {mid, sid, {i, j}};
    cellPriv({i, j}) = {MISSILE, -1, mid};
  }


  is >> s;
  my_assert(s == "players");

  // Reading auxiliary line.
  is >> s;  my_assert(s == "pid");
  is >> s;  my_assert(s == "score");
  is >> s;  my_assert(s == "status");

  scores = vector<int>   (number_players());
  statuses = vector<double>(number_players());

  for (int k = 0; k < number_players(); ++k) {

    int id, sc;
    double st;
    is >> id >> sc >> st;

    my_assert(id == k);
    my_assert(sc >= 0);
    my_assert(st >=-1);

    scores[k] = sc;
    statuses[k] = st;
  }

  assert(ok());
}

extern int seed;

void Board::print_settings(ostream& os, bool secgame) const {
  os << "starwar"          << "\t" << "v1"                         << endl;
  os << "secgame"          << "\t" << secgame                      << endl;
  os << "seed"             << "\t" << seed                         << endl;  
  os << "nb_players"       << "\t" << number_players()             << endl;
  os << "nb_rnds"          << "\t" << number_rounds()              << endl;
  os << "nb_rows"          << "\t" << number_rows()                << endl;
  os << "nb_uni_cols"      << "\t" << number_universe_columns()    << endl;
  os << "nb_win_cols"      << "\t" << number_window_columns()      << endl;
  os << "nb_ships_x_player"<< "\t" << number_starships_per_player()<< endl;
  os << "nb_rnds_regen"    << "\t" << number_rounds_to_regenerate()<< endl;
  os << "nb_miss_bonuses"  << "\t" << number_missile_bonuses()     << endl;
  os << "nb_point_bonuses" << "\t" << number_point_bonuses()       << endl;
  os << "bonus_miss"       << "\t" << bonus_missiles()             << endl;
  os << "bonus_pnts"       << "\t" << bonus_points()               << endl;
  os << "kill_pnts"        << "\t" << kill_points()                << endl;

  os << "names";
  for (Player_Id p = 0; p < number_players(); ++p) os << " " << name(p);
  os << endl << endl;
}


void Board::print_state(ostream& os) const {

  os << endl;
  os << "rnd " << round() << endl << endl;


  // Printing auxiliary lines labelling columns.
  os << "   ";
  for (int j = 0; j < number_universe_columns(); ++j) os << (j / 10);
  os << endl;

  os << "   ";
  for (int j = 0; j < number_universe_columns(); ++j) os << (j % 10);
  os << endl;
  os << endl;

  for (int i = 0; i < number_rows(); ++i) {
    os << (i / 10) << (i % 10) << " "; // Printing column labels.
    for (int j = 0; j < number_universe_columns(); ++j) {
      CType c = cell({i, j}).type;
      switch (c) {
      case ASTEROID:       os << 'X'; break;
      case POINT_BONUS:    os << 'P'; break;
      case MISSILE_BONUS:  os << 'M'; break;
      default:             os << '.'; break;
      };
    }
    os << endl;
  }
  os << endl << endl;



  os << "starships" << endl;
  os << "sid"      << "\t"
     << "row"     << "\t"
     << "column"  << "\t"
     << "nb_miss" << "\t"
     << "alive"   << "\t"
     << "time"    << endl;

  for (const Starship& s: ships)
    os << s.sid                    << "\t"
       << first( s.pos)            << "\t"
       << normalize(second(s.pos)) << "\t"
       << s.nb_miss                << "\t"
       << (s.alive ? 'y' : 'n')    << "\t"
       << s.time                   << endl;
  os << endl;



  os << "missiles"  << endl;
  os << miss.size() << endl;
  os << "mid"       << "\t"
     << "sid"       << "\t"
     << "row"       << "\t"
     << "column"    << endl;

  for (const auto& x: miss) {
    const Missile& m = x.second;
    os << m.mid                    << "\t"
       << m.sid                    << "\t"
       << first( m.pos)            << "\t"
       << normalize(second(m.pos)) << endl;
  }
  os << endl;



  os << "players" << endl;
  os << "pid"     << "\t"
     << "score"   << "\t"
     << "status"  << endl;

  for (Player_Id p = 0; p < number_players(); ++p)
    os << p         << "\t"
       << score(p)  << "\t"
       << status(p) << endl;

  os << endl;
}


Board Board::next (const vector<Action>& asked, vector<Action>& done) {

  assert(ok());

  // b will be the new board we shall return.
  Board b = *this;

  // Increment the round.
  ++b.rnd;

  // Move missiles.
  queue<Missile> q;
  for (const auto& x : b.miss)
    q.push(x.second);

  // Missiles should move continuously and at the same time,
  // but we move them discretely. We process them in a way
  // that they never crash with each other.
  while (not q.empty()) {
    Missile m = q.front();
    q.pop();
    if (b.cellPriv(m.pos + DEFAULT).type != MISSILE and
        b.cellPriv(m.pos +    FAST).type != MISSILE)
      b.move_missile(b.miss[m.mid]);
    else q.push(m);
  }

  // Sort players randomly.
  vector<int> random_players = random_permutation(number_players());

  // Handle commands of each player.
  vector<bool> commanded(number_starships(), false);
  for (int k = 0; k < number_players(); ++k) {
    Player_Id p = random_players[k];
    for (const Instruction& ins : asked[p].instructs)
      if (b.ships[ins.sid].alive) {
        my_assert(player_of(ins.sid) == p);
        my_assert(ins.type == MOVE or ins.type == SHOOT);
        if (b.apply(ins)) {
          commanded[ins.sid] = true;
          done[p].sids.insert(ins.sid);
          done[p].instructs.push_back(ins);
        }
      }
    for (Starship_Id s = begin(p); s != end(p); ++s)
      if (not commanded[s] and b.ships[s].alive)
        b.apply({s, MOVE, DEFAULT});
  }

  // Regenerate starships.
  for (Starship& s : b.ships)
    if (not s.alive) {
      if (s.time == 0) b.regenerate_starship(s);
      else --s.time;
    }

  // Place new bonuses.
  int nb_miss_bonuses_to_place  = nb_miss_bonuses;
  int nb_point_bonuses_to_place = nb_point_bonuses;
  for (int i = 0; i < number_rows(); ++i)
    for (int j = 0; j < number_universe_columns(); ++j) {
      if      (b.cellPriv({i, j}).type == MISSILE_BONUS) --nb_miss_bonuses_to_place;
      else if (b.cellPriv({i, j}).type == POINT_BONUS  ) --nb_point_bonuses_to_place;
    }

  while (nb_miss_bonuses_to_place  > 0 and b.place_new_bonus(MISSILE_BONUS))
    --nb_miss_bonuses_to_place;

  while (nb_point_bonuses_to_place > 0 and b.place_new_bonus(POINT_BONUS))
    --nb_point_bonuses_to_place;

  assert(b.ok());
  return b;
}


bool Board::move_starship(Starship& s, Dir dir) {

  my_assert(s.alive);
  my_assert(cellPriv(s.pos).type == STARSHIP);
  my_assert(cellPriv(s.pos).sid  == s.sid   );
  my_assert(cellPriv(s.pos).mid  == -1      );

  if (not within_window(s.pos + dir, round())) return false;

  if (dir == SLOW) return true;

  const auto& all = dir2all.at(dir);
  my_assert(not all.empty());
  for (Dir d : all) {
    my_assert(d != Dir(0, 0));
    Cell& c = cellPriv(s.pos + d);
    if (c.type == POINT_BONUS) {
      scores[player_of(s.sid)] += bonus_points();
      c.type = EMPTY;
    }
    else if (c.type == MISSILE_BONUS) {
      s.nb_miss += bonus_missiles();
      c.type = EMPTY;
    }
    else if (c.type == ASTEROID) {
      kill_starship(s);
      c.type = EMPTY;
      return true;
    }
    else if (c.type == STARSHIP) {
      Starship& t = ships[c.sid];
      my_assert(s.sid != t.sid);
      kill_starship(s);
      kill_starship(t);
      return true;
    }
    else if (c.type == MISSILE) {
      my_assert(miss.contains(c.mid));
      Missile m = miss[c.mid];
      if (player_of(m.sid) != player_of(s.sid))
        scores[player_of(m.sid)] += kill_points();
      kill_starship(s);
      kill_missile(m);
      return true;
    }
  }
  cellPriv(s.pos + dir) = {STARSHIP, s.sid, -1};
  cellPriv(s.pos      ) = {EMPTY,       -1, -1};

  s.pos += dir;
  s.pos.imag(normalize(s.pos.imag()));
  
  return true;
}


bool Board::shoot(Starship& s) {

  my_assert(s.alive);
  my_assert(cellPriv(s.pos).type == STARSHIP);
  my_assert(cellPriv(s.pos).sid  == s.sid   );
  my_assert(cellPriv(s.pos).mid  == -1      );

  if (s.nb_miss > 0) {
    --s.nb_miss;
    Missile_Id mid = new_missile_identifier();
    my_assert(not miss.contains(mid));
    miss[mid] = {mid, s.sid, s.pos};
    move_missile(miss[mid]);
    cellPriv(s.pos) = {STARSHIP, s.sid, -1 }; // Correcting.
    return true;
  }
  else return false;
}


bool Board::move_missile(Missile& m) {

  const auto& all = dir2all.at(FAST);
  my_assert(not all.empty());
  for (Dir d : all) {
    my_assert(d != Dir(0, 0));
    Cell& c = cellPriv(m.pos + d);
    my_assert(c.type != MISSILE);
    if (c.type == POINT_BONUS   or
        c.type == MISSILE_BONUS or
        c.type == ASTEROID) {
      kill_missile(m);
      c.type = EMPTY;
      return true;
    }
    else if (c.type == STARSHIP) {
      Starship& s = ships[c.sid];
      if (player_of(m.sid) != player_of(s.sid))
        scores[player_of(m.sid)] += kill_points();
      kill_starship(s);
      kill_missile(m);
      return true;
    }
  }
  cellPriv(m.pos + FAST) = {MISSILE, -1, m.mid};
  cellPriv(m.pos)        = {EMPTY,   -1, -1   };

  m.pos += FAST;
  m.pos.imag(normalize(m.pos.imag()));
  
  return true;
}


bool Board::place_new_bonus(CType bonus) {
  my_assert(bonus == POINT_BONUS or bonus == MISSILE_BONUS);
  Pos p;
  int r = round();
  int f = number_rows();
  int cu = number_universe_columns();
  int cw = number_window_columns();
  if (random_empty_position({0, r+cw}, {f-1, r+cu-1}, 5, {MISSILE, STARSHIP}, p)) {
    cellPriv(p) = {bonus, -1, -1};
    return true;
  }
  else return false;
}


bool Board::regenerate_starship(Starship& s) {
  Pos p;
  int r = round();
  int f = number_rows();
  int c = number_window_columns();
  if (random_empty_position({0, r}, {f-1, r+c-1}, 5, {MISSILE, STARSHIP, ASTEROID}, p)) {
    s.pos = p;
    s.alive = true;
    s.time = 0;
    cellPriv(p) = {STARSHIP, s.sid, -1};
    return true;
  }
  else return false;
}


bool Board::random_empty_position(Pos up_left, Pos down_right, int n, const set<CType>& s, Pos& p) {
  const int MAX_ATTEMPS = 20;
  for (int k = 0; k < MAX_ATTEMPS; ++k) {
    int i = randomize( first(up_left),  first(down_right));
    int j = randomize(second(up_left), second(down_right));
    p = {i, j % number_universe_columns() };
    my_assert(within_universe(p));
    if (cellPriv(p).type == EMPTY and is_free(p, n, s))
      return true;
  }
  return false;
}


bool Board::is_free(Pos p, int n, const set<CType>& s) {
  my_assert(n % 2 == 1);
  for (int i = first(p) - n/2; i <= first(p) + n/2; ++i)
    for (int j = second(p) - n/2; j <= second(p) + n/2; ++j) {
      Pos q = {i, j};
      if (within_universe(q)) {
        CType t = cellPriv(q).type;
        if (s.contains(t)) return false;
      }
    }
  return true;
}


void Board::kill_starship(Starship& s) {
  my_assert(s.alive);
  cellPriv(s.pos) = {EMPTY, -1, -1};
  s.alive = false;
  s.time = number_rounds_to_regenerate();
}


void Board::kill_missile(Missile& m) {
  cellPriv(m.pos) = {EMPTY, -1, -1};
  miss.erase(m.mid);
}


bool Board::ok() const {

  if (not(nb_players == 4)) {
    cerr << "debug: problems with parameter setting (0)." << endl;
    return false;
  }
  if (not(nb_rnds >= 1)) {
    cerr << "debug: problems with parameter setting (1)." << endl;
    return false;
  }
  if (not(nb_rows >= 10)) {
    cerr << "debug: problems with parameter setting (2)." << endl;
    return false;
  }
  if (not(nb_uni_cols >= 10)) {
    cerr << "debug: problems with parameter setting (3)." << endl;
    return false;
  }
  if (not(nb_win_cols >= 10)) {
    cerr << "debug: problems with parameter setting (4)." << endl;
    return false;
  }
  if (not(nb_win_cols <= nb_uni_cols)) {
    cerr << "debug: problems with parameter setting (5)." << endl;
    return false;
  }
  if (not(nb_ships_x_player >= 1)) {
    cerr << "debug: problems with parameter setting (6)." << endl;
    return false;
  }
  if (not(nb_rnds_regen >= 1)) {
    cerr << "debug: problems with parameter setting (7)." << endl;
    return false;
  }
  if (not(nb_miss_bonuses >= 0)) {
    cerr << "debug: problems with parameter setting (8)." << endl;
    return false;
  }
  if (not(nb_point_bonuses >= 0)) {
    cerr << "debug: problems with parameter setting (9)." << endl;
    return false;
  }
  if (not(bonus_miss >= 0)) {
    cerr << "debug: problems with parameter setting (10)." << endl;
    return false;
  }
  if (not(bonus_pnts >= 0)) {
    cerr << "debug: problems with parameter setting (11)." << endl;
    return false;
  }
  if (not(kill_pnts >= 0)) {
    cerr << "debug: problems with parameter setting (12)." << endl;
    return false;
  }
  if (not(rnd >= 0)) {
    cerr << "debug: problems with number of rounds (1)." << endl;
    return false;
  }
  if (not(rnd <= nb_rnds)) {
    cerr << "debug: problems with number of rounds (2)." << endl;
    return false;
  }

  if (int(cells.size()) != number_rows()) {
    cerr << "debug: problems with row dimension of the board." << endl;
    return false;
  }
  if (int(cells[0].size()) != number_universe_columns()) {
    cerr << "debug: problems with column dimension of the board." << endl;
    return false;
  }

  if (int(names.size()) != number_players()) {
    cerr << "debug: problems with number of players and names." << endl;
    return false;
  }
  for (const string& s : names)
    if (s == "") {
      cerr << "debug: names cannot be empty." << endl;
      return false;
    }

  int nb_miss_bonuses_on_board = 0;
  int nb_point_bonuses_on_board = 0;
  vector<int> nb_ships_on_board(number_players());
  for (int i = 0; i < number_rows(); ++i) {
    for (int j = 0; j < number_universe_columns(); ++j) {

      int sid = cells[i][j].sid;
      if (sid < -1 or sid > number_starships()) {
        cerr << "debug: problem with starship identifiers in cells." << endl;
        return false;
      }
      if (sid == -1 and cells[i][j].type == STARSHIP) {
        cerr << "debug: cell should have a valid starship identifier." << endl;
        return false;
      }
      if (sid != -1 and cells[i][j].type != STARSHIP) {
        cerr << "debug: cell should have an invalid starship identifier." << endl;
        return false;
      }
      
      int mid = cells[i][j].mid;
      if (mid < -1) {
        cerr << "debug: problem with missile identifiers in cells." << endl;
        return false;
      }
      if (mid == -1 and cells[i][j].type == MISSILE) {
        cerr << "debug: cell should have a valid missile identifier." << endl;
        return false;
      }
      if (mid != -1 and cells[i][j].type != MISSILE) {
        cerr << "debug: cell should have an invalid missile identifier." << endl;
        return false;
      }
      if (cells[i][j].type == MISSILE and not miss.contains(mid)) {
        cerr << "debug: missile map is corrupted." << endl;
        return false;
      }
      
      if      (cells[i][j].type == MISSILE_BONUS) ++nb_miss_bonuses_on_board;
      else if (cells[i][j].type == POINT_BONUS)   ++nb_point_bonuses_on_board;
      else if (cells[i][j].type == STARSHIP)      ++nb_ships_on_board[player_of(sid)];
    }
  }
  if (nb_miss_bonuses_on_board > nb_miss_bonuses) {
    cerr << "debug: problems with the number of missile bonuses" << endl;
    return false;
  }
  if (nb_point_bonuses_on_board > nb_point_bonuses) {
    cerr << "debug: problems with the number of point bonuses" << endl;
    return false;
  }
  for (int n : nb_ships_on_board)
    if (n > number_starships_per_player()) {
      cerr << "debug: problems with the number of starships" << endl;
      return false;
    }

  if (int(ships.size()) != number_starships()) {
      cerr << "debug: problems with the size of ships" << endl;
      return false;
  }

  vector<int> alive(number_players());
  for (int k = 0; k < int(ships.size()); ++k) {
    const Starship& s = ships[k];
    if (k != s.sid) {
      cerr << "debug: ships is in an inconsistent state" << endl;
      return false;
    }
    Player_Id p = player_of(s.sid);
    if (s.alive) {
      ++alive[p];
      if (cell(s.pos).type != STARSHIP) {
        cerr << "debug: problems with the correspondence between cells and ships" << endl;
        return false;
      }
    }
    if (s.nb_miss < 0) {
      cerr << "debug: number of available missiles cannot be negative" << endl;
      return false;
    }
    if (s.time < 0) {
      cerr << "debug: time cannot be negative" << endl;
      return false;
    }
    if (s.alive and s.time > 0) {
      cerr << "debug: found inconsistency between alive and time" << endl;
      return false;
    }
  }
  for (Player_Id p = 0; p < number_players(); ++p)
    if (nb_ships_on_board[p] != alive[p]) {
      cerr << "debug: problems with the number of alive ships" << endl;
      return false;
    }

  for (const auto& x : miss) {
    const Missile& m = x.second;
    if (x.first != m.mid) {
      cerr << "debug: miss is in an inconsistent state" << endl;
      return false;
    }
    if (not starship_ok(m.sid)) {
      cerr << "debug: missile has an invalid starship identifier" << endl;
      return false;
    }
    if (cell(m.pos).type != MISSILE) {
      cerr << "debug: problems with the correspondence between cells and missiles" << endl;
      return false;
    }
  }

  if (int(scores.size()) != number_players()) {
    cerr << "debug: problems with number of players and scores." << endl;
    return false;
  }
  for (int s : scores)
    if (s < 0) {
      cerr << "debug: there cannot be negative scores." << endl;
      return false;
    }

  if (int(statuses.size()) != number_players()) {
    cerr << "debug: problems with number of players and status." << endl;
    return false;
  }
  for (int s : statuses)
    if (s != -1 and not(0 <= s and s <= 1)) {
      cerr << "debug: problems with the status." << endl;
      return false;
    }

  return true;
}
