//--------------------------------------------------------------------
//Player:  abstract  class that  defines  a  basic player,  with  some
//derivate classes (including a  human/keyboard version and a computer
//version).
// author: Luiz Ramos

#ifndef PLAYER_HPP
#define PLAYER_HPP
#include <iostream>
#include <limits>   // std::numeric_limits
#include "cursor.hpp"
using namespace std;

//-------------------------------------------------------------------
// Player: abstract class  for a human player or any  kind of computer
// player.
class Player {
protected:
  string name;
  HexBoard *board;

public:
  Player(const char* nm, HexBoard *b): name(nm), board(b) {}

  // by default, our players are not interactive (we don't want to see error
  // messages or have events that pause the game).
  virtual bool is_interactive() { return false; }

  // player settles for a move into position (row,col)
  virtual void play(int& row, int& col) = 0;

  // resets the player state, if necessary
  virtual void reset() {}
  virtual ~Player() { name.clear(); }
};

//-------------------------------------------------------------------
// DefaultHumanPlayer: we read a move from the keyboard as a "row col"
// coordinate.

class DefaultHumanPlayer: public Player {
public:
  DefaultHumanPlayer(const char* nm, HexBoard *b): Player(nm, b) {}

  // we show messages for all human players
  virtual bool is_interactive() { return true; }


  // collects the player's move information
  void play(int& row, int& col) {
    cout << "\n" << name
         << " enter move (row col): ";
    cin >> row >> col;
    cin.clear();
    cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
  }
};

//-------------------------------------------------------------------
// ArrowHumanPlayer: we  read a  move from the  keyboard as  an arrow,
// relative to the last move. We start from the center of the

class ArrowHumanPlayer: public Player {
private:
  // current row and column cursor positions of this player
  int y, x; 

  // verifies if the movement is within the playable boundary of the board.
  // if so, commit the movement to the player's cursor.
  void commit_if_valid(int row, int col) {
    if((row >= 0 && row < board->get_playable_dim()) &&
       (col >= 0 && col < board->get_playable_dim())) {
      y = row;
      x = col;
    }
  }

public:
  ArrowHumanPlayer(const char* nm, HexBoard *b): Player(nm, b) {
    // our player starts at the center of the board
    reset();
  }


  // we show messages for all human players
  virtual bool is_interactive() { return true; }

  void play(int& row, int& col) {
    Cursor cur;
    Key code;
    cout << endl;
    while(true) {
      cout << "\r" << name << " at (" 
           << y << "," 
           << x << ")"
           << " use arrows to browse and SPACE to select move.   ";

      code = cur.read();
      switch(code) {
        // move cursor around until we hit space
        case Key::UP:   commit_if_valid(y-1, x); break;
        case Key::DOWN: commit_if_valid(y+1, x); break;
        case Key::LEFT: commit_if_valid(y, x-1); break;
        case Key::RIGHT:commit_if_valid(y, x+1); break;
        // space finishes the play
        case Key::SPACE: row=y; col=x; return;
      }
    }
  }

  void reset() {
    // our player starts at the center of the board
    y = x = board->get_playable_dim()/2; 
  }
};

//-------------------------------------------------------------------
//AIRandomPlayer:  generates random valid  moves, without  worrying if
//the board position is free.

class AIRandomPlayer: public Player {
public:
  AIRandomPlayer(const char* nm, HexBoard *b): Player(nm,b) {}
  void play(int& row, int& col) {
    row = rand()%board->get_playable_dim();
    col = rand()%board->get_playable_dim();
  }
};
#endif
