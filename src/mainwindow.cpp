#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDebug>

#define DRAWING "drawing.jpg"
#define OUTPUT "output.jpg"


/** Конструктор класса MainWindow, наследуемый от QWidget
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow) {
    _ui -> setupUi(this);
    /* Изначально виджет параметров настроек скрыт*/
    _ui -> _dock_widget -> setHidden(true);
    /* Подключаем сигналы к слоту slotUpdateOutputImage, в котором происходит поиск контуров */
    connect(this, SIGNAL(signalInputImageWasLoaded()), this, SLOT(slotUpdateOutputImage()));
    connect(_ui -> _spin_box_threshold, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
    connect(_ui -> _spin_box_min_object_size, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
    connect(_ui -> _spin_box_max_object_size, SIGNAL(valueChanged(int)), this, SLOT(slotUpdateOutputImage()));
}

/** Деструктор класса MainWindow
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow() {
    if (true == QFile(OUTPUT).exists())
        QFile::remove(OUTPUT);
    if (true == QFile(DRAWING).exists())
        QFile::remove(DRAWING);
    delete _ui;
}


/** Устанавливает в заголовке окна имя файла, загруженный в программу
 * @brief MainWindow::setFileNameOnTittle
 */
void MainWindow::setFileNameOnTittle() {
    setWindowTitle(_file_name);
}


/** Загружает изображение из файла, отображает его в _label_input
 * @brief MainWindow::setInputImage
 * @param file_name - путь изображения
 */
void MainWindow::setInputImage(const QString &file_name) {
    _file_name = file_name;
    _input_image.load(_file_name);
    QImage image  = _input_image.scaled(_input_image.size() / 5);
    _ui -> _label_input -> setPixmap(QPixmap::fromImage(image));
    _ui -> _dock_widget -> setHidden(false);
}


/** Обновляет перечень разрешённых действий пользователю
 * @brief MainWindow::updateButtons
 */
void MainWindow::updateButtons() {
    _ui -> _action_view_parametres -> setEnabled(true);
    _ui -> _action_save -> setEnabled(true);
    _ui -> _action_save_as -> setEnabled(true);
}


/** Сохраняет выходное изображение по пути, указанным пользователем
 * @brief MainWindow::saveAs
 * @return True, если изображение сохранено
 */
bool MainWindow::saveAs() {
    QString file_name = QFileDialog::getSaveFileName(this,
                                                     "Сохранить изображение",
                                                     QDir::currentPath(),
                                                     "Изображения (*.jpg *jpeg *.jpe)");
    if (true == file_name.isEmpty())
        return false;
    return save(file_name);
}


/** Сохраняет изображение по пути
 * @brief MainWindow::save
 * @param file_name - путь для сохранения
 * @return true, если изображение сохранено
 */
bool MainWindow::save(const QString &file_name) {
    _output_image.save(file_name, "jpg");
    _ui -> _status_bar -> showMessage(QString("Сохранено в %1").arg(file_name), 10);
    return true;
}


/** Слот, в котором реализуется поиск контуров клеток
 * @brief MainWindow::slotUpdateOutputImage
 */
void MainWindow::slotUpdateOutputImage() {
    int thresh = _ui -> _spin_box_threshold -> value();
    int min_size = _ui -> _spin_box_min_object_size -> value();
    int max_size = _ui -> _spin_box_max_object_size -> value();

    Mat input = qimageTocvMat(_input_image, true);
    Mat input_gray;
    cvtColor(input, input_gray, CV_BGR2GRAY);
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    /// Detect edges using Threshold
    threshold(input_gray, threshold_output, thresh, 255, THRESH_BINARY);
    /// Find contours
    findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    /// Find the rotated rectangles and ellipses for each contour
    vector<RotatedRect> min_ellipse;

    for (unsigned i = 0; i < contours.size(); i++) {
        if ((contours[i].size() > (unsigned) min_size) && (contours[i].size() < (unsigned) max_size))
            min_ellipse.push_back(fitEllipse(Mat(contours[i])));
    }

    /// Draw ellipses
    Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
    for (unsigned i = 0; i < min_ellipse.size(); i++) {
        RotatedRect current_ellipse = min_ellipse[i];
        Scalar color = Scalar(0, 0, 255);
        ellipse(drawing, current_ellipse, color, 2, 8);
        putText(drawing, QString::number(current_ellipse.boundingRect().area()).toStdString().c_str(), current_ellipse.center, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0));

    }

    cvMatToQImage(drawing).save(DRAWING);
    mixImages(_file_name, QString(DRAWING), QString(OUTPUT));

    _output_image.load(OUTPUT);

    QImage output = _output_image.scaled(_output_image.size() / 5);
    _ui -> _label_output -> setPixmap(QPixmap::fromImage(output));
}


/** Открыть исходное изображение
 * @brief MainWindow::on__action_open_triggered
 */
void MainWindow::on__action_open_triggered() {
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     "Открыть изображение",
                                                     QDir::currentPath(),
                                                     tr("Изображения (*.jpg *.jpeg *.jpe)")
                                                     );
    if (true == file_name.isEmpty())
        return;
    else {
        setInputImage(file_name);
        setFileNameOnTittle();
        emit signalInputImageWasLoaded();
        updateButtons();
    }
}


/** Слот, обрабатывающий нажатие кнопки 'сохранить'
 * @brief MainWindow::on__action_save_triggered
 */
void MainWindow::on__action_save_triggered() {
    QString dir = QDir::currentPath() + '/';
    QString suffix = QFileInfo(_file_name).completeSuffix();
    QString base_name = QFileInfo(_file_name).baseName();
    save(dir + base_name + "_output." + suffix);

}


/** Слот, обрабатывающий нажатие кнопки 'сохранить как'
 * @brief MainWindow::on__action_save_as_triggered
 */
void MainWindow::on__action_save_as_triggered() {
    saveAs();
}


/** Слот, обрабатывающий нажатие кнопки 'показать параметры обработки'
 * @brief MainWindow::on__action_view_parametres_triggered
 */
void MainWindow::on__action_view_parametres_triggered() {
    if (false == _input_image.isNull())
        _ui -> _dock_widget -> setHidden(false);
}


/** Слот, обрабатывающий нажатие кнопки 'выход'
 * @brief MainWindow::on__action_exit_triggered
 */
void MainWindow::on__action_exit_triggered() {
    if (true == QFile(OUTPUT).exists())
        QFile::remove(OUTPUT);
    if (true == QFile(DRAWING).exists())
        QFile::remove(DRAWING);
    exit(0);
}
