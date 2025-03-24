#ifndef RED_RED_CONTROLLER_H
#define RED_RED_CONTROLLER_H

#include <QLabel>
#include <QWidget>
#include <QDateTime>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

class RedController : public QWidget {
Q_OBJECT

public:
    QString remark;
    QTextEdit *logTextEdit = new QTextEdit(this);

    explicit RedController(QString redRemark, QWidget *parent = nullptr);

public slots:

    void log(const QString &text, const QString &color = "black") const;

signals:

    void refreshBrowser();
};


#endif //RED_RED_CONTROLLER_H
