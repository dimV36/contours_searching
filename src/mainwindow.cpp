#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow) {
    _ui -> setupUi(this);
    _ui -> _dock_widget -> setHidden(true);
    connect(_ui -> _spin_box_scale, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
    connect(_ui -> _spin_box_delta, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
    connect(_ui -> _button_shur, SIGNAL(clicked()), this, SLOT(slotUpdateOutputImage()));
    connect(_ui -> _button_sobel, SIGNAL(clicked()), this, SLOT(slotUpdateOutputImage()));
    connect(this, SIGNAL(signalInputImageWasLoaded()), this, SLOT(slotUpdateOutputImage()));
}


MainWindow::~MainWindow() {
    delete _ui;
}


void MainWindow::setFileNameOnTittle(const QString &file_name) {
    setWindowTitle(file_name);
}


void MainWindow::setInputImage(const QString &file_name) {
    _input_image.load(file_name);
    QImage image  = _input_image.scaled(_input_image.size() / 5);
    _ui -> _label_input -> setPixmap(QPixmap::fromImage(image));
    emit signalInputImageWasLoaded();
    _ui -> _dock_widget -> setHidden(false);
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
          QImage image(matrix.data, matrix.cols, matrix.rows, matrix.step, QImage::Format_RGB32);
          return image;
       }

       // 8-bit, 3 channel
       case CV_8UC3: {
          QImage image(matrix.data, matrix.cols, matrix.rows, matrix.step, QImage::Format_RGB888 );
          return image.rgbSwapped();
       }

       // 8-bit, 1 channel
       case CV_8UC1: {
          static QVector<QRgb>  color_table;
          if (color_table.isEmpty()) {
             for (int i = 0; i < 256; i++)
                color_table.push_back(qRgb(i, i, i));
          }
          QImage image(matrix.data, matrix.cols, matrix.rows, matrix.step, QImage::Format_Indexed8);
          image.setColorTable(color_table);
          return image;
       }

       default:
          break;
    }
    return QImage();
}


Mat MainWindow::qimageTocvMat(const QImage &image, bool clone) {
    switch (image.format()) {
             // 8-bit, 4 channel
             case QImage::Format_RGB32: {
                Mat matrix(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
                return (clone ? matrix.clone() : matrix);
             }

             // 8-bit, 3 channel
             case QImage::Format_RGB888: {
                QImage swapped = image.rgbSwapped();
                return Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();
             }

             // 8-bit, 1 channel
             case QImage::Format_Indexed8: {
                Mat matrix(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine());
                return (clone ? matrix.clone() : matrix);
             }

             default:
                break;
          }
          return Mat();
}


void MainWindow::slotUpdateOutputImage() {
    int scale = _ui -> _spin_box_scale -> value();
    int delta = _ui -> _spin_box_delta -> value();
    int ddepth = CV_16S;
    Mat input = qimageTocvMat(_input_image, true);
    if (NULL == input.data) {
        QMessageBox::critical(this, tr("Ошибка загрузки изображения"),
                              tr("Невозможно загрузить изображение из %1").arg(windowTitle()));

        return;
    }
    GaussianBlur(input, input, Size(3, 3), 0, 0, BORDER_DEFAULT);
    Mat input_gray;
    cvtColor(input, input_gray, CV_RGB2GRAY);
    Mat grad_x, grad_y;
    Mat abs_grad_x,abs_grad_y;
    if (_ui -> _button_shur -> isChecked()) {
        Scharr(input_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT);
        Scharr( input_gray, grad_y, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    }
    if (_ui -> _button_sobel -> isChecked()) {
        Sobel(input_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
        Sobel(input_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    }
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    Mat result;
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, result);
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
//    updateOutputImage();
}




void MainWindow::on__action_view_parametres_triggered() {
    if (false == _input_image.isNull())
        _ui -> _dock_widget -> setHidden(false);
}
