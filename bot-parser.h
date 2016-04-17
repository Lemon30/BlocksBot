// Christos Savvopoulos <savvopoulos@gmail.com>
// Elias Sprengel <blockbattle@webagent.eu>

#ifndef __BOT_PARSER_H
#define __BOT_PARSER_H

#include <iostream>
#include <string>
#include <vector>

#include "move.h"
#include "bot-starter.h"

using namespace std;

/**
 * Main class that will keep reading output from the engine.
 * Will either update the bot state or get actions.
 */
class BotParser {
 public:
  BotParser(const BotStarter& bot) : bot_(bot) {}

  void Run() {
    BotState currentState;
    bool predict = false;
    int round = 0;

    while (true) {
        cerr << "Round: " << round << endl;
        round++;
      string command;
      cin >> command;
      if (command == "settings") {
        string part1, part2;
        cin >> part1 >> part2;
        currentState.UpdateSettings(part1, part2);
      } else if (command == "update") {
        string part1, part2, part3;
        cin >> part1 >> part2 >> part3;
        currentState.UpdateState(part1, part2, part3);
      } else if (command == "action") {
        string output, moveJoin, part1;
        long long part2;
        cin >> part1 >> part2;

        predict = !predict;
        
        //Get solid height
        int solidHeight = 0;
        for (int j = 0; j < currentState.MyField().height(); j++) {
            if (currentState.MyField().GetCell(0, j).state() == Cell::CellState::SOLID) {
                solidHeight = currentState.MyField().height() - j;
                break;
            }
        }

        int tCheckArrUp[3] = { 0 };
        int tCheck = 0;
        int column1 = 0;
        int column2 = 0;
        bool done = false;

        for (int j = 0; j < currentState.MyField().height(); j++) {
            if (done)
                break;
            for (int i = 0; i < currentState.MyField().width(); i++) {
                if (done)
                    break;
                if (j == currentState.MyField().height() - solidHeight - 1) {
                    if (i >= 2) {
                        if (currentState.MyField().GetCell(i, j).state() == Cell::CellState::EMPTY) {
                            tCheckArrUp[tCheck] = i;
                            tCheck++;
                            if ((tCheck == 1 || tCheck == 3) && (currentState.MyField().GetCell(i, j+1).state() == Cell::CellState::BLOCK)) {
                                if (tCheck == 3) {
                                    column1 = tCheckArrUp[1];
                                    column2 = tCheckArrUp[2];
                                    done = true;
                                    cerr << "Chance for a t-spin at columns: " << column1 << column2 << endl;
                                }
                            }
                            else if((tCheck == 2) && (currentState.MyField().GetCell(i, j+1).state() == Cell::CellState::EMPTY)) {

                            }
                            else {
                                tCheck = 0;
                            }
                        }
                        else {
                            tCheck = 0;
                        }
                    }

                }
            }
        }

        vector<Move::MoveType> moves = bot_.GetMoves(currentState, part2, predict);

        if (moves.size() > 0) {
          for (Move::MoveType move : moves) {
            output += moveJoin;
            output += Move::MoveToString(move);
            moveJoin = ",";
          }
        } else {
          output += "no_moves";
        }

        cout << output << endl;
      } else if (command.size() == 0) {
        // no more commands, exit.
        break;
      } else {
        cerr << "Unable to parse command: " << command << endl;
      }
    }
  }

 private:
  const BotStarter& bot_;
};

#endif  //__BOT_PARSER_H
