#ifndef RED_RED_GAME_BROWSER_H
#define RED_RED_GAME_BROWSER_H

#include <QDir>
#include <QDialog>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "redController.h"

class RedGameBrowser : public QDialog {
Q_OBJECT

public:
    QString url;
    QString remark;
    RedController *panel;
    QWebEngineView *browser;
    static QMap<QString, QWebEngineProfile *> profileMap;

    explicit RedGameBrowser(
            QTabWidget *tabWidget,
            const QString &qqRemark,
            const QString &redRemark,
            const QString &link,
            int region
    );

    void closeEvent(QCloseEvent *event) override;

    ~RedGameBrowser() override;

public slots:

    void refresh() const;

signals:

    void closed(QString remark);
};

#endif //RED_RED_GAME_BROWSER_H
