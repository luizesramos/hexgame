//--------------------------------------------------------------------
// Hex Game
// author: Luiz Ramos

// Rules, from Wikipedia: each player  has an allocated color, Red and
// Blue (...) Players  take turns placing a stone of  their color on a
// single cell  within the  overall playing board.  The goal  for each
// player is to form a connected  path of their own stones linking the
// opposing sides  of the board  marked by their colors,  before their
// opponent connects his or her  sides in a similar fashion. The first
// player to  complete his or her  connection wins the  game. The four
// corner  hexagons each  belong to  both adjacent  sides.   Since the
// first player to move in Hex  has a distinct advantage, the pie rule
// is generally implemented for  fairness. This rule allows the second
// player to choose whether to  switch positions with the first player
// after the first player makes the first move.
#ifndef HEXBOARD_HPP
#define HEXBOARD_HPP

#include <iostream> // cout, cin, vector etc
#include <iomanip> // setw
#include <cstdlib> // system("clear")
#include <cassert> // assert
#include "graph.hpp"
using namespace std;

// Blue: vertex taken by player1 or player1's margin (wall)
// Red: vertex taken by player2 or player2's margin (wall)
// Gray: invalid vertex (no one can move here)
// White: any player can move here and claim this cell
enum class Color: char {BLUE='X', RED='O', GRAY='*', WHITE='.'};

ostream& operator<<
(ostream& out, Color c) {
  out << static_cast<char>(c);
  return out;
}

//inline bool operator==
//(Color a, Color b) {
//  return static_cast<int>(a) == static_cast<int>(b);
//}

// possible outcomes for one play: 
// OCC_ERROR = position already occupied
// OOB_ERROR = position out of bounds
// NO_VICTORY = successsful play, no winner
// P1_WINS = player 1 wins
// P2_WINS = player 2 wins
enum class Outcome: int {OCC_ERROR=-2, OOB_ERROR, NO_WIN, P1_WIN, P2_WIN};

ostream& operator<<
(ostream& out, Outcome outcome) {
  switch(outcome) {
    case Outcome::OCC_ERROR: out << "Position already taken."; break;
    case Outcome::OOB_ERROR: out << "Position out of bounds."; break;
    case Outcome::NO_WIN: out << "Successful play, no winner."; break;
    case Outcome::P1_WIN: out << "Player1 wins!"; break;
    case Outcome::P2_WIN: out << "Player2 wins!"; break;
  }
  return out;
}

// transpose is a functor that converts an x,y coordinate into one index of
// graph vertex i. The conversion may use different limits and there may or not
// be an (x,y) offset involved.

class Transpose {
private:
  vertID ro, co, dim;
  
public:
  Transpose(vertID roffs, vertID coffs, vertID dim):
    ro(roffs), co(coffs), dim(dim) {}

  // makes a (x,y) coordinate into an index i
  vertID operator()(vertID row, vertID col) {
    //cout << x << " " << y << " " << dim;
    vertID i = (((row+ro)*dim) + (col+co)); 
    assert(i>=0);
    assert(i<(dim*dim));
    return i;
  }
};

// HexBoard:  in this  design, a  Hex board  is a  graph with  'color'
// vertex  labels  and  integer  weights.   The  idea  is  to  find  a
// color-aware  MST  at every  player  move  to  find a  path  between
// opposing walls  belonging to  the current  player. For  example, if
// there  is a  Red  MST,  player1 wins.   I  will add  interconnected
// 'margin vertices'  (representing the  walls) that are  connected to
// the actual board,  so that if I  have a 3x3 board,  the graph would
// actually be  5x5 (the  corner vertices are  invalid, the  upper and
// lower margins are  RED and the left and right  margins are BLUE). I
// address the  5x5 matrix  with absolute indices  and the  3x3 matrix
// with relative indices.

// To determine victory, I use a simple color-aware depth-first search
// algorithm from the  top-left to the bottom right of  the board (the
// specific coordinates  for the different players  are different, but
// the  principle is  the same).   This mechanism  replaced Dijkstra's
// algorithm from my  previous submission. DFS is  more efficient than
// Dijkstra  in this  case, because  we simply  look for  a path,  not
// necessarily the shortest one.

class HexBoard: public Graph<Color,int> {
private:
  // dimension of the square hex board with margins (visible+invisible)
  vertID abs_dim;
  // dimension of the square hex board without margins (visible-only)
  vertID rel_dim;

  // transpose functors for plotting the HexBoard
  // abs_pos: for building and debugging the board
  Transpose abs_pos;
  // rel_pos: for displaying the visible part of the board 
  Transpose rel_pos;

  bool p1_turn; // it's either player1's turn(true) or player2's turn(false)

