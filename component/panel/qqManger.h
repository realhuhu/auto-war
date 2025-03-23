#ifndef RED_QQ_MANGER_H
#define RED_QQ_MANGER_H

#include <QTimer>
#include <QDialog>
#include <QPainter>
#include <QLineEdit>
#include <QJsonArray>
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
#include "../../../util/state.h"


class QQManger : public QDialog {
Q_OBJECT

public:
    QTableWidget *tableWidget;
    enum StatusColumn {
        RemarkCol = 0,
        QQNumberCol = 1,
        PasswordCol = 2,
        LinkCol = 3,
        StatusCol = 4,
        ActionCol = 5
    };

    explicit QQManger(QWidget *parent = nullptr);

    int getIndex(QObject *button) const;

    void insertRow(
            const QString &remark,
            const QString &qq = "",
            const QString &password = "",
            const QString &link = ""
    );

    void loadConfig();

    void saveConfig();

    void closeEvent(QCloseEvent *event) override;

public slots:

    void handleTest(int row) const;

    void handleLogin(int row);

    void handleDelete(int row);

    void addNew();

    void testAll();

    void loginAll();

signals:

    void configChanged();
};

#endif //RED_QQ_MANGER_H
