#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow) {
    _ui -> setupUi(this);
}


MainWindow::~MainWindow() {
    delete _ui;
}


void MainWindow::setFileNameOnTittle(const QString &file_name) {
    setWindowTitle(file_name);
}


void MainWindow::setInputImage(const QString &file_name) {
    _input_image.load(file_name);
    QImage image  = _input_image.scaled(_input_image.size() / 1.5);
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
