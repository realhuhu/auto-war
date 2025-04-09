#ifndef RED_BROWSER_H
#define RED_BROWSER_H

#include <QDir>
#include <QWidget>
#include <QVariant>
#include <QJsonObject>
#include <QHBoxLayout>
#include <QLocalSocket>
#include <QApplication>
#include <QStyleFactory>
#include <QJsonDocument>
#include <QWebEngineView>
#include <QCoreApplication>
#include <QCoreApplication>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QCommandLineParser>

class Browser : public QWidget {
Q_OBJECT

public:
    QString url;
    QString remark;
    QLocalSocket *socket;
    QWebEngineView *browser;

    explicit Browser(
            QString redUrl,
            const QString &title,
            QString redRemark,
            QWidget *parent = nullptr
    );

    void refresh() const;

public slots:

    void onMainData() const;
};

#endif //RED_BROWSER_H
