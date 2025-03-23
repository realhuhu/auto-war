#ifndef RED_CONTROLLER_H
#define RED_CONTROLLER_H

#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include "../../util/tool.h"


class ControlPanel : public QWidget {
Q_OBJECT

public:
    WId browserWId;

    explicit ControlPanel(WId wid, QWidget *parent = nullptr);

signals:

    void refreshBrowser();
};


#endif //RED_CONTROLLER_H
