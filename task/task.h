// task.h
#ifndef QT_TASK_H
#define QT_TASK_H

#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>
#include <QString>

#include "../flow/until.h"
#include "../flow/segment.h"
#include "../flow/emitter.h"

void clear_until(
        std::vector<std::unique_ptr<Until>> &start_until,
        std::vector<std::unique_ptr<Until>> &click_until,
        std::vector<std::unique_ptr<Until>> &run_until
);

void guild_war();
void country_war();
void country_arena();
void world_arena();
void arms_compound();
void exterminate_enemy();

#endif // QT_TASK_H