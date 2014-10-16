#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow) {
    _ui -> setupUi(this);
    _ui -> _dock_widget -> setHidden(true);
    connect(this, SIGNAL(signalInputImageWasLoaded()), this, SLOT(slotUpdateOutputImage()));
    connect(_ui -> _spin_box_threshold, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
    connect(_ui -> _spin_box_min_object_size, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
//    connect(_ui -> _spin_box_scale, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
//    connect(_ui -> _spin_box_delta, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
//    connect(_ui -> _button_shur, SIGNAL(clicked()), this, SLOT(slotUpdateOutputImage()));
//    connect(_ui -> _button_sobel, SIGNAL(clicked()), this, SLOT(slotUpdateOutputImage()));
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
    _ui -> _dock_widget -> setHidden(false);
    emit signalInputImageWasLoaded();
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


void MainWindow::slotUpdateOutputImage() {
    int thresh = _ui -> _spin_box_threshold -> value();
    int min_size = _ui -> _spin_box_min_object_size -> value();

    Mat input = qimageTocvMat(_input_image, true);
    Mat input_gray;
    cvtColor(input, input_gray, CV_BGR2GRAY);
    blur(input_gray, input_gray, Size(3, 3));
    Mat threshold_output = Mat(input.size(), CV_8UC3);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    qDebug() << (input.size() == input_gray.size());

    /// Detect edges using Threshold
    threshold(input_gray, threshold_output, thresh, 255, THRESH_BINARY);
    /// Find contours
    findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    /// Find the rotated rectangles and ellipses for each contour
    vector<RotatedRect> min_ellipse(contours.size());

    for (unsigned i = 0; i < contours.size(); i++) {
        if (contours[i].size() > (unsigned) min_size)
            min_ellipse[i] = fitEllipse(Mat(contours[i]));
    }

    /// Draw ellipses
    Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
    for (unsigned i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(0, 0, 255);
        ellipse(drawing, min_ellipse[i], color, 2, 8);
    }

    Mat result;
    addWeighted(input, 0.5, drawing, 0.5, 0, result);
    _output_image = cvMatToQImage(result);

    QImage output = _output_image.scaled(_output_image.size() / 5);
    _ui -> _label_output -> setPixmap(QPixmap::fromImage(output));

//    IplImage input_image = input;
//    IplImage drawing_image = drawing;
//    qDebug() << (input.size() == drawing.size() && (input.channels() == drawing.channels()));
//    IplImage *result = NULL;

//    cvSetImageROI(&input_image, cvRect(0, 0, drawing_image.width, drawing_image.height));
//    AddWeighted(&input_image, 0.5, &drawing_image, 0.5, 0.0, result);

//    cvSaveImage("mixed.jpg", result);
//    cvResetImageROI(&input_image);
//    cvReleaseImage(&result);
//    IplImage *input_image = cvCreateImage(cvSize(input.cols, input.rows), IPL_DEPTH_8U, 1);
//    input_image -> imageData = (char*) input.data;

//    IplImage *drawing_image = cvCreateImage(cvSize(drawing.cols, drawing.rows), IPL_DEPTH_8U, 1);
//    drawing_image -> imageData = (char*) drawing.data;

//    IplImage *result_image = cvCloneImage(input_image);

//    cvSetImageROI(input_image, cvRect(0, 0, drawing_image -> width, drawing_image -> height));
//    cvAddWeighted(input_image, 0.5, drawing_image, 0.5, 0.0, result_image);
//    cvResetImageROI(input_image);

//    cvSave("test.jpg", result_image);
//    cvMatToQImage(Mat(result_image, true)).save("/home/dimv36/output_q.jpg");

//    cvReleaseImage(&input_image);
//    cvReleaseImage(&result_image);


//    _output_image = cvMatToQImage(output_matrix);

////    _output_image = IplImageToQImage(result_image);
//     QImage output = _output_image.scaled(_output_image.size() / 5);

//////    QImage drawing_image = cvMatToQImage(drawing);
////     output = drawing_image.scaled(drawing_image.size() / 5);
//    _ui -> _label_output -> setPixmap(QPixmap::fromImage(output));
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
