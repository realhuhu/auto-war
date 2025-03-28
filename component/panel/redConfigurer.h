#ifndef RED_RED_CONFIGURER_H
#define RED_RED_CONFIGURER_H

#include <QLabel>
#include <QDialog>
#include <QCheckBox>
#include <QJsonArray>
#include <QJsonObject>
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
    struct ModuleConfig {
        int order;
        QJsonArray checkboxes;
        QJsonArray inputs;
        QJsonArray selects;
        QString tips;
    };

    QString qqRemark;
    QString redRemark;
    QJsonObject config;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;
    QMap<QString, ModuleConfig> modules;
    QMap<QString, QList<QCheckBox *>> moduleCheckboxes;
    QMap<QString, QList<LabelSpinBox *>> moduleInputs;
    QMap<QString, QList<LabelComboBox *>> moduleSelects;

    explicit RedConfigurer(QString qqRemark, QString redRemark, QWidget *parent = nullptr);

    QWidget *createModulePage(const QString& moduleName);

    QWidget *createCheckboxGroup(const QString& moduleName, const QJsonArray &checkboxes);

    QWidget *createSelectGroup(const QString& moduleName, const QJsonArray &selects);

    QWidget *createInputGroup(const QString& moduleName, const QJsonArray &inputs);

    void closeEvent(QCloseEvent *event) override;

};

#endif //RED_RED_CONFIGURER_H
