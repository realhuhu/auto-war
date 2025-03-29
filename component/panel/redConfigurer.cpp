#include "redConfigurer.h"

RedConfigurer::RedConfigurer(
        QString qq,
        QString remark,
        QWidget *parent
) : qqRemark(std::move(qq)),
    redRemark(std::move(remark)), QDialog(parent),
    listWidget(new QListWidget(this)),
    stackedWidget(new QStackedWidget(this)) {
    setWindowTitle(QString("配置账号: %1 %2").arg(qqRemark, redRemark));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto setting = loadSetting(state.config, qqRemark, redRemark, state.settingDefault);
    for (auto it = setting.begin(); it != setting.end(); ++it) {
        auto obj = it.value().toObject();
        modules.insert(it.key(), ModuleConfig{
                .order = obj["order"].toInt(),
                .checkboxes = obj["checkbox"].toArray(),
                .inputs = obj["input"].toArray(),
                .selects = obj["select"].toArray(),
                .tips = obj["tips"].toString(),
        });
    }

    QList<QString> moduleNames = modules.keys();
    std::sort(moduleNames.begin(), moduleNames.end(), [this](const QString &a, const QString &b) {
        return modules[a].order < modules[b].order;
    });

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    listWidget->setFixedWidth(100);
    std::sort(moduleNames.begin(), moduleNames.end(), [this](const QString &a, const QString &b) {
        return modules[a].order < modules[b].order;
    });
    for (const auto &moduleName: moduleNames) { listWidget->addItem(moduleName); }
    listWidget->setCurrentRow(0);
    mainLayout->addWidget(listWidget);

    std::sort(moduleNames.begin(), moduleNames.end(), [this](const QString &a, const QString &b) {
        return modules[a].order < modules[b].order;
    });
    for (const auto &moduleName: moduleNames) { stackedWidget->addWidget(createModulePage(moduleName)); }
    mainLayout->addWidget(stackedWidget);

    connect(listWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
}

QWidget *RedConfigurer::createModulePage(const QString &moduleName) {
    auto page = new QWidget;
    auto layout = new QVBoxLayout(page);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(20);

    auto redConfig = modules[moduleName];
    if (!redConfig.checkboxes.isEmpty()) {
        layout->addWidget(createCheckboxGroup(moduleName, redConfig.checkboxes));
    }
    if (!redConfig.selects.isEmpty()) {
        layout->addWidget(createSelectGroup(moduleName, redConfig.selects));
    }
    if (!redConfig.inputs.isEmpty()) {
        layout->addWidget(createInputGroup(moduleName, redConfig.inputs));
    }
    if (!redConfig.tips.isEmpty()) {
        auto tipsLabel = new QLabel(redConfig.tips);
        tipsLabel->setWordWrap(true);
        layout->addWidget(tipsLabel);
    }
    layout->addStretch();

    return page;
}

QWidget *RedConfigurer::createCheckboxGroup(const QString &moduleName, const QJsonArray &checkboxes) {
    auto group = new QWidget;
    auto grid = new QGridLayout(group);
    grid->setContentsMargins(0, 0, 0, 0);

    for (const auto &item: checkboxes) {
        auto obj = item.toObject();
        auto order = obj["order"].toInt();
        auto text = obj["text"].toString();

        auto check = new QCheckBox();
        check->setChecked(obj["value"].toBool());
        check->setProperty("order", order);
        check->setProperty("text", text);

        grid->addWidget(check, order / 3, order % 3);
        moduleCheckboxes[moduleName].append(check);
    }

    return group;
}

QWidget *RedConfigurer::createSelectGroup(const QString &moduleName, const QJsonArray &selects) {
    auto group = new QWidget;
    auto grid = new QGridLayout(group);
    grid->setContentsMargins(0, 0, 0, 0);

    for (const auto &select: selects) {
        auto obj = select.toObject();
        auto order = obj["order"].toInt();
        auto text = obj["text"].toString();
        auto options = obj["options"].toArray();

        auto combo = new LabelComboBox(
                text,
                options,
                obj["value"].toString()
        );
        combo->setProperty("order", order);
        combo->setProperty("text", text);
        combo->setProperty("options", options);

        grid->addWidget(combo, order / 2, order % 2);
        moduleSelects[moduleName].append(combo);
    }

    return group;
}

QWidget *RedConfigurer::createInputGroup(const QString &moduleName, const QJsonArray &inputs) {
    auto group = new QWidget;
    auto grid = new QGridLayout(group);
    grid->setContentsMargins(0, 0, 0, 0);

    for (const auto &input: inputs) {
        auto obj = input.toObject();
        auto order = obj["order"].toInt();
        auto text = obj["text"].toString();

        auto spin = new LabelSpinBox(
                text,
                obj["value"].toInt()
        );
        spin->setProperty("order", order);
        spin->setProperty("text", text);


        moduleInputs[moduleName].append(spin);
        grid->addWidget(spin, order / 2, order % 2);
    }

    return group;
}

void RedConfigurer::closeEvent(QCloseEvent *event) {
    QJsonObject newSetting;

    for (auto it = modules.keyBegin(); it != modules.keyEnd(); ++it) {
        const QString &moduleName = *it;
        QJsonObject moduleConfig;

        QJsonArray checkboxArray;
        for (QCheckBox *cb: moduleCheckboxes[moduleName]) {
            checkboxArray.append(QJsonObject{
                    {"order", cb->property("order").toInt()},
                    {"text",  cb->property("text").toString()},
                    {"value", cb->isChecked()}
            });
        }
        moduleConfig["checkbox"] = checkboxArray;

        QJsonArray inputArray;
        for (LabelSpinBox *spin: moduleInputs[moduleName]) {
            inputArray.append(QJsonObject{
                    {"order", spin->property("order").toInt()},
                    {"text",  spin->property("text").toString()},
                    {"value", spin->spinBox->value()}
            });
        }
        moduleConfig["input"] = inputArray;

        QJsonArray selectArray;
        for (LabelComboBox *combo: moduleSelects[moduleName]) {
            selectArray.append(QJsonObject{
                    {"order",   combo->property("order").toInt()},
                    {"text",    combo->property("text").toString()},
                    {"value",   combo->comboBox->currentText()},
                    {"options", combo->property("options").toJsonArray()}
            });
        }
        moduleConfig["select"] = selectArray;

        moduleConfig["tips"] = modules[moduleName].tips;
        newSetting[moduleName] = moduleConfig;
    }

    auto configObj = state.config;
    auto accountArray = configObj["account"].toArray();
    for (int i = 0; i < accountArray.count(); ++i) {
        auto accountObj = accountArray[i].toObject();
        if (accountObj["remark"] != qqRemark) continue;

        auto redArray = accountObj["red"].toArray();
        for (int j = 0; j < redArray.count(); ++j) {
            auto redObj = redArray[j].toObject();
            if (redObj["remark"] != redRemark) continue;

            redObj["setting"] = newSetting;
            redArray[j] = redObj;
            accountObj["red"] = redArray;
            accountArray[i] = accountObj;
            configObj["account"] = accountArray;
            state.config = configObj;
            break;
        }
        break;
    }

    QDialog::closeEvent(event);
}
