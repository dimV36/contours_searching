#ifndef QOPENCV_H
#define QOPENCV_H

#include <QImage>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

QImage cvMatToQImage(const Mat &matrix);
Mat qimageTocvMat(const QImage &image, bool clone);

IplImage cvMatToIplImage(const Mat &matrix);
Mat iplImageToCvMat(const IplImage &image);

void mixImages(const QString image_path1, const QString image_path2, const QString output_path);

#endif // QOPENCV_H
