#ifndef RED_QQ_BROWSER_H
#define RED_QQ_BROWSER_H

#include <QDir>
#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "../../processor/env.h"
#include "../../interceptor/link.h"

class QQBrowser : public QDialog {
Q_OBJECT

public:
    QString modifiedRemark;
    QWebEngineView *browser;
    static QMap<QString, QWebEngineProfile *> profileMap;

    explicit QQBrowser(QWidget *parent, const QString &remark);

    void closeEvent(QCloseEvent *event) override;

    ~QQBrowser() override;

public slots:

    void onLinkDetected(QUrl url);

signals:

    void linkDetected(QUrl url);
};

#endif //RED_QQ_BROWSER_H
