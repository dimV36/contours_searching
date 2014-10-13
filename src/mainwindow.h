#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class MainWindow;
}

using namespace cv;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QImage _input_image;
    QImage _output_image;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setFileNameOnTittle(const QString& file_name);
    void setInputImage(const QString& file_name);
    void updateButtons();
    QImage cvMatToQImage(const Mat &matrix);
    void updateOutputImage();
    bool saveAs();
    bool save(const QString& file_name);

private slots:
    void on__action_open_triggered();
    void on__action_save_triggered();
    void on__action_save_as_triggered();
    void on__action_search_countours_triggered();

    void on__slider_threshold_valueChanged(int value);

private:
    Ui::MainWindow *_ui;
    QString _file_name;
    int _threshold;
};

#endif // MAINWINDOW_H
