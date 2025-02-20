#ifndef QT_REPLACEDIALOG_H
#define QT_REPLACEDIALOG_H

#include <windows.h>

#include <QLabel>
#include <QTimer>
#include <QScreen>
#include <QDialog>
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
#include <QPushButton>
#include <QMainWindow>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <QGuiApplication>

# include "../flow/cv.h"
# include "../util/state.h"

class ScreenshotArea : public QWidget {
Q_OBJECT
public:
    explicit ScreenshotArea(QWidget *parent = nullptr);

    ~ScreenshotArea() override;

signals:

    void screenshotCaptured(const QPoint p1, const QPoint p2);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *) override;

    void keyPressEvent(QKeyEvent *event) override;

    void updateScreenCache();

private:
    QTimer *refreshTimer;
    QPixmap cachedScreen;
    QElapsedTimer frameTimer;

    QPoint origin;
    QPoint current;
    QPoint mousePos;
    bool selecting = false;

    void showEvent(QShowEvent *event) override;

    void closeEvent(QCloseEvent *event) override;
};

class ReplaceDialog : public QDialog {
Q_OBJECT
public:
    explicit ReplaceDialog(QWidget *parent = nullptr);

private slots:

    void captureScreenshot() const;

    void showScreenshot(QPoint p1, QPoint p2);

    void openFile();

    void saveFile();

private:
    QLabel *fileLabel;
    QLabel *tipLabel;
    QLabel *selectedImageLabel;
    QLabel *croppedImageLabel;
    cv::Mat croppedImage;
    QString selectedPath;

    void closeEvent(QCloseEvent *) override;
};

#endif //QT_REPLACEDIALOG_H
