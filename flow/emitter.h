#ifndef QT_EMITTER_H
#define QT_EMITTER_H

#include <QObject>


class Emitter : public QObject {
Q_OBJECT
public:
    static Emitter *instance() {
        static auto *emitter = new Emitter();
        return emitter;
    }

signals:

    void log(const QString &message, const QString &color = "black", bool force = false);
};

#endif //QT_EMITTER_H
