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
                                  long long timeout, bool predict) const {
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
    cerr << "---------------------------------------------------" << endl;
    tita t = findBestMove(field, &newShape, state.CurrentShape(), true, state.NextShape());
    //cerr << "Block should move turn right " << t.rotation << " times for the best move." << endl;
    //cerr << "Block should move left " << t.left << " times for the best move." << endl;
    //cerr << "Block should move right " << t.right << " times for the best move." << endl;
    //cerr << "This move has a score of " << t.score << endl;

    while (t.rotation > 0) {
        moves.push_back(Move::MoveType::TURNRIGHT);
        t.rotation--;
    }

    int vertical = t.left - t.right;
    while (vertical != 0) {
        if (vertical < 0) {
            moves.push_back(Move::MoveType::RIGHT);
            vertical++;
        }
        else if (vertical > 0) {
            moves.push_back(Move::MoveType::LEFT);
            vertical--;
        }
    }

    moves.push_back(Move::MoveType::DROP);
    return moves;
  }

  struct tita {
      int rotation;
      int right;
      int left;
      float score;
  };

  tita findBestMove(Field field, Shape *shape, Shape::ShapeType shapetype, bool first, Shape::ShapeType nextshape) const {      
      int totalRotations = 0;
      float bestscore = -999999;
      int totalRights = 0;
      int totalLefts = 0;
      float averageScore = 0;
      int tries = 0;

      //Gelen ?ekli 0, 1, 2 ve 3 kere döndür
      int rotations = 0;
      while (rotations < 4) {
          //cerr << "Testing block for " << rotations << ". rotation." << endl;

          //Sola ne kadar gidebilecegini gormek icin seklin kopyasini yarat
          Shape testShapeLeft(shapetype, field, shape->x(), shape->y());

          //Sola gidecek sekli gerektigi kadar saga dondur
          for (int r = 0; r < rotations; r++)
              testShapeLeft.TurnRight();

          //Seklin sola kac kere gidebilecegini hesapla
          int testLeftMoves = checkMove(&testShapeLeft, 'l');
          //cerr << "Block can move left " << testLeftMoves << " times." << endl;

          //Saga ne kadar gidebilecegini hesaplamak icin seklin kopyasini olustur
          Shape testShapeRight(shapetype, field, shape->x(), shape->y());

          //Bu sekli de digerleri kadar dondur
          for (int r = 0; r < rotations; r++)
              testShapeRight.TurnRight();

          //Saga gidecek sekli en sola gotur
          move(&testShapeRight, 'l', testLeftMoves);

          //Saga gidecek sekil su an donmus olarak en solda duruyor

          //Seklin saga ne kadar gidebilecegini hesapla
          int testRightMoves = checkMove(&testShapeRight, 'r');
          //cerr << "Block can move right a total of " << testRightMoves << " times." << endl;
          
          //Sekil ne kadar saga gidebiliyosa o kadar dene
          int testReverse = 0;
          while (testReverse <= testRightMoves) {
              //cerr << "Block will move right " << testRightMoves << " times to test." << endl;

              //Sahanin kopyasini olustur
              Field newField(field.width(), field.height(), field.copyField());

              //Seklin puanini kontrol etmek icin kopyasini olusur
              Shape ghostShape(shapetype, field, shape->x(), shape->y());
              for (int r = 0; r < rotations; r++)
                  ghostShape.TurnRight();

              //Ghostshape'i hizala.
              move(&ghostShape, 'l', testLeftMoves);
              move(&ghostShape, 'r', testReverse);

              //Seklin ne kadar asagiya inebilecegini gormek icin kopyala
              Shape testShapeDown(shapetype, field, shape->x(), shape->y());

              //Bu sekli de ayni miktarda dondur
              for (int r = 0; r < rotations; r++)
                  testShapeDown.TurnRight();
              //Down shapei ghostla hizala
              move(&testShapeDown, 'l', testLeftMoves);
              move(&testShapeDown, 'r', testReverse);

              //Seklin asagiya ne kadar gidebilecegini hesapla
              int testDownMoves = checkMove(&testShapeDown, 'd');
              //cerr << "Tested down. This piece has to move down " << testDownMoves << " times." << endl;

              //Ghostshape'i hesapladigimiz kadar asagiya indir
              move(&ghostShape, 'd', testDownMoves);

              //Kopyalamis oldugumuz sahaya bu sekli blok olarak yerlestir
              for (const Cell* cell : ghostShape.GetBlocks()) {
                  const Cell& c = *cell;
                  newField.SetCell(c.x(), c.y());
              }

              //Sahanin puanini hesapla
              //cerr << "Testing for " << rotations << "rotations and " << testReverse << " rights: ";
              float score = evaluate(&newField);

              //En yuksek puanli hareketi hatirla
              if(first){
                  if (averageScore == 0)
                      averageScore = score;
                  else
                      averageScore = ((averageScore * tries) + score) / (tries + 1);
                  tries++;
                  cerr << "AS=" << averageScore << " S=" << score;
                  if(score >= averageScore){
                      cerr << " -> Testing next shape" << endl;
                      Shape nextShape(nextshape, newField, shape->x(), shape->y());
                      tita next = findBestMove(newField, &nextShape, nextshape, false, nextshape);
                      if (score + next.score > bestscore) {
                          bestscore = score + next.score;
                          totalLefts = testLeftMoves;
                          totalRights = testReverse;
                          totalRotations = rotations;
                      }
                  } else {
                      cerr << " -> NOT testing next shape" << endl;
                  }
              } else {
                  if (score > bestscore) {
                      bestscore = score;
                      totalLefts = testLeftMoves;
                      totalRights = testReverse;
                      totalRotations = rotations;
                  }
              }
              

              //Bir soldaki durum icin tekrar dene
              testReverse++;
          }
          //Cevirip tekrar dene
          rotations++;
      }

      //En iyi hareketi dondur
      tita t;
      t.right = totalRights;
      t.left = totalLefts;
      t.rotation = totalRotations;
      t.score = bestscore;
      return t;
  }

  float evaluate(Field *field) const {
      float score = -9999;

      float maxHeight = 0;
      for (int i = 0; i < field->width(); i++) {
          for (int j = 0; j < field->height(); j++) {
              if (field->GetCell(i, j).state() == Cell::CellState::BLOCK || field->GetCell(i, j).state() == Cell::CellState::SOLID) {
                  if(maxHeight < field->height() - j)
                    maxHeight = field->height() - j;
                  break;
              }
          }
      }

      int solidHeight = 0;
      for (int i = 0; i < field->width(); i++) {
          for (int j = 0; j < field->height(); j++) {
              if (field->GetCell(i, j).state() == Cell::CellState::SOLID) {
                  solidHeight = field->height() - j;
                  break;
              }
          }
          break;
      }
      
      float aggregateHeight = 0;
      for (int i = 0; i < field->width(); i++) {
          for (int j = 0; j < field->height(); j++) {
              if (field->GetCell(i, j).state() == Cell::CellState::BLOCK || field->GetCell(i, j).state() == Cell::CellState::SOLID) {
                  //cerr << "Agg height starts at " << field->height()-j << " for the " << i << "th column." << endl;
                  aggregateHeight = aggregateHeight + field->height() - j - solidHeight;
                  break;
              } // SOLIDLERI ÇIKAR
          }
      }
      //cerr << endl;

      float completedLines = 0;
      for (int i = 0; i < field->height(); i++) {
          int checkFullRow = 0;
          for (int j = 0; j < field->width(); j++) {
              if (field->GetCell(i, j).state() == Cell::CellState::EMPTY) {
                  checkFullRow++;
              }
          }
          if (checkFullRow == 0)
              completedLines++;
      }

      float holes = 0;
      for (int i = 0; i < field->width(); i++) {
          bool startChecking = false;
          for (int j = 0; j < field->height(); j++) {
              if (field->GetCell(i, j).state() == Cell::CellState::BLOCK && !startChecking)
                      startChecking = true;
              if (field->GetCell(i, j).state() == Cell::CellState::EMPTY && startChecking)
                  holes++;
          }
      }

      float blockades = 0;
      for (int i = 0; i < field->width(); i++) {
          bool startChecking = false;
          for (int j = field->height(); j > 0; j--) {
              if (field->GetCell(i, j).state() == Cell::CellState::EMPTY && !startChecking)
                  startChecking = true;
              if (field->GetCell(i, j).state() == Cell::CellState::BLOCK && startChecking)
                  blockades++;
          }
      }

      float bumpiness = 0;
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

      score = (-0.510066) * aggregateHeight + (0.760666) * completedLines + (-0.35663) * holes + (-0.184483) * bumpiness + (-0.2) * maxHeight + (-0.05) * blockades;
      //cerr << "Agg: " << aggregateHeight << ". Comp: " << completedLines << ". Hole: " << holes << ". Bump: " << bumpiness << ". Blok: " << blockades << ". MaxH: " << maxHeight << ". SolidH: " << solidHeight << ". Score" << score << endl;
      /*for (int i = 0; i < field->height(); i++) {
          for (int j = 0; j < field->width(); j++) {
              cerr << "|" << field->GetCell(j, i).AsString();
          }
          cerr << "|" << endl;
      }*/
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

  void move(Shape *shape, char side, int amount) const {
      for (int i = 0; i < amount; i++) {
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
              cerr << "CANT MOVE. ABORT!" << endl;
          }
      }
  }

};

#endif  //__BOT_STARTER_H
