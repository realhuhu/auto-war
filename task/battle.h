#ifndef QT_BATTLE_H
#define QT_BATTLE_H

#include <vector>
#include <memory>
#include <chrono>
#include <thread>

#include <QString>
#include <QFileInfo>

#include "common.h"
#include "../state.h"
#include "../flow/cv.h"
#include "../flow/enum.h"
#include "../flow/until.h"
#include "../flow/runner.h"
#include "../flow/emitter.h"


void countryWar();

void countryArena();

void worldArena();

void armsCompound();

void exterminateEnemy();

#endif // QT_BATTLE_H