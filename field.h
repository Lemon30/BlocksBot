// Christos Savvopoulos <savvopoulos@gmail.com>
// Elias Sprengel <blockbattle@webagent.eu>

#ifndef __FIELD_H
#define __FIELD_H

#include <cassert>
#include <cstdlib>
#include <memory>
#include <string>

#include "cell.h"


using namespace std;

/**
 * Represents the playing field for one player.
 * Has some basic methods already implemented.
 */
class Field {
 public:
  // Parses the input string to get a grid with Cell objects.
  Field(int width, int height, const string& fieldStr)
      : width_(width), height_(height), grid_(width * height) {
    int x = 0;
    int y = 0;
    aggregate_height_ = 0;
    const char* strPos = fieldStr.c_str();

    while (strPos < fieldStr.c_str() + fieldStr.size()) {
      // Read cell code.
      char* strNext = nullptr;
      int cellCode = strtol(strPos, &strNext, 10);
      assert(strPos != strNext);  // check that we read sth.
      strPos = strNext;
      strNext = nullptr;

      // Update this cell.
      grid_[y * width + x].SetLocation(x, y);
      grid_[y * width + x].set_state(cellCode);

      // Advance position, parse separator.
      x++;
      if (x == width_) {
        assert(*strPos == ';' || y == height_ - 1);
        x = 0;
        y++;
      } else {
        assert(*strPos == ',');
      }
      strPos++;
    }

    //Calculate total aggregate height of the field
    for (int i = 0; i < width_; i++) {
        for (int j = height_-1; j > 0; j--) {
            const Cell& field_cell = GetCell(i, j);
            if (!field_cell.IsEmpty()) {
                SetAggregateHeight(aggregate_height_ + j);
                break;
            }
        }
    }
  }

  

  bool IsOutOfBounds(const Cell& c) {
    return c.x() >= width_ || c.x() < 0 || c.y() >= height_ || c.y() < 0;
  }

  bool HasCollision(const Cell& block_cell) const {
    const Cell& field_cell = GetCell(block_cell.x(), block_cell.y());
    return (block_cell.IsShape() &&
            (field_cell.IsSolid() || field_cell.IsBlock()));
  }

  void setCell(const Cell& cell) {
      if (cell.x() < 0 || cell.x() >= width_ || cell.y() < 0 || cell.y() >= height_) {
          return;
      }
      grid_[cell.y() * width_ + cell.x()].setBlock();
  }

  Field copyField() {
      string board = "";

      for (int i = 0; i < height_; i++) {
          for (int j = 0; j < height_; j++) {
              Cell::CellState stat = grid_[j * width_ + i].getState();
              switch (stat) {
              case ' ': //EMPTY
                  board = board + "0,";
                  break;
              case '+': //Shape
                  board = board + "1,";
                  break;
              case '#': //BLOCK
                  board = board + "2,";
                  break;
              case '-': //SOLID
                  board = board + "3,";
                  break;
              }
          }
          board = board + ";";
      }
      return Field(width_, height_, board);
  }

  int GetAggregateHeight() const { return aggregate_height_; }

  void SetAggregateHeight(int val) { aggregate_height_ = val; }

  const Cell& GetCell(int x, int y) const { return grid_[y * width_ + x]; }

  int width() const { return width_; }

  int height() const { return height_; }

 private:
  int width_;
  int height_;
  int aggregate_height_;
  vector<Cell> grid_;
};

#endif  // __FIELD_H
