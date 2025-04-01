#ifndef RED_ENUM_H
#define RED_ENUM_H

#include <iostream>
#include <QString>

enum class Mode {
    GRAY,
    RGB,
};

enum class Click {
    CENTER,
    LEFT,
    TOP,
    RIGHT,
    DOWN,
};

enum class Previous {
    LEFT,
    TOP,
    RIGHT,
    DOWN,
    LEFT_CENTER,
    TOP_CENTER,
    RIGHT_CENTER,
    DOWN_CENTER,
    INNER,
    NONE,
};

QString PreviousToString(Previous previous);

#endif //RED_ENUM_H
