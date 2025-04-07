#ifndef RED_BROWSER_SELECTOR_H
#define RED_BROWSER_SELECTOR_H

#include <QMap>
#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QListWidgetItem>

#include "redBrowser.h"

class BrowserSelector : public QDialog {
Q_OBJECT

public:
    explicit BrowserSelector(const QMap<QString, RedBrowser *> &browsers, QWidget *parent = nullptr);

signals:

    void browserSelected(RedBrowser *selectedBrowser);

private slots:

    void onItemDoubleClicked(QListWidgetItem *item);

    void onOkClicked();

private:
    QMap<QString, RedBrowser *> m_browsers;
    QListWidget *m_listWidget;
};

#endif //RED_BROWSER_SELECTOR_H
