#ifndef RED_EMITTER_H
#define RED_EMITTER_H

#include <QObject>


class Emitter : public QObject {
Q_OBJECT
public:
    static Emitter *instance() {
        static auto *emitter = new Emitter();
        return emitter;
    }

signals:

    void log(const QString &remark, const QString &message, const QString &color = "black");
};

#endif //RED_EMITTER_H
