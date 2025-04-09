#ifndef RED_BROWSER_SELECTOR_H
#define RED_BROWSER_SELECTOR_H

#include <QMap>
#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QListWidgetItem>

#include "redWorker.h"

class BrowserSelector : public QDialog {
Q_OBJECT

public:
    explicit BrowserSelector(const QMap<QString, RedWorker *> &workers, QWidget *parent = nullptr);

signals:

    void browserSelected(RedWorker *selectedBrowser);

private slots:

    void onItemDoubleClicked(QListWidgetItem *item);

    void onOkClicked();

private:
    QMap<QString, RedWorker *> m_workers;
    QListWidget *m_listWidget;
};

#endif //RED_BROWSER_SELECTOR_H
