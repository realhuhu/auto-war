#include "daily.h"


void test(const QString& remark) {
    emit Emitter::instance()->log(remark, QString("remark %1").arg(remark));
}