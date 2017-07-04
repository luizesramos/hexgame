//--------------------------------------------------------------------
// Cursor: reads characters from  the keyboard without pressing enter.
// Note: this code is not platform-independent
// author: Luiz Ramos

#ifndef CURSOR_HPP
#define CURSOR_HPP
#include <cstdio>
#include <iomanip>
#include <termios.h> // disable "enter" on cin
using namespace std;

// Key: special-purpose keys that the user.
enum Key: char  {UP=65, DOWN=66, RIGHT=67, LEFT=68, ESC=27, SPACE=32};

// Cursor: reads one character at a  time from stdin without having to
// press enter.
class Cursor {
private:
  // enables(on=true) or disables(on=false) the terminal state
  void set_terminal_state(bool on) {
    // obtain current terminal IO state
    struct termios curterm;
    tcgetattr(0, &curterm);

    // enable/disable buffered input (used in cin)
    if(on) {
      curterm.c_lflag |= (ICANON | ECHO);
    } else { 
      curterm.c_lflag &= ~(ICANON | ECHO);
    }

    // apply option
    tcsetattr(0, TCSANOW, &curterm);
  }

public:
  Key read() {
    // disable terminal buffer and echo
    set_terminal_state(false);

    // directional arrows 
    Key code = static_cast<Key>(getchar());
    if(code == Key::ESC) {
      //cout << "\n\nESC (" << code << ") ";
      code = static_cast<Key>(getchar());
      if(code == 91) {
        code = static_cast<Key>(getchar());
        //cout << "=> (" << code << ")\n";
      }
    }

    // enable terminal buffer and echo
    set_terminal_state(true);
    return code;
  }
};
#endif
