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
    bool saveAs();
    bool save(const QString& file_name);

private slots:
    void on__action_open_triggered();
    void on__action_save_triggered();
    void on__action_save_as_triggered();

    void on__action_search_countours_triggered();

private:
    Ui::MainWindow *_ui;
};

#endif // MAINWINDOW_H
