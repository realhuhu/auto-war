#ifndef RED_RED_MANAGER_H
#define RED_RED_MANAGER_H

#include <QDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QJsonArray>
#include <QMessageBox>
#include <QJsonObject>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidget>
#include <QDialogButtonBox>

#include "../../../util/state.h"

class RedManger : public QDialog {
Q_OBJECT

public:
    enum Column {
        RemarkCol = 0,
        QQCol = 1,
        RegionCol = 2,
        ActionCol = 3
    };

    QTableWidget *tableWidget;

    explicit RedManger(QWidget *parent = nullptr);

    int getIndex(QObject *button) const;

    void insertRow(
            const QString &remark,
            const QString &qq,
            const QString &region
    );

    void loadConfig();

    void saveConfig();

    void closeEvent(QCloseEvent *event) override;

public slots:

    void handleConfig(int row);

    void handleDelete(int row);

    void addNew();

signals:

    void configChanged();
};

#endif //RED_RED_MANAGER_H
