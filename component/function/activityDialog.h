#ifndef RED_ACTIVITY_DIALOG_H
#define RED_ACTIVITY_DIALOG_H

#include <QTimer>
#include <QLabel>
#include <QDialog>
#include <QDateTime>
#include <QTableView>
#include <QEventLoop>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QStandardItemModel>
#include <QNetworkAccessManager>

struct ResultEntry {
    QString name;
    QDateTime startTime;
    QDateTime endTime;
};

class ActivityDialog : public QDialog {
Q_OBJECT
public:
    explicit ActivityDialog(QWidget *parent = nullptr);

    QVector<ResultEntry> getActivity();

    static QDateTime getTime(const QString &timeStr);

private slots:

    void fetchDataAndUpdateUI();

private:
    QLabel *label;
    QTableView *tableView;
    QStandardItemModel *model;
};

#endif //RED_ACTIVITY_DIALOG_H
