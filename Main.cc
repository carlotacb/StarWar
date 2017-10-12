#include "Game.hh"
#include "Registry.hh"
#include "Defs.hh"

#include <fstream>
#include <getopt.h>

using namespace std;


void help (int argc, char** argv) {
  cout << "Usage: " << argv[0] << " [options] player1 player2 ... [< default.cnf] [> default.res] " << endl;
  cout << "Available options:" << endl;
  cout << "--seed=seed           -s seed      set random seed (default: 1)"      << endl;
  cout << "--input=file          -i input     set input file  (default: stdin)"  << endl;
  cout << "--output=file         -o output    set output file (default: stdout)" << endl;
  cout << "--list                -l           list registered players"           << endl;
  cout << "--version             -v           print version"                     << endl;
  cout << "--help                -h           print help"                        << endl;
}


void version () {
  cout << LONG_GAME_NAME << " v" << VERSION << endl;
  cout << "compiled " << __TIME__ << " " << __DATE__ << endl;
}

int seed;

int main (int argc, char** argv) {

    if (argc == 1) {
        help(argc, argv);
        return EXIT_SUCCESS;
    }

    struct option long_options[] = {
        {"seed",           required_argument,  0, 's'},
        {"input",          required_argument,  0, 'i'},
        {"output",         required_argument,  0, 'o'},
        {"list",           no_argument,        0, 'l'},
        {"version",        no_argument,        0, 'v'},
        {"help",           no_argument,        0, 'h'},
        {0, 0, 0, 0}
    };

    char* ifile = 0;
    char* ofile = 0;
    seed = -1;
    vector<string> names;

    while (true) {
        int option_index = 0;
        int c = getopt_long(
            argc, argv,
            "s:i:o:lvh",
            long_options, &option_index
        );

        if (c == -1) break;

        switch (c) {

            case 's':
                seed = atoi(optarg);
                break;

            case 'i':
                ifile = optarg;
                break;

            case 'o':
                ofile = optarg;
                break;

            case 'l':
                Registry::print_players(cout);
                return EXIT_SUCCESS;

            case 'v':
                version();
                return EXIT_SUCCESS;

            case 'h':
                help(argc, argv);
                return EXIT_SUCCESS;

            default:
                return EXIT_FAILURE;
    }   }

    while (optind < argc) {
        names.push_back(argv[optind++]);
    }

    if (seed < 0) seed = -seed;

    istream* is = ifile ? new ifstream(ifile) : &cin ;
    ostream* os = ofile ? new ofstream(ofile) : &cout;

    Game::run(names, *is, *os, seed);

    if (ifile) delete is;
    if (ofile) delete os;
}
