Hex Game

Implementation by Luiz Ramos

In this implementation enables different player games: human vs human, human vs
computer, and computer vs computer.  The computer version computes the best
next move based on Monte Carlo simulations (the more iterations, the better the
move, but the longer it takes).  In the future, I plan on adding a faster AI,
but leveraging pruning techniques.

Rules, from Wikipedia: each player  has an allocated color, Red and
Blue (...) Players  take turns placing a stone of  their color on a
single cell  within the  overall playing board.  The goal  for each
player is to form a connected  path of their own stones linking the
opposing sides  of the board  marked by their colors,  before their
opponent connects his or her  sides in a similar fashion. The first
player to  complete his or her  connection wins the  game. The four
corner  hexagons each  belong to  both adjacent  sides.   Since the
first player to move in Hex  has a distinct advantage, the pie rule
is generally implemented for  fairness. This rule allows the second
player to choose whether to  switch positions with the first player
after the first player makes the first move.