  //void print(ostream& out) { print(out, abs_pos, abs_dim); }; // debug
  void print(ostream& out) { print(out, rel_pos, rel_dim); }; // game mode
  void print(ostream& out, Transpose& pos, vertID dim);

public:
  HexBoard(unsigned dim): 
    // initializes the dimensions excluding/including margins
    rel_dim(static_cast<vertID>(dim)),  // excludes margins
    abs_dim(static_cast<vertID>(dim+2)),// includes margins
    // intitializes the functors for printing
    // abs_pos: converts x,y into a graph vertex index including the margins
    abs_pos(Transpose(0,0,static_cast<vertID>(dim+2))), 
    // rel_pos: converts x,y into a graph vertex index excluding the margins
    rel_pos(Transpose(1,1,static_cast<vertID>(dim+2))),
    p1_turn(true) { // start with player1
    // validate parameters and build graph
    assert(rel_dim > 2);
    reset_board();
  }

  friend ostream& operator<<
  (ostream& out, HexBoard& b) {
    b.print(out);
    return out;
  }

  // builds a new board ready to begin playing
  void reset_board();
  // tries to play a move
  Outcome play(int row, int col);
  // return information about the current player
  int get_current_player() { return (p1_turn ? 1 : 2); }
  Color get_current_player_symbol() { 
    return (p1_turn ? Color::BLUE : Color::RED); 
  }

  // returns the dimension of the playable area of the board
  int get_playable_dim() { return static_cast<int>(rel_dim); }
  // fills the free vector with all blank positions on the board 
  void get_free_vertices(vector<vertID>& fvert);
  // translates a vertex number into a row,col coordinate
  void vertex_to_row_col(vertID vert, int& row, int& col);
  // copies over the state of all vertices
  void clone_board_state(HexBoard& other);
  // determines if the player with color 'sym' has won
  bool is_victory(Color sym);

  ~HexBoard() { clear(); }
};

// builds a new board ready to begin playing
void HexBoard::reset_board() {
  clear(); // if there was anything in the graph, remove it

  // add all vertices (including margins) initially as white 
  for(vertID i=0; i<(abs_dim * abs_dim); ++i) 
    add_vertex(Color::WHITE);

  // paint the vertices the right color
  for(vertID col=0; col<abs_dim; ++col) {
    // setting up the RED wall
    set_vertex_key(abs_pos(0,col),Color::RED);
    set_vertex_key(abs_pos(abs_dim-1,col),Color::RED);

    // setting up the BLUE wall
    set_vertex_key(abs_pos(col,0),Color::BLUE);
    set_vertex_key(abs_pos(col,abs_dim-1),Color::BLUE);

    // setting up the gray spots
    set_vertex_key(abs_pos(0,0),Color::GRAY);
    set_vertex_key(abs_pos(0,abs_dim-1),Color::GRAY);
    set_vertex_key(abs_pos(abs_dim-1,0),Color::GRAY);
    set_vertex_key(abs_pos(abs_dim-1,abs_dim-1),Color::GRAY);
  }

  // add the edges of the left, right, and bottom margins
  for(vertID row=0; row<abs_dim; ++row) {
    for(vertID col=0; col<abs_dim; ++col) {
      // horizontal edges
      if(col<abs_dim-1) 
        add_edge(abs_pos(row,col), abs_pos(row,col+1), 1);

      // downward edges
      if(row<abs_dim-1)
        add_edge(abs_pos(row,col), abs_pos(row+1,col), 1);

      // right to left diagonals
      if(col>0 && row<abs_dim-1) {
        //cout << abs_pos(row,col) << "<->" << abs_pos(row+1,col-1) << endl;
        add_edge(abs_pos(row,col), abs_pos(row+1,col-1), 1);
      }
    }

    // bottom row horizontal
    if(row<abs_dim-1)
      add_edge(abs_pos(row,abs_dim-1), abs_pos(row+1,abs_dim-1), 1);
  }

  p1_turn = true; // we always begin with player 1
}

// prints the complete graph or the visible board, depending on the
// functor passed in pos and the dimension in dim
void HexBoard::print(ostream& out, Transpose& pos, vertID dim) {
  string spacing = " ";

  // print upper col numbers
  out << "  ";
  for(vertID col=0; col<dim; ++col)
    out << "" <<setw(2) << col << "  ";
  out << endl;

  for(vertID row=0; row<dim; ++row) {
    cout << setw(2) << row << " ";

    for(vertID col=0; col<dim; ++col) {
      // print vertex
      out << get_vertex_key(pos(row,col));

      // print horizontal edges with righthand neighbor
      if(col < (dim-1))
        out << (is_adjacent(pos(row,col), pos(row,col+1)) ?
                " - " : "   ");
    }
    out << endl << spacing << "   ";
    spacing += " ";

    // print edges with bottom neighbors
    if(row < dim-1) {
      for(vertID col=0; col<dim; ++col) {
        //cout << row << "," << col << endl;
        if(col>0)
          out << (is_adjacent(pos(row,col), pos(row+1,col-1)) ?
                  "/ " : "  ");

        out << (is_adjacent(pos(row,col), pos(row+1,col)) ?
                "\\ " : "  ");
      }
    }
    out << endl << spacing;
    spacing += " ";
  }
}

// tries to fill a hex with the current player's color in the graph; checks for
// victory (path between the players walls).

