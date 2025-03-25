#ifndef RED_RED_CONFIGURER_H
#define RED_RED_CONFIGURER_H

#include <QLabel>
#include <QDialog>
#include <QCheckBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidget>
#include <QHBoxLayout>
#include <QStackedWidget>

#include "../../util/state.h"
#include "../unit/labelSpinBox.h"
#include "../unit/labelComboBox.h"

class RedConfigurer : public QDialog {
Q_OBJECT
public:
    explicit RedConfigurer(QString qqRemark, QString redRemark, QWidget *parent = nullptr);

private:
    struct ModuleConfig {
        int order;
        QJsonArray checkboxes;
        QJsonArray inputs;
        QJsonArray selects;
        QString tips;
    };

    static QWidget *createModulePage(const ModuleConfig &config);

    static QWidget *createCheckboxGroup(const QJsonArray &checkboxes);

    static QWidget *createSelectGroup(const QJsonArray &selects);

    static QWidget *createInputGroup(const QJsonArray &inputs);

    QString qqRemark;
    QString redRemark;
    QJsonObject config;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;
    QMap<QString, ModuleConfig> modules;
};

#endif //RED_RED_CONFIGURER_H
