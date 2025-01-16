#ifndef QT_EMITTER_H
#define QT_EMITTER_H

#include <QObject>

class SignalEmitter : public QObject {
Q_OBJECT
public:
    static SignalEmitter *instance() {
        static auto *emitter = new SignalEmitter();
        return emitter;
    }

signals:

    void logMessage(const QString &message, const QString &color = "black");
};

#endif //QT_EMITTER_H
