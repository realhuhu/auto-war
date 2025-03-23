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
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include "qqLoginBrowser.h"
#include "../unit/placeholderDelegate.h"
#include "../../../util/state.h"


class QQManger : public QDialog {
Q_OBJECT

public:
    QTableWidget *tableWidget;
    enum Column {
        RemarkCol = 0,
        QQNumberCol = 1,
        PasswordCol = 2,
        LinkCol = 3,
        StatusCol = 4,
        ActionCol = 5
    };
    enum Status {
        Waiting = 0,
        Empty = 1,
        Testing = 2,
        Timeout = 3,
        NetError = 4,
        Valid = 5,
        Invalid = 6
    };
    QNetworkAccessManager *networkManager;

    explicit QQManger(QWidget *parent = nullptr);

    int getIndex(QObject *button) const;

    void insertRow(
            const QString &remark,
            const QString &qq = "",
            const QString &password = "",
            const QString &link = ""
    );

    void setStatus(int row, Status status);

    void loadConfig();

    void saveConfig();

    void closeEvent(QCloseEvent *event) override;


public slots:

    void handleTest(int row);

    void handleLogin(int row);

    void handleDelete(int row);

    void addNew();

    void testAll();

    void loginAll();

signals:

    void configChanged();
};

#endif //RED_QQ_MANGER_H
