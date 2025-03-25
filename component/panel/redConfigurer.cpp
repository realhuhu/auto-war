#include "redConfigurer.h"

RedConfigurer::RedConfigurer(
        QString qq,
        QString remark,
        QWidget *parent
) : qqRemark(std::move(qq)), redRemark(std::move(remark)), QDialog(parent) {
    setWindowTitle(QString("配置账号: %1 %2").arg(qqRemark, redRemark));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    config = state.configDefault;

    for (auto it = config.begin(); it != config.end(); ++it) {
        auto obj = it.value().toObject();
        modules.insert(it.key(), ModuleConfig{
                .order = obj["order"].toInt(),
                .checkboxes = obj["checkbox"].toArray(),
                .inputs = obj["input"].toArray(),
                .selects = obj["select"].toArray(),
                .tips = obj["tips"].toString(),
        });
    }

    QList<QString> keys = modules.keys();
    std::sort(keys.begin(), keys.end(), [this](const QString &a, const QString &b) {
        return modules[a].order < modules[b].order;
    });

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    listWidget = new QListWidget(this);
    listWidget->setFixedWidth(100);
    std::sort(keys.begin(), keys.end(), [this](const QString &a, const QString &b) {
        return modules[a].order < modules[b].order;
    });
    for (const auto &key: keys) { listWidget->addItem(key); }
    listWidget->setCurrentRow(0);
    mainLayout->addWidget(listWidget);

    stackedWidget = new QStackedWidget(this);
    std::sort(keys.begin(), keys.end(), [this](const QString &a, const QString &b) {
        return modules[a].order < modules[b].order;
    });
    for (const auto &key: keys) { stackedWidget->addWidget(createModulePage(modules[key])); }
    mainLayout->addWidget(stackedWidget);

    connect(listWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
}


QWidget *RedConfigurer::createModulePage(const ModuleConfig &redCconfig) {
    auto page = new QWidget;
    auto layout = new QVBoxLayout(page);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(20);

    if (!redCconfig.checkboxes.isEmpty()) { layout->addWidget(createCheckboxGroup(redCconfig.checkboxes)); }
    if (!redCconfig.selects.isEmpty()) { layout->addWidget(createSelectGroup(redCconfig.selects)); }
    if (!redCconfig.inputs.isEmpty()) { layout->addWidget(createInputGroup(redCconfig.inputs)); }
    if (!redCconfig.tips.isEmpty()) {
        auto tipsLabel = new QLabel(redCconfig.tips);
        tipsLabel->setWordWrap(true);
        layout->addWidget(tipsLabel);
    }
    layout->addStretch();

    return page;
}

QWidget *RedConfigurer::createCheckboxGroup(const QJsonArray &checkboxes) {
    auto group = new QWidget;
    auto grid = new QGridLayout(group);
    grid->setContentsMargins(0, 0, 0, 0);

    for (const auto &item: checkboxes) {
        auto obj = item.toObject();
        int order = obj["order"].toInt();
        int row = order / 3;
        int col = order % 3;

        auto cb = new QCheckBox(obj["text"].toString());
        cb->setChecked(obj["value"].toBool());
        grid->addWidget(cb, row, col);
    }

    return group;
}

QWidget *RedConfigurer::createSelectGroup(const QJsonArray &selects) {
    auto group = new QWidget;
    auto grid = new QGridLayout(group);
    grid->setContentsMargins(0, 0, 0, 0);

    for (const auto &select: selects) {
        auto obj = select.toObject();
        int order = obj["order"].toInt();
        int row = order / 2;
        int col = order % 2;

        auto combo = new LabelComboBox(
                obj["text"].toString(),
                obj["options"].toArray(),
                obj["value"].toString()
        );
        grid->addWidget(combo, row, col);
    }

    return group;
}

QWidget *RedConfigurer::createInputGroup(const QJsonArray &inputs) {
    auto group = new QWidget;
    auto grid = new QGridLayout(group);
    grid->setContentsMargins(0, 0, 0, 0);

    for (const auto &input: inputs) {
        auto obj = input.toObject();
        int order = obj["order"].toInt();
        int row = order / 2;
        int col = order % 2;

        auto spin = new LabelSpinBox(
                obj["text"].toString(),
                obj["value"].toInt()
        );
        grid->addWidget(spin, row, col);
    }

    return group;
}