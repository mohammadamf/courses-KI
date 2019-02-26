//
// C++ program to play simple chess:
// white king (WK) and white queen/rook (WQ) against black king (BK)
// Compile: g++ -Wall -O2 -o chess chess.cc
// Run:     ./chess <thesize> <simulations> <maxgamelength> [0|1]
//          thesize: number of rows = columns of the game board
//          simulations: number of games played
//          maxgamelength: game is tied after this number of moves
//          [0|1]: 0 for white queen, 1 for white rook
// Walter Kosters, January 22, 2019; w.a.kosters@liacs.leidenuniv.nl
//
// Modified as part of an assignment by David Kleingeld, feb 14, 2019;

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>    // std::min

using namespace std;

const int MAX = 30;
const bool WILL_TAKE_QUEEN = true;
const bool PROTECT_QUEEN = false;
const bool PROTECT_KING = false;
#define DISABLE_PRINT //increases performence as array is no longer copied

enum Player { play_random=0, play_simple=1, play_monte_carlo=2 };//Player

struct WhiteMove {
    int score;
    int x, y;        // position of white king
    WhiteMove(int _x, int _y) {
        x = _x;
        y = _y;
        score = 0;
    }
};

class Board {
  public:
    #ifndef DISABLE_PRINT
    void humanwhitemove ( );
    char A[MAX][MAX];    // game board (pretty useless) top left is (1,1) = (row,column)
    void print ( );
    #endif
    int thesize;         // height = width of game board
    int xBK, yBK;        // position of black king
    int xWK, yWK;        // position of white king
    int xWQ, yWQ;        // position of white queen (or rook)
    bool queencaptured;  // is white queen captured?
    bool whoistomove;    // who is to move? (true: white)
    int countmoves;      // number of moves so far
    bool queenorrook;    // true: white queen; false: white rook
    Player white_player; // random, simple or monte_carlo
    // member functions, see below for comments
    Board ( );
    Board (int somesize, bool qor);
    bool legalforblackking (int i, int j);
    bool legalforwhiteking (int i, int j);
    bool canwhitequeenreach (int i, int j);
    bool canwhiterookreach (int i, int j);
    bool legalforwhitequeen (int i, int j);
    bool incheck (int i, int j);
    bool checkmate ( );
    int numberofblackmoves ( );
    int numberofwhitequeenmoves ( );
    int numberofwhitekingmoves ( );
    int randomblackmove ( );

    int score_white_king_move ();
    int score_white_queen_move ();

    void randomwhitemove ( );
    void simple_whitemove ( );
    void monte_carlo_whitemove (int max_monte_carlo_depth);

    int simulate_game(int max_monte_carlo_depth);

};//Board

// get first non-enter
char getfirstchar ( ) {
  char c = cin.get ( );
  while ( c == '\n' )
    c = cin.get ( );
  return c;
}//getfirstchar

// default constructor
Board::Board ( ) {

}//Board::Board

// constructor
Board::Board (int somesize, bool qor) {
  thesize = somesize;
  queenorrook = qor;
#ifndef DISABLE_PRINT
  int i, j;
  for ( i = 0; i < MAX; i++ )
    for ( j = 0; j < MAX; j++ )
      if ( 0 < i && i <= thesize && 0 < j && j <= thesize )
        A[i][j] = '.';
      else
        A[i][j] = 'O'; // outside board
#endif

  xWK = rand ( ) % thesize + 1;
  yWK = rand ( ) % thesize + 1;
  do {
    xBK = rand ( ) % thesize + 1;
    yBK = rand ( ) % thesize + 1;
  } while ( xBK - xWK <= 1 && xWK - xBK <= 1 
            && yBK - yWK <= 1 && yWK - yBK <= 1 );
  do {
    xWQ = rand ( ) % thesize + 1;
    yWQ = rand ( ) % thesize + 1;
  } while ( ( xWQ == xWK && yWQ == yWK ) || ( xWQ == xWK && yWQ == yWK )
            || incheck (xBK,yBK) || numberofblackmoves ( ) == 0 );
  queencaptured = false;
  whoistomove = true;
  countmoves = 0;
}//Board::Board

