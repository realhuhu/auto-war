#ifndef QT_STATE_H
#define QT_STATE_H

#include <windows.h>
#include <QDateTime>

struct State {
    HWND hwnd = nullptr;
    QTextEdit *output_text = nullptr;
    QThread *currentThread = nullptr;
    std::atomic<bool> stopFlag{};

    void appendColoredText(const QString &text, const QColor &color = "red") const {
        // 创建HTML格式的字符串，其中包含了颜色信息
        QString html = QString("<div style=\"color:%1;\">%2</div>").arg(color.name(), text);

        // 将HTML字符串追加到QTextEdit中
        output_text->append(html);
    }

    void log(const QString &text, const QColor &color = "black") const {
        QDateTime currentDateTime = QDateTime::currentDateTime();

        // 提取时间部分
        QTime currentTime = currentDateTime.time();

        // 格式化时间为字符串 "HH:MM:SS"
        QString timeString = currentTime.toString("hh:mm:ss");

        QString html = QString(
                R"(
                    <div>
                    <span style="color:white;background-color:green;margin_right 5px">&nbsp;%1&nbsp;</span>
                    <span style="color:%2;">%3</span>
                    </div>
                    )"
        ).arg(timeString, color.name(), text);

        // 将HTML字符串追加到QTextEdit中
        output_text->append(html);
    }
};

State state;

#endif //QT_STATE_H
