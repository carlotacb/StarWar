#include "Action.hh"
#include "Board.hh"

using namespace std;

Action::Action(istream& is) {
  string s;
  while (getline(is, s)) {
    istringstream iss(s);    
    int sid;
    while (iss >> sid) {
      if (sid == -1) return;
      char c;
      int di, dj;
      iss >> c >> di >> dj;
      instructs.push_back({sid, c2i(c), {di, dj}});
      sids.insert(sid);
    }
  }
}
  

void Action::print(ostream& os) const {
  for (const Instruction& ins : instructs)
    os << ins.sid << " " << i2c(ins.type) << " " << first(ins.dir) << " " << second(ins.dir) << endl;
  os << -1 << endl;
}