// can BK move from (xBK,yBK) to (i,j)?
bool Board::legalforblackking (int i, int j) {
  if ( i <= 0 || j <= 0 || i > thesize || j > thesize )
    return false;
  if ( i == xBK && j == yBK )
    return false;
  if ( i > xBK+1 || i < xBK-1 || j > yBK+1 || j < yBK-1 )
    return false;
  if ( i - xWK <= 1 && xWK - i <= 1 && j - yWK <= 1 && yWK - j <= 1 )
    return false;
  if ( i == xWQ && j == yWQ )
    return true;  // king captures queen
  if ( incheck (i,j) )
    return false;
  return true;
}//legalforblackking

// can WK move from (xWK,yWK) to (i,j)?
bool Board::legalforwhiteking (int i, int j) {
  if ( i <= 0 || j <= 0 || i > thesize || j > thesize )
    return false;
  if ( i == xWK && j == yWK )
    return false;
  if ( i > xWK+1 || i < xWK-1 || j > yWK+1 || j < yWK-1 )
    return false;
  if ( i - xBK <= 1 && xBK - i <= 1 && j - yBK <= 1 && yBK - j <= 1 )
    return false;
  if ( i == xWQ && j == yWQ )
    return false;
  return true;
}//legalforwhiteking

// can WQ reach (i,j) from (xWQ,yWQ)?
bool Board::canwhitequeenreach (int i, int j) {
  if ( i <= 0 || j <= 0 || i > thesize || j > thesize )
    return false;
  if ( i != xWQ && j != yWQ && i + j != xWQ + yWQ && i - j != xWQ - yWQ )
    return false;
  if ( i == xWQ && i == xWK 
       && ( ( yWQ < yWK && yWK < j ) || ( j < yWK && yWK < yWQ ) ) )
    return false;
  if ( j == yWQ && j == yWK 
       && ( ( xWQ < xWK && xWK < i ) || ( i < xWK && xWK < xWQ ) ) )
    return false;
  if ( i + j == xWQ + yWQ && i + j == xWK + yWK 
       && ( ( xWQ - yWQ < xWK - yWK && xWK - yWK < i - j ) 
            || ( i - j < xWK - yWK && xWK - yWK < xWQ - yWQ ) ) )
    return false;
  if ( i - j == xWQ - yWQ && i - j == xWK - yWK 
       && ( ( xWQ + yWQ < xWK + yWK && xWK + yWK < i + j ) 
            || ( i + j < xWK + yWK && xWK + yWK < xWQ + yWQ ) ) )
    return false;
  return true;
}//canwhitequeenreach

// can WQ reach (i,j) from (xWQ,yWQ), when being a ROOK?
bool Board::canwhiterookreach (int i, int j) {
  if ( i <= 0 || j <= 0 || i > thesize || j > thesize )
    return false;
  if ( i != xWQ && j != yWQ )
    return false;
  if ( i == xWQ && i == xWK 
       && ( ( yWQ < yWK && yWK < j ) || ( j < yWK && yWK < yWQ ) ) )
    return false;
  if ( j == yWQ && j == yWK 
       && ( ( xWQ < xWK && xWK < i ) || ( i < xWK && xWK < xWQ ) ) )
    return false;
  return true;
}//canwhiterookreach

// is BK at (i,j) in check? TODO is this what this function does?
bool Board::incheck (int i, int j) {
  if ( queenorrook )
    return canwhitequeenreach (i,j);
  else
    return canwhiterookreach (i,j);
}//Board::incheck

// can WQ move from (xWQ,yWQ) to (i,j)?
bool Board::legalforwhitequeen (int i, int j) {
  if ( i <= 0 || j <= 0 || i > thesize || j > thesize )
    return false;
  if ( i == xWQ && j == yWQ )
    return false;
  if ( i == xWK && j == yWK )
    return false;
  return incheck (i,j);
}//legalforwhitequeen

