#include "LabeledComboBox.h"

LabeledComboBox::LabeledComboBox(
        const QString &text,
        const QJsonArray &options,
        const QString &initialValue
) : QWidget(nullptr) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    label = new QLabel(text + ":");
    comboBox = new QComboBox();
    comboBox->setFixedWidth(100);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    for (const auto &option: options) comboBox->addItem(option.toString());

    if (!initialValue.isEmpty()) comboBox->setCurrentText(initialValue);

    layout->addWidget(label);
    layout->addWidget(comboBox);
    layout->addStretch(1);

    setLayout(layout);
}

QComboBox *LabeledComboBox::getComboBox() { return comboBox; }