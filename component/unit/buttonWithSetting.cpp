#include "buttonWithSetting.h"

ButtonWithSetting::ButtonWithSetting(
        const QString &text,
        bool showSettingButton,
        QWidget *parent
) : QWidget(parent), textButton(new QPushButton(text)) {
    auto layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    textButton->setFixedHeight(25);
    textButton->setMinimumWidth(60);
    textButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(textButton);

    if (showSettingButton) {
        settingButton = new QToolButton();
        settingButton->setIcon(QIcon(":/ui/setting.png"));
        settingButton->setIconSize(QSize(16, 16));
        settingButton->setFixedHeight(25);
        settingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        layout->addWidget(settingButton);
    } else {
        settingButton = nullptr;
    }
}