// black cannot move and is in check
bool Board::checkmate ( ) {
  return ( numberofblackmoves ( ) == 0 && incheck (xBK,yBK) );
}//checkmate

// how many moves are available for BK?
int Board::numberofblackmoves ( ) {
  int howmany = 0;
  int i, j;
  for ( i = -1; i <= 1; i++ )
    for ( j = -1; j <= 1; j++ )
      howmany += legalforblackking (xBK+i,yBK+j);
  return howmany;
}//Board::numberofblackmoves

// how many moves are available for WK?
int Board::numberofwhitekingmoves ( ) {
  int howmany = 0;
  int i, j;
  for ( i = -1; i <= 1; i++ )
    for ( j = -1; j <= 1; j++ )
      howmany += legalforwhiteking (xWK+i,yWK+j);
  return howmany;
}//Board::numberofwhitekingmoves

// how many moves are available for WQ?
int Board::numberofwhitequeenmoves ( ) {
  int howmany = 0;
  int i, j;
  for ( i = 1; i <= thesize; i++ )
    for ( j = 1; j <= thesize; j++ )
      howmany += legalforwhitequeen (i,j);
  return howmany;
}//Board::numberofwhitequeenmoves

// do a random move for White
void Board::randomwhitemove ( ) {
  int move, i, j;
  int numberK = numberofwhitekingmoves ( );
  int numberQ = numberofwhitequeenmoves ( );
  move = rand ( ) % ( numberK + numberQ );
  if ( move < numberK )
    for ( i = -1; i <= 1; i++ )
      for ( j = -1; j <= 1; j++ )
        if ( legalforwhiteking (xWK+i,yWK+j) ) {
          if ( move == 0 ) {
            xWK = xWK+i; yWK = yWK+j;
            whoistomove = ! whoistomove;
            countmoves++;
            return;
          }//if
          move--;
        }//if
  move -= numberK;
  for ( i = 1; i <= thesize; i++ )
    for ( j = 1; j <= thesize; j++ )
        if ( legalforwhitequeen (i,j) ) {
          if ( move == 0 ) {
            xWQ = i; yWQ = j;
            whoistomove = ! whoistomove;
            countmoves++;
            return;
          }//if
          move--;
        }//if
}//Board::randomwhitemove

// do a random move for BK
// return 0 if checkmate, 1 if stalemate, 2 if queen captured,
// 3 otherwise
int Board::randomblackmove ( ) {
  int move, i, j;
  int number = numberofblackmoves ( );
  if ( number == 0 ) {
    if ( incheck (xBK,yBK) )
      return 0;
    else
      return 1;
  }//if
// optional: if BK can take WQ, do it ...
  if (WILL_TAKE_QUEEN) {
    for ( i = -1; i <= 1; i++ )
     for ( j = -1; j <= 1; j++ )
       if ( legalforblackking (xBK+i,yBK+j) ) {
         if ( xBK+i == xWQ && yBK+j == yWQ ) {
           xBK = xBK+i; yBK = yBK+j;
             queencaptured = true;
             return 2;
           }//if
       }//if
    //cout<<"Queen taken"<<endl;
  }
  move = rand ( ) % number;
  for ( i = -1; i <= 1; i++ )
    for ( j = -1; j <= 1; j++ )
      if ( legalforblackking (xBK+i,yBK+j) ) {
        if ( move == 0 ) {
          xBK = xBK+i; yBK = yBK+j;
          if ( xBK == xWQ && yBK == yWQ ) {
            queencaptured = true;
            return 2;
          }//if
          whoistomove = ! whoistomove;
          return 3;
        }//if
        move--;
      }//if
  return 999;
}//Board::randomblackmove

