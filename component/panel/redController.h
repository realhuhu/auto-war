#ifndef RED_RED_CONTROLLER_H
#define RED_RED_CONTROLLER_H

#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

class RedController : public QWidget {
Q_OBJECT

public:
    QString remark;

    explicit RedController(QString redRemark, QWidget *parent = nullptr);

signals:

    void refreshBrowser();
};


#endif //RED_RED_CONTROLLER_H
