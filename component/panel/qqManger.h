#ifndef RED_QQ_MANGER_H
#define RED_QQ_MANGER_H

#include <QTimer>
#include <QDialog>
#include <QPainter>
#include <QProcess>
#include <QLineEdit>
#include <QClipboard>
#include <QJsonArray>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QTableWidget>
#include <QInputDialog>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "qqBrowser.h"
#include "../unit/placeholderDelegate.h"
#include "../../../util/state.h"
#include "../../../cheat/memory.h"


class QQManger : public QDialog {
Q_OBJECT

public:
    enum Column {
        RemarkCol = 0,
        QQNumberCol = 1,
        PasswordCol = 2,
        GameIDCol = 3,
        LinkCol = 4,
        StatusCol = 5,
        ActionCol = 6
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
    QTableWidget *tableWidget;
    QNetworkAccessManager *networkManager;

    explicit QQManger(QWidget *parent = nullptr);

    int getIndex(QObject *button) const;

    void insertRow(
            const QString &remark,
            const QString &qq = "",
            const QString &password = "",
            const QString &id = "",
            const QString &link = ""
    );

    void setStatus(int row, Status status) const;

    static int scanPID(const QString &id);

    static QString scanLink(int pid);

    void loadConfig();

    void saveConfig();

    void handleTest(int row);

    void handleLogin(int row);

    void handleDelete(int row);

    void closeEvent(QCloseEvent *event) override;

public slots:

    void onAdd();

    void onTestAll();

    void onLoginAll();

signals:

    void configChanged();
};

#endif //RED_QQ_MANGER_H