#ifndef DISABLE_PRINT
// do a move for White: human player
void Board::humanwhitemove ( ) {
  char choice;
  bool OK = false;
  int i, j;
  do {
    print ( );
    cout << "A (white) move please (Q or K) .. ";
    choice = getfirstchar ( );
    if ( choice == 'q' || choice == 'Q' ) {
      cout << "Row number for queen (1..)     .. ";
      i = getfirstchar ( ) - '0';
      cout << "Column number for queen (1..)  .. ";
      j = getfirstchar ( ) - '0';
      if ( legalforwhitequeen (i,j) ) {
        xWQ = i; yWQ = j;
        OK = true;
      }//if
      else {
        cout << "Illegal queen move ..." << endl;
      }//else
    }//if
    else {
      cout << "Row number for king (1..)      .. ";
      i = getfirstchar ( ) - '0';
      cout << "Column number for king (1..)   .. ";
      j = getfirstchar ( ) - '0';
      if ( legalforwhiteking (i,j) ) {
        xWK = i; yWK = j;
        OK = true;
      }//if
      else {
        cout << "Illegal king move ..." << endl;
      }//else
    }//if
  } while ( ! OK );
  whoistomove = ! whoistomove;
  countmoves++;
}//Board::humanwhitemove

// print board
void Board::print ( ) {
  int i, j;
  A[xBK][yBK] = 'z';
  A[xWK][yWK] = 'K';
  if ( ! queencaptured )
    A[xWQ][yWQ] = 'Q';
  cout << "  ";
  for ( j = 1; j <= thesize; j++ )
    cout << " " << j % 10;
  cout << endl;
  for ( i = 1; i <= thesize; i++ ) {
    cout << ( i < 10 ? " " : "" ) << i << " ";
    for ( j = 1; j <= thesize; j++ )
      cout << A[i][j] << " ";
    cout << endl;
  }//for
  A[xBK][yBK] = '.';
  A[xWK][yWK] = '.';
  A[xWQ][yWQ] = '.';
}//Board::print
#endif

bool neighboars (int xB, int yB, int xW, int yW) {
  return (abs(xB - xW) <= 1 && abs(yB - yW) <= 1);
}//neighboars

int distance_from_line_of_sight (int xB, int yB, int xW, int yW) {
  int dx = xB-xW;
  int dy = yB-yW;
  //the closer the difference between dx and dy comes to 0 the closer we
  //are to line of sight between the pieces
  
  //score diagonal, closer to diagonal = lower score
  int diag_score = abs(abs(dx)-abs(dy));
  //score straight, closer to straight line = lower score
  int vert_horizontal_score = abs(min(dx,dy));
  
  return min(diag_score, vert_horizontal_score);
}

int distance(int xB, int yB, int xW, int yW) {
  int dx = xB-xW;
  int dy = yB-yW;
  
  return sqrt(dx*dx+dy*dy);
}

int Board::score_white_king_move () {
  int score = 3;

  //taking the black piece is impossible, dont try
  if (xWK == xBK && yWK == yBK)
    return 0;

  // possibly having the king taken is really bad
  if (PROTECT_KING) {
    if (neighboars(xBK,yBK, xWK, yWK)){
      //cout << "king could be taken" << endl;
      return 0;
    }
  }

  //minimise number of black moves
  score += 20*1/float(1+numberofblackmoves() );

  //being close is good
  score += 10*1/float(1+distance(xBK,yBK, xWK, yWK)); //how close is the queen
  score += 10*1/float(1+distance(xBK,yBK, xWQ, yWQ)); //how close is the queen

  score += 15*incheck (xBK,yBK);

  score += 2*numberofwhitequeenmoves ( );
  score += 2*numberofwhitekingmoves ( );

  //having close to line of sight is great
  //score += 10*1/float(1+distance_from_line_of_sight(xBK,yBK, xWK, yWK));
  //score += 10*1/float(1+distance_from_line_of_sight(xBK,yBK, xWQ, yWQ));

  return score;
}


