#include "browserSelector.h"

BrowserSelector::BrowserSelector(
        const QMap<QString, RedWorker *> &workers,
        QWidget *parent
) : QDialog(parent), m_workers(workers) {
    setWindowTitle("选择要操作的窗口");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_listWidget = new QListWidget(this);
    for (const QString &key: m_workers.keys()) {
        m_listWidget->addItem(key);
    }

    auto okButton = new QPushButton("OK", this);
    connect(okButton, &QPushButton::clicked, this, &BrowserSelector::onOkClicked);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_listWidget);
    layout->addWidget(okButton);

    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &BrowserSelector::onItemDoubleClicked);
}

void BrowserSelector::onItemDoubleClicked(QListWidgetItem *item) {
    QString key = item->text();
    if (m_workers.contains(key)) {
        emit browserSelected(m_workers.value(key));
        accept();
    }
}

void BrowserSelector::onOkClicked() {
    QList<QListWidgetItem *> selectedItems = m_listWidget->selectedItems();
    if (!selectedItems.isEmpty()) {
        QString key = selectedItems.first()->text();
        if (m_workers.contains(key)) {
            emit browserSelected(m_workers.value(key));
            accept();
        }
    }
}