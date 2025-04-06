#ifndef RED_EMITTER_H
#define RED_EMITTER_H

#include <QObject>


class Emitter : public QObject {
Q_OBJECT
public:
    explicit Emitter();

signals:

    void log(const QString &text, const QString &color = "black") const;

    void error(const QString &text) const;
};

#endif //RED_EMITTER_H