int Board::score_white_queen_move () {
  int score = 3;

  //taking the black piece is impossible, dont try
  if (xWQ == xBK && yWQ == yBK)
    return 0;

  if (PROTECT_QUEEN) {
    if (neighboars(xBK,yBK, xWQ, yWQ)){ //queen can be taken
      if (not neighboars(xWK, yWK, xWQ, yWQ)){ //queen not coverd
        return 0; //will lose queen, bad!
      }
    }
  }
  
  if (WILL_TAKE_QUEEN) {
    if (neighboars(xBK,yBK, xWQ, yWQ)){ //queen can be taken next turn
      return 1;
    }
  }

  //having close to line of sight is great
  score += 20*1/float(1+numberofblackmoves() );
  
  //being close is good
  score += 10*1/float(1+distance(xBK,yBK, xWQ, yWQ)); //how close is the queen
  score += 10*1/float(1+distance(xBK,yBK, xWK, yWK)); //how close is the king

  score += 15*incheck (xBK,yBK);

  score += 2*numberofwhitequeenmoves ( );
  score += 2*numberofwhitekingmoves ( );

  //having close to line of sight is great
  //score += 10*1/float(1+distance_from_line_of_sight(xBK,yBK, xWK, yWK));
  //score += 10*1/float(1+distance_from_line_of_sight(xBK,yBK, xWQ, yWQ));
  
  return score;
}


// do a simple move for White
void Board::simple_whitemove ( ) {
  //cout << "move" << endl;
  countmoves++;
  whoistomove = ! whoistomove;
  
  WhiteMove king_move(xWK, yWK);
  WhiteMove queen_move(xWQ, yWQ);

  Board copied_board = *this; // assignment shallow copies
  //try all legal moves for the king
  for ( int i = -1; i <= 1; i++ ) {
    for ( int j = -1; j <= 1; j++ ) {
      if ( legalforwhiteking (xWK+i,yWK+j) ) {
        copied_board.xWK = xWK+i; copied_board.yWK = yWK+j;
        int score = copied_board.score_white_king_move();

        if (score > king_move.score) {
          king_move.score = score;
          king_move.x = xWK+i; king_move.y = yWK+j;
        }
      }
    }
  }
  //reset king position
  copied_board.xWK = xWK; copied_board.yWK = yWK;
  for ( int k = 1; k <= thesize; k++ ) {
    for ( int l = 1; l <= thesize; l++ ) {
      if ( legalforwhitequeen (k,l) ) {
        copied_board.xWQ = k; copied_board.yWQ = l;
        int score = copied_board.score_white_queen_move();

        if (score > queen_move.score) {
          queen_move.score = score;
          queen_move.x = k; queen_move.y = l;
        }
      }
    }
  }
  //apply the best move to the current board
  if (king_move.score >= queen_move.score) {
    xWK = king_move.x; yWK = king_move.y;
  } else {
    xWQ = queen_move.x; yWQ = queen_move.y;
  }
}//Board::simple_whitemove

int Board::simulate_game(int max_monte_carlo_depth) {
    int themove = randomblackmove ( ); //0 if checkmate, 1 if stalemate, 2 if simple tie,
    while ( themove == 3 && countmoves < max_monte_carlo_depth ) {
        randomwhitemove ( );
        themove = randomblackmove ( );
    }//while
    return themove;
}

