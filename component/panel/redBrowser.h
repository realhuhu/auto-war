#ifndef RED_REDBROWSER_H
#define RED_REDBROWSER_H

#include <QDir>
#include <QDialog>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "controller.h"
#include "../../util/tool.h"


class RedBrowser : public QDialog {
Q_OBJECT

public:
    QString url;
    QString remark;
    ControlPanel *panel;
    QWebEngineView *browser;
    static QMap<QString, QWebEngineProfile *> profileMap;

    explicit RedBrowser(
            QTabWidget *tabWidget,
            const QString &qqRemark,
            const QString &redRemark,
            const QString &link,
            int region
    );

    void closeEvent(QCloseEvent *event) override;

    ~RedBrowser() override;

public slots:

    void refresh() const;

signals:

    void closed(QString remark);
};

#endif //RED_REDBROWSER_H
