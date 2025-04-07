#include "labelSpinBox.h"

LabelSpinBox::LabelSpinBox(
        const QString &text,
        int value,
        QWidget *parent
) : QWidget(parent), label(new QLabel(text + ":")), spinBox(new QSpinBox()) {
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(label);

    spinBox->setRange(-1000, 1000);
    spinBox->setValue(value);
    spinBox->setFixedWidth(100);
    spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(spinBox);
    layout->addStretch(1);
}