// do a random move for White
void Board::monte_carlo_whitemove(int max_monte_carlo_depth) {
  countmoves++;
  whoistomove = ! whoistomove;

  WhiteMove king_move(xWK, yWK);
  WhiteMove queen_move(xWQ, yWQ);

  //try all legal moves for the king
  for ( int i = -1; i <= 1; i++ ) {
    for ( int j = -1; j <= 1; j++ ) {
      if ( legalforwhiteking (xWK+i,yWK+j) ) {
        int score = 0;
        for ( int x = 0; x <= 1000; x++){
          Board copied_board = *this; copied_board.countmoves = 0;
          copied_board.xWK = xWK+i; copied_board.yWK = yWK+j;
          int themove = copied_board.simulate_game(max_monte_carlo_depth);
          score += max_monte_carlo_depth- copied_board.countmoves;
          if (themove == 0) score += 1000;
        }
        if (score > king_move.score) {
          king_move.score = score;
          king_move.x = xWK+i; king_move.y = yWK+j;
        }
      }
    }
  }
  //try all queen moves
  for ( int k = 1; k <= thesize; k++ ) {
    for ( int l = 1; l <= thesize; l++ ) {
      if ( legalforwhitequeen (k,l) ) {
        //check if queen wont be next to the king
        if (!neighboars(xBK,yBK, k, l)){
          int score = 0;
          for ( int x = 0; x <= 1000; x++){
            Board copied_board = *this; copied_board.countmoves = 0;
            copied_board.xWQ = k; copied_board.yWQ = l;
            int themove = copied_board.simulate_game(max_monte_carlo_depth);
            score += max_monte_carlo_depth- copied_board.countmoves;
            if (themove == 0) score += 1000;
          }
          if (score > queen_move.score) {
            queen_move.score = score;
            queen_move.x = k; queen_move.y = l;
          }
        }
      }
    }
  }
  //apply the best move to the current board
  cout << king_move.score << endl;
  if (king_move.score >= queen_move.score) {
    xWK = king_move.x; yWK = king_move.y;
  } else {
    xWQ = queen_move.x; yWQ = queen_move.y;
  }

}//Board::randomwhitemove

// play one game
// return 0 if checkmate, 1 if stalemate, 2 if simple tie,
// 3 if stopped
int playagame (int somesize, int maxgamelength, Player white_player, bool queenorrook) {
  Board board (somesize,queenorrook);
  int themove = 3;
  switch(white_player) {
    case play_random:
      while ( themove == 3 && board.countmoves < maxgamelength ) {
        board.randomwhitemove ( );
        themove = board.randomblackmove ( ); 
      }//while
      break;
    case play_simple:
      while ( themove == 3 && board.countmoves < maxgamelength ) {
        board.simple_whitemove ( );
        themove = board.randomblackmove ( ); 
      }//while
      break;
    case play_monte_carlo:
      while ( themove == 3 && board.countmoves < maxgamelength ) {
        board.monte_carlo_whitemove (200);
        themove = board.randomblackmove ( ); 
      }//while
      break;
  }

  return themove;
}//playagame

int main (int argc, char* argv[ ]) {
  int i;
  int stats[4];
  int somesize;
  int simulations;
  int maxgamelength;
  Player white_player;
  bool queenorrook;
  if ( argc >= 5 ) {
    somesize = atoi (argv[1]);
    if ( somesize > MAX - 2 )
      somesize = MAX - 2;
    if ( somesize <= 3 )
      somesize = 4;
    simulations = atoi (argv[2]);
    maxgamelength = atoi (argv[3]);
    white_player = static_cast<Player>(atoi (argv[4] ));
    if (white_player > 2)
      white_player = Player::play_random;
    queenorrook = ( atoi (argv[5]) == 0 );
  }//if
  else {
    cout << "Rather use " << argv[0]
         << " <thesize> <simulations> <maxgamelength> [0|1|2] [0|1]" << endl;
    somesize = 8;
    simulations = 1000;
    maxgamelength = 200;
    white_player = Player::play_random;
    queenorrook = true;
  }//else
  srand (time(NULL));  // seed random generator
  stats[0] = stats[1] = stats[2] = stats[3] = 0;
  for ( i = 0; i < simulations; i++ )
     stats[playagame (somesize,maxgamelength,white_player,queenorrook)]++;
  cout << "Board size:      " << somesize << endl
       << "Max game length: " << maxgamelength << endl
       << "Wins:            " << stats[0] << endl
       << "Stalemates:      " << stats[1] << endl
       << "Simple ties:     " << stats[2] << endl
       << "Stopped:         " << stats[3] << endl
       << "Total:           " << simulations << endl;
  return 0;
}//main

