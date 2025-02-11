#include "buttonWithSetting.h"

ButtonWithSetting::ButtonWithSetting(
        const QString &text,
        bool showSettingButton,
        QWidget *parent
) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    textButton = new QPushButton(text);
    textButton->setFixedHeight(25);
    textButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(textButton);

    if (showSettingButton) {
        settingButton = new QToolButton();
        settingButton->setIcon(QIcon(":/resource/ui/setting.png"));
        settingButton->setIconSize(QSize(16, 16));
        settingButton->setFixedHeight(25);
        settingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        layout->addWidget(settingButton);
    } else {
        settingButton = nullptr;
    }

    setLayout(layout);
}

QPushButton *ButtonWithSetting::getTextButton() const { return textButton; }

QToolButton *ButtonWithSetting::getSettingButton() const { return settingButton; }