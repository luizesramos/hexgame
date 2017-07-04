//--------------------------------------------------------------------
//Player:  abstract  class that  defines  a  basic player,  with  some
//derivate classes (including a  human/keyboard version and a computer
//version).
//author: Luiz Ramos

#ifndef AIPLAYER_HPP
#define AIPLAYER_HPP
#include <iostream>
#include <algorithm> // shuffle
#include <random>    // default_random_engine
#include <chrono>    // chrono::system_clock
#include "cursor.hpp"
#include "player.hpp"
using namespace std;

//-------------------------------------------------------------------
// AIMonteCarloPlayer: uses the Monte Carlo  method to select the next
// move. To make a move, the steps  I follow are: (1) initially have a
// scratchpad board  (another instance of  the board, where  we simply
// update the states  of the vertices, according to  the current state
// of the gameboard); (2) obtain a list of the free positions; (3) for
// each  free position,  assume  it  is fixed  and  do  a Monte  Carlo
// simulation to  find out how many  times it would win;  (4) pick the
// move  that provides  the  largest number  of  successes across  all
// simulations.

// A  Monte Carlo  simulation  consists of  the  following steps:  (1)
// assume that we will make one move into a free board position (fixed
// move), so  we mark that position  on the scratchpad board  with the
// symbol of the current player; (2) create a copy of the list of free
// positions without  the fixed  move (temp);  (3) shuffle  the 'temp'
// list and  traverse it once,  assigning moves to  alternate players,
// begining with  the symbol  of the opponent;  (4) evaluate  (using a
// color-aware depth-first search from top-left to bottom-right to see
// if the current player won  (update victories accordingly); (5) undo
// the  simulation  moves (except  the  fixed  move); for  efficiency,
// traverse 'temp' again  filling the positions with  blank again; (6)
// go to  step (3) until  we reach the  desired number of  trials; (7)
// return the number of victories across all number of trials.

// In my evaluations, with 1000 trials per Monte Carlo simulation, the
// computer takes  about 25  seconds on a  single-core Atom  1.5GHz to
// make a move. However, with 200 trials, the computer makes a move in
// about  3 seconds,  without noticeable  loss in  the quality  of its
// playing  decisions. On  a  Core i5  2.5GHz,  1000 iterations  takes
// around 6 seconds, whereas 100 iterations are almost instantaneous.

class AIMonteCarloPlayer: public Player {
private:
  // random number generator
  default_random_engine gen;
  // scratchboard copy of the gameboard
  HexBoard gcopy;
  // number of iterations in monte carlo simulations
  int trials;
  // iterate over the list of free vertices and return the number of wins
  int simulate(vertID curmove, vector<vertID>& others);
  // determines if a player won, given that the board is completely full
  bool is_victory(Color sym);

public:
  AIMonteCarloPlayer(const char* nm, HexBoard *b): 
    // initializes the superclass
    Player(nm,b),
    // initializes the random number generator with the current time
    gen(chrono::system_clock::now().time_since_epoch().count()),
    // create a scratchpad hex board
    gcopy(b->get_playable_dim()),
    trials(1000) {}

  void play(int& row, int& col);

  // sets the number of iterations in monte carlo simulations
  void set_trials(int t) { trials = t; }

  ~AIMonteCarloPlayer() { gcopy.clear(); }
};

// Performs  a monte  carlo  simulation for  1  move. Because  integer
// operations are typically more  efficient and easily comparable than
// floating-point operations,  I simply return and  compare the number
// of  successful  outcomes,  rather than  calculating  a  probability
// (successes/trials).

int AIMonteCarloPlayer::simulate(vertID curmove, vector<vertID>& fvert) {
  // counter of wins
  int wins = 0;

  // find all elements of fvert that are not curmove
  vector<vertID> tmp;
  for(auto p=fvert.begin(); p!=fvert.end(); ++p) {
    if(*p != curmove)
      tmp.push_back(*p);
  }

  // copy over the current board state
  gcopy.clone_board_state(*board); 
  // determing the symbol of the current playera and opponent
  Color me = board->get_current_player_symbol();
  Color op = (me==Color::BLUE ? Color::RED : Color::BLUE);
  // pretend we made a move at curmove
  gcopy.set_vertex_key(curmove, me);

  // for a specified number of trials
  for(int i=0; i<trials; ++i) {
    // shuffle the remaining free positions of the board
    shuffle (tmp.begin(), tmp.end(), gen);

    // fill the scratchpad graph (alternating the player symbols
    for(int j=0; j<tmp.size(); ++j)
      gcopy.set_vertex_key(tmp[j], ((j%2)==0) ? op : me);

    // see if 'me' won and update wins if necessary
    if(gcopy.is_victory(me))
      wins++;

    // partially undo graph filling (leave curmove)
    for(auto p=tmp.begin(); p!=tmp.end(); ++p)
      gcopy.set_vertex_key(*p, Color::WHITE);
  }

  // undo curmove change
  gcopy.set_vertex_key(curmove, Color::WHITE);

  tmp.clear();
  return wins;
}

// Uses Monte Carlo simulations to determine the next best move.
void AIMonteCarloPlayer::play(int& row, int& col) {
  // find the list of free vertices (still playable)
  vector<vertID> fvert;
  board->get_free_vertices(fvert);
  assert(!fvert.empty());

  // current winner and the highest number of wins so far
  vertID winner = 0;
  int hiwins = 0, wins;

  // progress counter
  int now=0, target=fvert.size();

  // For  each playable  move  (in  fvert) we  perform  a Monte  Carlo
  // simulation (iterate 1000 times and compute the number of times we
  // won). Compare the  number of wins to find the  winning move. Show
  // the  thinking progress  of  the computer,  since  this number  is
  // deterministic.

  for(auto p=fvert.begin(); p!=fvert.end(); ++p) {
    now++;
    cout << "\r" << name << " thinking..." << ((now*100)/target) << "%   ";
    wins = simulate(*p, fvert);
    if(wins > hiwins) {
      hiwins = wins;
      winner = *p;
    }
    //cout << "P[" << *p << "] = " << wins  
    //     << " hiwins:" << hiwins 
    //     << " winner:" << winner
    //     << endl;
    cout << "\r" << name << " thinking..." << ((now*100)/target) << "%   ";
  }
  cout << endl;

  // return the corresponding row,col coordinates of winner
  assert(winner != 0); // 0 is an invalid playable position
  board->vertex_to_row_col(winner,row,col);
}
#endif
