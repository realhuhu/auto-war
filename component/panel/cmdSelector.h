#ifndef RED_CMD_SELECTOR_H
#define RED_CMD_SELECTOR_H

#include <QDialog>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QPushButton>


#include "../../task/daily.h"
#include "../../task/battle.h"
#include "../../task/expedition.h"
#include "../../task/presupposition.h"
#include "../../processor/env.h"

class CmdSelector : public QDialog {
Q_OBJECT
public:
    QMap<QString, std::function<void(Env &env)>> tasks;

    explicit CmdSelector(const QString& title,QWidget *parent);

    QGroupBox *createCmdGroup(const QString &groupTitle, const QStringList &cmdList);

    std::function<void(Env &env)> createTask(const QString &cmd);

signals:

    void taskCreated(QString command, std::function<void(Env &env)> task);

};

#endif //RED_CMD_SELECTOR_H
