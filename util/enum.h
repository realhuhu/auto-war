#ifndef QT_ENUM_H
#define QT_ENUM_H

#include <iostream>

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

std::string PreviousToString(Previous previous);

#endif //QT_ENUM_H
