
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp> 
void findTheBall(const cv::Mat &src, cv::Point2d &startPoint, cv::Point2d &endPoint)
{
    cv::Size imgSize = src.size();
    int hmax = 0, hmin = imgSize.height, wmax = 0, wmin = imgSize.width;

    for (int i = 0; i < imgSize.height; i++)
    {
        for (int j = 0; j < imgSize.width; j++)
        {
            if (src.at<uchar>(i, j) == 255)
            {
                if (i < hmin)
                    hmin = i;
                if (i > hmax)
                    hmax = i;
                if (j < wmin)
                    wmin = j;
                if (j > wmax)
                    wmax = j;
            }
        }
    }

    startPoint.x = wmin;
    startPoint.y = hmin;
    endPoint.x = wmax;
    endPoint.y = hmax;
}

int main()
{
    cv::VideoCapture vidCap("./BouncingBall.mp4");
    cv::Mat frame, frameHSV, mask;

    cv::Point2d stPoint, enPoint;

    const int hmin = 0, smin = 50, vmin = 0;
    const int hmax = 179, smax = 255, vmax = 255;

    unsigned int i = 0;
    while (true)
    {
        vidCap.read(frame);

        cv::cvtColor(frame, frameHSV, cv::COLOR_BGR2HSV);
        cv::inRange(frameHSV, cv::Scalar(hmin, smin, vmin), cv::Scalar(hmax, smax, vmax), mask);

        findTheBall(mask, stPoint, enPoint);
        cv::rectangle(frame, stPoint, enPoint, cv::Scalar(100, 255, 0), 3);

        cv::imshow("video", frame);
        // cv::imshow("masked frame", mask);
        cv::waitKey(30);

        i++;
        if (i >= 100)
            break;
    }

    vidCap.release();
    cv::destroyAllWindows();
}