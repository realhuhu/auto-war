#include "labelComboBox.h"

LabelComboBox::LabelComboBox(
        const QString &text,
        const QJsonArray &options,
        const QString &initialValue,
        QWidget *parent
) : QWidget(parent),label(new QLabel(text + ":")),comboBox( new QComboBox()) {
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(label);

    comboBox->setFixedWidth(100);
    comboBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    for (const auto &option: options) comboBox->addItem(option.toString());
    if (!initialValue.isEmpty()) comboBox->setCurrentText(initialValue);
    layout->addWidget(comboBox);

    layout->addStretch(1);
}