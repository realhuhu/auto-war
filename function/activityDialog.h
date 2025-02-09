#ifndef QT_ACTIVITYDIALOG_H
#define QT_ACTIVITYDIALOG_H

#include <QTimer>
#include <QLabel>
#include <QDialog>
#include <QDateTime>
#include <QTableView>
#include <QEventLoop>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QXmlStreamReader>
#include <QNetworkAccessManager>

struct ResultEntry {
    QString name;
    QDateTime startTime;
    QDateTime endTime;
};

class ActivityResultDialog : public QDialog {
Q_OBJECT
public:
    explicit ActivityResultDialog(QWidget *parent = nullptr);

    QVector<ResultEntry> getActivity();

    static QDateTime getTime(const QString &timeStr);

private slots:

    void fetchDataAndUpdateUI();

private:
    QLabel *label;
    QTableView *tableView;
    QStandardItemModel *model;
};

#endif //QT_ACTIVITYDIALOG_H
