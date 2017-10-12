#include "Game.hh"
#include "Registry.hh"


using namespace std;


void Game::run (vector<string> names, istream& is, ostream& os, int seed) {

  cerr << "info: loading game" << endl;
  Board b0(is);
  cerr << "info: loaded game" << endl;

  b0.srandomize(seed);
  
  if (int(names.size()) != b0.number_players()) {
    cerr << "fatal: wrong number of players." << endl;
    exit(EXIT_FAILURE);
  }

  vector<Player*> players;        
  for (int player = 0; player < b0.number_players(); ++player) {
    string name = names[player];
    cerr << "info: loading player " << name << endl;
    players.push_back(Registry::new_player(name));
    players[player]->srandomize(seed + player);
    b0.names[player] = name;
  }
  cerr << "info: players loaded" << endl;

  b0.print_settings(os, false);
  b0.print_state(os);

  for (int round = 0; round < b0.number_rounds(); ++round) {
    cerr << "info: start round " << round << endl;
    os << "actions_asked" << endl;
    vector<Action> asked;
    for (int player = 0; player < b0.number_players(); ++player) {
      cerr << "info:     start player " << player << endl;
      Action a;
      int s = players[player]->randomize();
      players[player]->reset(player, b0, a);
      players[player]->srandomize(s);
      players[player]->play();
      asked.push_back(*players[player]);

      os << endl << player << endl;
      Action(*players[player]).print(os);
      cerr << "info:     end player " << player << endl;
    }
    vector<Action> done(b0.number_players());
    // cerr << "info:     start next" << endl;
    Board b1 = b0.next(asked, done);
    // cerr << "info:     end next" << endl;

    os << endl << "actions_done" << endl;
    for (int player = 0; player < b0.number_players(); ++player) {
      os << endl << player << endl;
      done[player].print(os);
    }
    os << endl;

    b1.print_state(os);
    b1.srandomize(b0.randomize());
    b0 = b1;
    cerr << "info: end round " << round << endl;
  }

  vector<int> max_players;
  int max_score = -1;

  for (int player = 0; player < b0.number_players(); ++player) {
    cerr << "info: player " << b0.name(player) << " got score " << b0.score(player) << endl;
    if (max_score < b0.score(player)) {
      max_score = b0.score(player);
      max_players = vector<int>(1, player);
    }
    else if (max_score == b0.score(player))
      max_players.push_back(player);
  }
  cerr << "info: player(s)";
  for (int k = 0; k < int(max_players.size()); ++k)
    cerr << " " << b0.name(max_players[k]);
  cerr << " got top score" << endl;

  cerr << "info: game played" << endl;
}
