// Umut Can Ozyar <umutcanozyar@yahoo.com>

#ifndef __PREDICTION_H
#define __PREDICTION_H

#include <string>
#include <vector>

#include "bot-state.h"
#include "move.h"
#include "shape.h"

using namespace std;
/**
* Predict the best possible move
*/
class Prediction {
public:
    void GetBestMove() {
        BotState state;
        Field field = state.MyField();
        Shape::ShapeType shape_type;
        Shape shape(state.CurrentShape(), field, state.ShapeLocation().first, state.ShapeLocation().second);
        if (state.CurrentShape() == 'I') {
            int horizontal_moves = 4;
            shape.OneLeft();
            shape.OneLeft();
            for (int i = 0; i < field.width(); i++) {
            }
        
        }

        //Test all possible places

    }

    
private:

};

#endif  //__PREDICTION_H
