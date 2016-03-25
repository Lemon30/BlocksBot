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
    int nrOfMoves = rand() % 41 + 1;

    for (int n = 0; n < nrOfMoves; ++n) {
      Move::MoveType randomMove =
          static_cast<Move::MoveType>(rand() % Move::MoveType::LAST);
      if (randomMove != Move::MoveType::DROP) {
        moves.push_back(randomMove);
      }
    }
    moves.push_back(Move::MoveType::DROP);
    return moves;
  }
};

#endif  //__BOT_STARTER_H
