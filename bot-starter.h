// Christos Savvopoulos <savvopoulos@gmail.com>
// Elias Sprengel <blockbattle@webagent.eu>

#ifndef __BOT_STARTER_H
#define __BOT_STARTER_H

#include <cstdlib>
#include <vector>

#include "bot-state.h"
#include "move.h"
#include "prediction.h"

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
             int bestscore = -1000;
             Shape newShape(state.CurrentShape(), state.MyField(), 
                 state.ShapeLocation().first, state.ShapeLocation().second);
             
                 //For every rotation
             for (int rotation = 0; rotation < 4; rotation++) {
                 int left = 0;
                 //Don't rotate for the first time around
                 if (rotation != 0) {
                     newShape.TurnRight();
                 }

                 Shape ghostShape = newShape.ghost();
                 Field newField = state.MyField();
                 bool checked = newField.checkOneLeft(ghostShape);
                 while (checked) {
                     ghostShape.OneLeft();
                     left++;
                 }

             }
             moves.push_back(Move::MoveType::DROP);
             return moves;
     }
     
     

     /*vector<Move::MoveType> GetMoves(const BotState& state,
         long long timeout) const {
         vector<Move::MoveType> moves;
         if (state.CurrentShape() == Shape::ShapeType::I) {
         //if (state.MyField().GetAggregateHeight() <= 4) {
             moves.push_back(Move::MoveType::LEFT);
             moves.push_back(Move::MoveType::LEFT);
             moves.push_back(Move::MoveType::LEFT);
         }
         else if (state.CurrentShape() == Shape::ShapeType::S || state.CurrentShape() == Shape::ShapeType::Z) {

         } else {
             moves.push_back(Move::MoveType::RIGHT);
             moves.push_back(Move::MoveType::RIGHT);
             moves.push_back(Move::MoveType::RIGHT);
         }

         moves.push_back(Move::MoveType::DROP);
         return moves;
     }*/
};

#endif  //__BOT_STARTER_H

/*int nrOfMoves = rand() % 41 + 1;

for (int n = 0; n < nrOfMoves; ++n) {
Move::MoveType randomMove =
static_cast<Move::MoveType>(rand() % Move::MoveType::LAST);
if (randomMove != Move::MoveType::DROP) {
moves.push_back(randomMove);
}
}
moves.push_back(Move::MoveType::DROP); */
