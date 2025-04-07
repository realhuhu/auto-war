#ifndef RED_FUNC_SELECTOR_H
#define RED_FUNC_SELECTOR_H

#include <QUrl>
#include <QDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QDesktopServices>
#include <QCoreApplication>

#include "browserSelector.h"
#include "../function/clickerDialog.h"
#include "../function/replaceDialog.h"
#include "../function/activityDialog.h"

class FuncSelector : public QDialog {
Q_OBJECT
public:
    explicit FuncSelector(const QMap<QString, RedBrowser *> &browsers, QWidget *parent);

signals:

    void toConsole(const QString &text, const QString &color = "black") const;
};


#endif //RED_FUNC_SELECTOR_H
