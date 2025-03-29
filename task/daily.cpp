#include "daily.h"


void test(const Env& env) {
    Segment::env = env;
    emit env.emitter->log(QString("remark %1").arg(env.redRemark));
    emit env.emitter->log(QString("segment remark %1").arg(Segment::env.redRemark));
}