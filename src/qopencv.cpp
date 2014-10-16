#include "qopencv.h"


QImage cvMatToQImage(const Mat &matrix) {
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


Mat qimageTocvMat(const QImage &image, bool clone) {
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

