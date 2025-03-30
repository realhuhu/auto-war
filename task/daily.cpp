#include "daily.h"


void test(const Env &e) {
    env = e;
    emit env.emitter->log(QString("remark %1").arg(env.redRemark));
    emit env.emitter->log(QString("segment remark %1").arg(env.redRemark));
}