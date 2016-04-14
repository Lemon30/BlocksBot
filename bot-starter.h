// Christos Savvopoulos <savvopoulos@gmail.com>
// Elias Sprengel <blockbattle@webagent.eu>

#ifndef __BOT_STARTER_H
#define __BOT_STARTER_H

#include <cstdlib>
#include <vector>

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

    Shape::ShapeType blockType = state.CurrentShape();
    Field field = state.MyField();
    Shape newShape(blockType, field, state.ShapeLocation().first, state.ShapeLocation().second);
    
    newShape.OneDown();
    newShape.OneDown();
    moves.push_back(Move::MoveType::DOWN);
    moves.push_back(Move::MoveType::DOWN);

    int left = 0;
    Shape testShapeLeft(blockType, field, newShape.x(), newShape.y());
    while (testShapeLeft.IsOk()) {
        left++;
        testShapeLeft.OneLeft();
    }
    left--;
    
    while (left > 0) {
        left--;
        moves.push_back(Move::MoveType::LEFT);
    }


    int bestrights = findBestMove(field, &newShape);

    while (bestrights > 0) {
        moves.push_back(Move::MoveType::RIGHT);
        bestrights--;
    }

    moves.push_back(Move::MoveType::DROP);
    return moves;
  }

  int findBestMove(Field field, Shape *shape) const {
      int rotations = 0;
      int bestscore = -999999;
      int totalRights = 0;
      int oldRight;

      while (rotations < 1) {
          int right = 0;
          Shape testShapeRight(shape->type(), field, shape->x(), shape->y());
          while (testShapeRight.IsOk()) {
              right++;
              testShapeRight.OneRight();
          }
          right--;
          oldRight = right;
          while (right > 0) {
              Field newField(field.width(), field.height(), field.copyField());
              Shape ghostShape(shape->type(), field, shape->x(), shape->y());
              Shape testShapeDown(shape->type(), field, shape->x(), shape->y());
              cerr << "Moving right " << oldRight - right << " times." << endl;
              for (int i = 0; i < oldRight - right; i++) {
                  ghostShape.OneRight();
                  testShapeDown.OneRight();
              }
              int down = 0;
              while (testShapeDown.IsOk()) {
                  down++;
                  testShapeDown.OneDown();
              }
              down--;
              cerr << "Tested down. This piece has to move down " << down << " times." << endl;
                  while (down > 0) {
                  ghostShape.OneDown();
                  down--;
              }
              for (const Cell* cell : ghostShape.GetBlocks()) {
                  const Cell& c = *cell;
                  newField.SetCell(c.x(), c.y());
              }
              int score = evaluate(&newField);
              cerr << "Evaluating option with score: " << score << " as " << oldRight-right << "th test. " << endl;
              if (score > bestscore) {
                  bestscore = score;
                  totalRights = oldRight - right;
              }
              right--;
          }
          rotations++;
      }
      cerr << "Selected option with score: " << bestscore << " after " << oldRight << " tests. " << endl;
      return totalRights;
  }

  int evaluate(Field *field) const {
      int score = -9999;
      
      int aggregateHeight = 0;
      for (int i = 0; i < field->width(); i++) {
          for (int j = field->height()-1; j > 0; j--) {
              cerr << " Checking cell x: " << i << " y: " << j;
              if (field->GetCell(i, j).state() == Cell::CellState::BLOCK || field->GetCell(i, j).state() == Cell::CellState::SOLID) {
                  aggregateHeight = aggregateHeight + j;
                  cerr << "Agg is now: " << aggregateHeight << endl;
                  j = 0;
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
          for (int j = field->height(); j > 0; j--) {
              if (field->GetCell(i, j).state() != Cell::CellState::EMPTY) {
                  if (!startChecking)
                      startChecking = true;
              }
              if (field->GetCell(i, j).state() == Cell::CellState::EMPTY)
                  holes++;
          }
      }

      int bumpiness = 0;
      int c1 = -1;
      int c2 = -1;
      for (int i = 0; i < field->width(); i++) {
          for (int j = field->height(); j > 0; j--) {
              if (field->GetCell(i, j).state() != Cell::CellState::EMPTY) {
                  if (c1 < 0 && c2 < 0)
                      c1 = j;
                  else if (c1 >= 0 && c2 < 0) {
                      c2 = j;
                      bumpiness = bumpiness + (c2 - c1);
                      c1 = j;                        
                  }
                  else if (c1 >= 0 && c2 >= 0) {
                      c2 = j;
                      bumpiness = bumpiness + (c2 - c1);
                      c1 = j;
                  }
              }
          }
      }

      cerr << "Agg: " << aggregateHeight << ". Comp: " << completedLines << ". Hole: " << holes << ". Bump: " << bumpiness << endl;
      score = (-50) * aggregateHeight + (76) * completedLines + (-35) * holes +(-18) * bumpiness;
      return score;
  }

};

#endif  //__BOT_STARTER_H
