#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

IplImage* image1 = 0;
IplImage* image2 = 0;
IplImage* result = 0;

int mixImages(char *filename, char *filename2, char *output) {
    image1 = cvLoadImage(filename, 1);
    assert( image1 != 0 );

    image2 = cvLoadImage(filename2, 1);
    assert( image2 != 0 );

    // размер шаблона
    int width = image2->width;
    int height = image2->height;

    // устанавливаем область интереса
    cvSetImageROI(image1, cvRect(0, 0 , width, height));
    // взвешенная сумма
    cvAddWeighted(image1, 0.5, image2, 0.5, 0.0, result);
    // освобождаем область интереса
    cvResetImageROI(image1);

    cvSave("output.jpeg", output);

    // освобождаем ресурсы
    cvReleaseImage( &image1 );
    cvReleaseImage( &image2 );
    cvReleaseImage( &result );
    return 0;
}

int main() {
	mixImages("1.jpg", "2.jpg", "output.jpg");
}