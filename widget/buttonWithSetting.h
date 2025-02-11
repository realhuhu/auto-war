#ifndef QT_BUTTONWITHSETTING_H
#define QT_BUTTONWITHSETTING_H

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QHBoxLayout>

class ButtonWithSetting : public QWidget {
Q_OBJECT
public:
    explicit ButtonWithSetting(
            const QString &text,
            bool showSettingButton = true,
            QWidget *parent = nullptr
    );

    [[nodiscard]] QPushButton *getTextButton() const;
    [[nodiscard]] QToolButton *getSettingButton() const;

private:
    QPushButton *textButton;
    QToolButton *settingButton;
};


#endif //QT_BUTTONWITHSETTING_H
