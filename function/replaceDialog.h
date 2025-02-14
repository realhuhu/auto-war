#ifndef QT_REPLACEDIALOG_H
#define QT_REPLACEDIALOG_H

#include <QLabel>
#include <QScreen>
#include <QDialog>
#include <QPainter>
#include <QPushButton>
#include <QMainWindow>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <QGuiApplication>

class ScreenshotArea : public QWidget {
Q_OBJECT
public:
    explicit ScreenshotArea(QWidget *parent = nullptr);

signals:

    void screenshotCaptured(const QPixmap &pixmap);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    QPoint origin;
    QPoint current;
    QPoint mousePos;
    bool selecting = false;
};

class ReplaceDialog : public QDialog {
Q_OBJECT
public:
    explicit ReplaceDialog(QWidget *parent = nullptr);

private slots:

    void captureScreenshot() const;

    void showScreenshot(const QPixmap &pixmap);

private:
    QLabel *label;
    void closeEvent(QCloseEvent *) override;
};

#endif //QT_REPLACEDIALOG_H
