#ifndef RED_STATE_H
#define RED_STATE_H

#include <QJsonObject>

struct State {
    QJsonObject config;
};

extern State state;

#endif // RED_STATE_H