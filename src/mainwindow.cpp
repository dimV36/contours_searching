#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow) {
    _ui -> setupUi(this);
    _threshold = 1;
}


MainWindow::~MainWindow() {
    delete _ui;
}


void MainWindow::setFileNameOnTittle(const QString &file_name) {
    setWindowTitle(file_name);
}


void MainWindow::setInputImage(const QString &file_name) {
    _file_name = file_name;
    _input_image.load(file_name);
    QImage image  = _input_image.scaled(_input_image.size() / 5);
    _ui -> _label_input -> setPixmap(QPixmap::fromImage(image));
}


void MainWindow::updateButtons() {
//    bool input_is_loaded = _ui -> _label_input -> pixmap() -> isNull();
//    bool output_is_set = ui_ -> label_output_ -> pixmap();
//    if (false == input_is_loaded)
//       ui_ -> action_blur_image -> setEnabled(true);
//    ui_ -> action_save -> setEnabled(output_is_set);
//    ui_ -> action_save_as -> setEnabled(output_is_set);
}


bool MainWindow::saveAs() {
    QString file_name = QFileDialog::getSaveFileName(this,
                                                     "Сохранить изображение",
                                                     QDir::currentPath(),
                                                     "Изображения (*.jpg *jpeg *.jpe)");
    if (true == file_name.isEmpty())
        return false;
    return save(file_name);
}


bool MainWindow::save(const QString &file_name) {
    _output_image.save(file_name, "jpeg");
    _ui -> _status_bar -> showMessage(QString("Сохранено в %1").arg(file_name), 10);
    return true;
}


QImage MainWindow::cvMatToQImage(const Mat &matrix) {
    switch (matrix.type()) {
       // 8-bit, 4 channel
       case CV_8UC4: {
          qDebug() << "rgb32";
          QImage image(matrix.data, matrix.cols, matrix.rows, matrix.step, QImage::Format_RGB32);
          return image;
       }

       // 8-bit, 3 channel
       case CV_8UC3: {
          qDebug() << "rbt8888";
          QImage image(matrix.data, matrix.cols, matrix.rows, matrix.step, QImage::Format_RGB888 );
          return image.rgbSwapped();
       }

       // 8-bit, 1 channel
       case CV_8UC1: {
          static QVector<QRgb>  color_table;
          qDebug() << "indexed8";
          // only create our color table once
          if (color_table.isEmpty()) {
             for (int i = 0; i < 256; i++)
                color_table.push_back(qRgb(i, i, i));
          }
          QImage image(matrix.data, matrix.cols, matrix.rows, matrix.step, QImage::Format_Indexed8);
          image.setColorTable(color_table);
          return image;
       }

       default:
          qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << matrix.type();
          break;
    }
    return QImage();
}


void MainWindow::updateOutputImage() {
//    Mat input, copy, input_gray, detected_edges;
    int ddepth = CV_16S;
    Mat input = imread(windowTitle().toStdString());
    if (NULL == input.data) {
        QMessageBox::critical(this, tr("Ошибка загрузки изображения"),
                              tr("Невозможно загрузить изображение из %1").arg(windowTitle()));

        return;
    }
    GaussianBlur(input, input, Size(3, 3), 0, 0, BORDER_DEFAULT);
    Mat input_gray;
    cvtColor(input, input_gray, CV_BGR2GRAY);
    Mat grad_x, grad_y;
    Mat abs_grad_x,abs_grad_y;
    Sobel(input_gray, grad_x, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);
    Sobel(input_gray, grad_y, ddepth, 0, 1, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);
    Mat result;
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, result);
//    copy.create(input.size(), input.type());
//    cvtColor(input, input_gray, CV_BGR2GRAY);
//    blur(input_gray, detected_edges, Size(3, 3));
//    Canny(detected_edges, detected_edges, _threshold, _threshold * 3, 3);
//    copy = Scalar::all(0);
//    input.copyTo(copy, detected_edges);
//    vector<int> jpeg_parametres;
//    jpeg_parametres.push_back(CV_IMWRITE_JPEG_QUALITY);
//    _output_image = cvMatToQImage(copy);
    _output_image = cvMatToQImage(result);
    QImage image = _output_image.scaled(_output_image.size() / 5);
    _ui -> _label_output -> setPixmap(QPixmap::fromImage(image));
}


void MainWindow::on__action_open_triggered() {
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     "Открыть изображение",
                                                     QDir::currentPath(),
                                                     tr("Изображения (*.jpg *.jpeg *.jpe)")
                                                     );
    if (true == file_name.isEmpty())
        return;
    else {
        setFileNameOnTittle(file_name);
        setInputImage(file_name);
        _ui -> _label_output -> clear();
        updateButtons();
    }
}


void MainWindow::on__action_save_triggered() {
    QString current_path = windowTitle();
    QString dir = QFileInfo(current_path).dir().path() + "/";
    QString suffix = QFileInfo(current_path).completeSuffix();
    save(dir + "output." + suffix);
}


void MainWindow::on__action_save_as_triggered() {
    saveAs();
}


void MainWindow::on__action_search_countours_triggered() {
    updateOutputImage();
}


void MainWindow::on__slider_threshold_valueChanged(int value) {
    _threshold = value;
    updateOutputImage();
}