Outcome HexBoard::play(int rowi, int coli) {
  vertID row = static_cast<vertID>(rowi);
  vertID col = static_cast<vertID>(coli);

  if(!(row>=0 && col>=0 && row<rel_dim && col<rel_dim))
    return Outcome::OOB_ERROR; // illegal move: out-of-bounds

  if(get_vertex_key(rel_pos(row,col)) != Color::WHITE)
    return Outcome::OCC_ERROR; // illegal move: position occupied

  // legal move: search for victory
  set_vertex_key(rel_pos(row,col), (p1_turn ? Color::BLUE : Color::RED));
  // checks if the last player to play has won (denoted by p1_turn)
  if(is_victory(get_current_player_symbol()))
    return (p1_turn ? Outcome::P1_WIN : Outcome::P2_WIN);

  // switch to opposite player
  p1_turn = !p1_turn; 

  return Outcome::NO_WIN; // legal move, no winner
}

// returns a list of free board positions (as graph vertices)
void HexBoard::get_free_vertices(vector<vertID>& fvert) {
  // removes any items from the list of free positions
  fvert.clear();
  
  // searches the list of playable positions for free positions
  for(vertID row=0; row<rel_dim; ++row) {
    for(vertID col=0; col<rel_dim; ++col) {
      if(get_vertex_key((rel_pos(row,col))) == Color::WHITE)
        fvert.push_back(rel_pos(row,col));
    }
  }
}

// translates from vertex ID into a row and col coordinate
void HexBoard::vertex_to_row_col(vertID vert, int& row, int& col) {
  row = static_cast<int>(vert / abs_dim)-1;
  col = static_cast<int>(vert % abs_dim)-1;
}

// copies over the state of the vertices
void HexBoard::clone_board_state(HexBoard& other) {
  // make sure the boards have the same size
  assert(get_nodes() == other.get_nodes());
  for(vertID i=0; i<other.get_nodes(); ++i) {
    set_vertex_key(i, other.get_vertex_key(i));
    //cout << static_cast<int>(get_vertex_key(i)) << " ";
  }
}

// Using a color-aware depth-first search, determine if there is a path across
// the board, using the color of the player under evaluation.
bool HexBoard::is_victory(Color sym) {
  // find src and dst for the path of victory (if it exists)
  vertID src, dst;
  if(sym == Color::BLUE) {
    src = abs_pos(1,0); // top-left margin
    dst = abs_pos(abs_dim-2,abs_dim-1); // matching bottom-right margin
  } else {
    src = abs_pos(0,1); // top-left margin
    dst = abs_pos(abs_dim-1,abs_dim-2); // matching bottom-right margin
  }

  // keep track of visited nodes
  vector<bool> visited(get_nodes(), false);
  // implement a stack in a vector
  vector<vertID> stack;
  stack.push_back(src);
  // list of neighbors of cur (top of stack)
  vector<vertID> neigh;
  vertID top;

  while(!stack.empty()) {
    // get top of the stack
    top = stack.back();
    stack.pop_back();

    // find all neighbors of top of the stack
    get_neighbors(top, neigh);
    // check if we found dst or push neighbor
    for(auto p=neigh.begin(); p!=neigh.end(); ++p) {
      // have we found the destination node?
      if(*p == dst) {
        stack.clear();
        neigh.clear();
        visited.clear();
        return true;
      }

      // we haven't found dst yet, look at unvisited neighbors of the same
      // color.
      if(!visited[*p]) {
        visited[*p] = true;
        if(get_vertex_key(*p) == sym)
          stack.push_back(*p);
      }
    }

    neigh.clear();
  }

  // dst not found, we didn't win
  visited.clear();
  return false;
}

#if 0
void clear_screen(HexBoard& board) {
  system("clear"); // compatible with linux/unix
  cout << "######################################################################\n"
       << "# Hex Game\n"
       << "######################################################################\n"
       << board;
}

void end_game(HexBoard board) {
  char opt;

  while(true) {
    cout << "\nContinue (y/n): ";
    cin >> opt;
    cin.clear(); 
    cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
    if(opt == 'y') {
      board.reset_board();
      return;
    } else if(opt == 'n') {
      cout << "Thanks for playing! Bye!" << endl;
      exit(0);
   }
  }
}

// read input and ignore invalid input
void read_xy(int& x, int& y, HexBoard& board) {
  cout << "\nPlayer " << board.get_current_player()
       << " enter move (x y): ";

  cin >> x >> y;
  cin.clear(); 
  cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
}

void rand_xy(int& x, int& y, int dim) {
  x = rand()%dim;
  y = rand()%dim;
}

int main() {
  HexBoard board(11);
  int x, y;

  //while(true) {
  //  clear_screen(board);
  //  cin.get();
  //}

  while(true) {
    clear_screen(board);
    read_xy(x,y, board);
    //rand_xy(x,y,11);

    Outcome outcome = board.play(x,y);
    if(outcome != Outcome::NO_WIN) {
      if(outcome < Outcome::NO_WIN) {
        cout << "Invalid move (" << x << "," << y << "): " << outcome <<  endl;
        cin.get(); // pause execution
      } else {
        clear_screen(board);
        cout << endl << outcome << endl << endl;
        cin.get(); // pause execution
        end_game(board);
      }
    } 
  }
}
#endif
#endif
