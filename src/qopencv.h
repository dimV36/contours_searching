#ifndef QOPENCV_H
#define QOPENCV_H

#include <QImage>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

QImage cvMatToQImage(const Mat &matrix);
Mat qimageTocvMat(const QImage &image, bool clone);

#endif // QOPENCV_H
