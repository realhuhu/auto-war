#ifndef RED_QQ_MANGER_H
#define RED_QQ_MANGER_H

#include <QDialog>
#include <QPainter>
#include <QLineEdit>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTableWidget>
#include <QInputDialog>
#include <QRandomGenerator>

#include "qqLoginBrowser.h"
#include "../unit/placeholderDelegate.h"


class QQManger : public QDialog {
Q_OBJECT

public:
    explicit QQManger(QWidget *parent = nullptr);

    int getIndexThroughButton(QObject* button);

private slots:

    void handleTest(int row);

    void handleLogin(int row);

    void handleDelete(int row);

    void addNew();

    void testAll();

    void loginAll();

private:
    QTableWidget *tableWidget;
    enum StatusColumn {
        RemarkCol = 0,
        QQNumberCol = 1,
        PasswordCol = 2,
        LinkCol = 3,
        StatusCol = 4,
        ActionCol = 5
    };
};

#endif //RED_QQ_MANGER_H
