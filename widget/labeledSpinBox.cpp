#include "LabeledSpinBox.h"

LabeledSpinBox::LabeledSpinBox(
        const QString &text,
        int value,
        QWidget *parent
) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    label = new QLabel(text + ":");
    spinBox = new QSpinBox();
    spinBox->setValue(value);
    spinBox->setMinimum(0);
    spinBox->setFixedWidth(100);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    layout->addWidget(label);
    layout->addWidget(spinBox);
    layout->addStretch(1);

    setLayout(layout);
}

QSpinBox *LabeledSpinBox::getSpinBox() const { return spinBox; }