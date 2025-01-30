#ifndef QT_IMAGEDIALOG_H
#define QT_IMAGEDIALOG_H

#include <QLabel>
#include <QDialog>
#include <QWidget>
#include <QPixmap>
#include <QVBoxLayout>

class ImageDialog : public QDialog {
Q_OBJECT
public:
    explicit ImageDialog(const QImage &image, QWidget *parent = nullptr);

private:
    QLabel *imageLabel;
};

#endif //QT_IMAGEDIALOG_H
