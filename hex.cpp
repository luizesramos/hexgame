//--------------------------------------------------------------------
// Hex Game
// author: Luiz Ramos

#include <iostream> // cout, cin, vector etc
#include <cstdlib> // system("clear")
#include <cassert> // assert
#include "hexboard.hpp"
#include "cursor.hpp"
#include "player.hpp"
#include "aiplayer.hpp"
using namespace std;

// select player types
void select_players(HexBoard &board, Player* &p1, Player* &p2) {
  cout << "Select game type:" << endl
       << "1 - Computer(X) vs (O)Human" << endl
       << "2 -    Human(X) vs (O)Computer" << endl
       << "3 -    Human(X) vs (O)Human" << endl
       << "4 - Computer(X) vs (O)Computer" << endl;

  Cursor cur;
  int code;
  while(true) {
    code = static_cast<int>(cur.read()) - static_cast<int>('0');
    if(code >= 1 && code <= 4)
      break;
  }

  // selecting player1
  if(code == 1 || code == 4) {
    p1 = new AIMonteCarloPlayer("Player1",&board);
    //static_cast<AIMonteCarloPlayer*>(p1)->set_trials(100);
  } else {
    p1 = new ArrowHumanPlayer("Player1", &board);
  }

  // selecting player2
  if(code == 2 || code == 4) {
    p2 = new AIMonteCarloPlayer("Player2",&board);
    //static_cast<AIMonteCarloPlayer*>(p2)->set_trials(100);
  } else {
    p2 = new ArrowHumanPlayer("Player2", &board);
  }
}

// draws the board and prepares the game for a next move
void clear_screen(HexBoard& board) {
  system("clear"); // compatible with linux/unix
  cout << "#################################################################\n"
       << "# Hex Game\n"
       << "#################################################################\n"
       << board
       << "\r#################################################################"
       << endl;
}

// at the end of a match we ask if the human wants another match
bool end_game(HexBoard& board, Player* &p1, Player* &p2) {
  Cursor cur;
  Key code;
  while(true) {
    cout << "Continue(y/n) or change players (c): ";
    code = cur.read();

    // play again: reset board and players
    if(code == static_cast<Key>('y')) {
      board.reset_board();
      p1->reset();
      p2->reset();
      return false; // continue

    // delete and reselect players
    } else if(code == static_cast<Key>('c')) {
      delete p1;
      delete p2;
      clear_screen(board);
      board.reset_board();
      select_players(board, p1, p2);
      return false; // continue after selecting new player

    // quit game
    } else if(code == static_cast<Key>('n')) {
      clear_screen(board);
      cout << endl << "Thanks for playing! Bye!" << endl << endl;
      // delete players
      delete p1;
      delete p2;
      return true; // quit
    }
  }
}

// actual gameplay
void start_game(HexBoard& board, Player *p1, Player *p2) {
  int x, y;

  // main loop of the game
  while(true) {
    // prepares the screen for the next move
    clear_screen(board);

    // get move from current player
    Player *cur_player = (board.get_current_player() == 1) ? p1 : p2;
    cur_player->play(x,y);

    // tries to update the state of the HexBoard
    Outcome outcome = board.play(x,y);

    // evaluate if move was valid and if there were winners
    if(outcome != Outcome::NO_WIN) {
      // invalid plays
      if(outcome < Outcome::NO_WIN) {
        if(cur_player->is_interactive()) {
          cout << "\nInvalid move (" << x << "," << y << "): " 
               << outcome <<  endl;
          cin.get(); // pause execution
        }

      // one of the players wins the game
      } else {
        clear_screen(board);
        cout << endl << outcome << endl;
        cin.get(); // pause execution
        return;
      }
    } 
  }
}

int main() {
  // define board dimensions and create board
  HexBoard board(11);
  Player *p1, *p2;

  clear_screen(board);
  // instantiate two players via pointers
  select_players(board, p1, p2);

  // creates and starts the game
  do {
    // run the actual game
    start_game(board, p1, p2);

    // quit, continue or change player types?
  } while(!end_game(board,p1,p2));
}
