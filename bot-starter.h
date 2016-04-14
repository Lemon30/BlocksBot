// Christos Savvopoulos <savvopoulos@gmail.com>
// Elias Sprengel <blockbattle@webagent.eu>

#ifndef __BOT_STARTER_H
#define __BOT_STARTER_H

#include <cstdlib>
#include <vector>
#include <math.h>

#include "bot-state.h"
#include "move.h"

using namespace std;

/**
 * This class is where the main logic should be. Implement getMoves() to
 * return something better than random moves.
 */
class BotStarter {
 public:
  /**
   * Returns a random amount of random moves
   * @param state : current state of the bot
   * @param timeout : time to respond
   * @return : a list of moves to execute
   */
  vector<Move::MoveType> GetMoves(const BotState& state,
                                  long long timeout) const {
    vector<Move::MoveType> moves;

    Field field = state.MyField(); // Sahan?n durumunu ö?ren
    Shape newShape(state.CurrentShape(), field, state.ShapeLocation().first, state.ShapeLocation().second); //Gelen ?ekli yarat

    /*
    //Field checker copy pasta
    for (int i = 0; i < field.height(); i++) {
        for (int j = 0; j < field.width(); j++) {
            cerr << "|" << field.GetCell(j, i).AsString();
        }
        cerr << "|" << endl;
    }*/

    if (newShape.IsOk())
        cerr << "MOVING LEFT " << endl;

    Shape testShapeLeft(state.CurrentShape(), field, newShape.x(), newShape.y()); //?eklin kopyas?n? yarat
    
    int testLeftMoves = checkMove(&testShapeLeft, 'l'); //Kaç kere sola gidebilir?
    cerr << "Block can move left " << testLeftMoves << " times." << endl;

    for (int i = 0; i < testLeftMoves; i++) {
        newShape.OneLeft();
        moves.push_back(Move::MoveType::LEFT);
    }

    tita t = findBestMove(field, &newShape, state.CurrentShape());
    cerr << "Block should move right " << t.right << " times for the best move." << endl;

    while (t.right > 0) {
        moves.push_back(Move::MoveType::RIGHT);
        t.right--;
    }

    while (t.rotation > 0) {
        moves.push_back(Move::MoveType::TURNRIGHT);
        t.rotation--;
    }

    moves.push_back(Move::MoveType::DROP);
    return moves;
  }

  struct tita {
      int rotation;
      int right;
  };

  tita findBestMove(Field field, Shape *shape, Shape::ShapeType shapetype) const {      
      int totalRotations = 0;
      int rotations = 0;
      int bestscore = -999999;
      int totalRights = 0;

      while (rotations < 4) {
          
          cerr << "Testing block for " << rotations << "th rotation." << endl;
          Shape testShapeRight(shapetype, field, shape->x(), shape->y());
          int testRightMoves = checkMove(&testShapeRight, 'r');
          for (int r = 0; r < rotations; r++)
              testShapeRight.TurnRight();
          cerr << "Block can move right a total of " << testRightMoves << " times." << endl;
          while (testRightMoves >= 0) {
              cerr << "Block will move right " << testRightMoves << " times to test." << endl;
              Field newField(field.width(), field.height(), field.copyField());
              Shape ghostShape(shapetype, field, shape->x(), shape->y());
              for (int r = 0; r < rotations; r++)
                  ghostShape.TurnRight();
              for (int i = 0; i < testRightMoves; i++) {
                  ghostShape.OneRight();
              }
              Shape testShapeDown(shapetype, field, ghostShape.x(), ghostShape.y());
              for (int r = 0; r < rotations; r++)
                  testShapeDown.TurnRight();
              int testDownMoves = checkMove(&testShapeDown, 'd');
              cerr << "Tested down. This piece has to move down " << testDownMoves << " times." << endl;
              for (int i = 0; i < testDownMoves; i++) {
                  ghostShape.OneDown();
              }
              for (const Cell* cell : ghostShape.GetBlocks()) {
                  const Cell& c = *cell;
                  newField.SetCell(c.x(), c.y());
              }
              int score = evaluate(&newField);
              if (score > bestscore) {
                  bestscore = score;
                  totalRights = testRightMoves;
                  totalRotations = rotations;
              }
              testRightMoves--;
          }
          rotations++;
      }
      tita t;
      t.right = totalRights;
      t.rotation = totalRotations;
      return t;
  }

  int evaluate(Field *field) const {
      int score = -9999;
      
      int aggregateHeight = 0;
      for (int i = 0; i < field->width(); i++) {
          for (int j = 0; j < field->height(); j++) {
              if (field->GetCell(i, j).state() == Cell::CellState::BLOCK || field->GetCell(i, j).state() == Cell::CellState::SOLID) {
                  //cerr << "Agg height starts at " << field->height()-j << " for the " << i << "th column." << endl;
                  aggregateHeight = aggregateHeight + field->height() - j;
                  j = field->height();
              }
          }
      }
      cerr << endl;
      int completedLines = 0;
      for (int i = 0; i < field->height(); i++) {
          int checkFullRow = 0;
          for (int j = 0; j < field->width(); j++) {
              if (field->GetCell(j, i).state() != Cell::CellState::EMPTY) {
                  checkFullRow++;
              }
          }
          if (checkFullRow == 8)
              completedLines++;
      }

      int holes = 0;
      for (int i = 0; i < field->width(); i++) {
          bool startChecking = false;
          for (int j = 0; j < field->height(); j++) {
              if (field->GetCell(i, j).state() == Cell::CellState::BLOCK) {
                  if (!startChecking)
                      startChecking = true;
              }
              if (field->GetCell(i, j).state() == Cell::CellState::EMPTY)
                  holes++;
          }
      }

      int bumpiness = 0;
      int heights[10] = {0};
      for (int i = 0; i < field->width(); i++) {
          for (int j = 0; j < field->height(); j++) {
              if (field->GetCell(i, j).state() == Cell::CellState::BLOCK || field->GetCell(i, j).state() == Cell::CellState::SOLID) {
                  heights[i] = field->height() - j;
                  j = field->height();
              }
          }
      }
      
      for (int i = 0; i < field->width() - 1; i++) {
          bumpiness = bumpiness + abs(heights[i] - heights[i + 1]);
      }

      cerr << "Agg: " << aggregateHeight << ". Comp: " << completedLines << ". Hole: " << holes << ". Bump: " << bumpiness << endl;
      score = (-50) * aggregateHeight + (76) * completedLines + (-35) * holes + (-18) * bumpiness;
      return score;
  }

  int checkMove(Shape *shape, char side) const {
      int move = 0;
      bool checking = true;
      while ( checking ) {
          move++;
          switch (side) {
          case 'd':
              shape->OneDown();
              break;
          case 'l':
              shape->OneLeft();
              break;
          case 'r':
              shape->OneRight();
              break;
          default:
              cerr << "CHECKING AN INVALID SIDE. ABORT!" << endl;
          }
          checking = shape->IsOk();
      }
      move--;
      return move;
  }

};

#endif  //__BOT_STARTER_H
