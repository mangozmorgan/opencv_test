#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>

// Fonction pour détecter les coordonnées de la balle dans l'image
void findTheBall(const cv::Mat &mask, cv::Point2d &center, cv::Rect &boundingBox)
{
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double maxArea = 0;
    int largestContourIndex = -1;

    for (size_t i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea)
        {
            maxArea = area;
            largestContourIndex = i;
        }
    }

    if (largestContourIndex != -1)
    {
        boundingBox = cv::boundingRect(contours[largestContourIndex]);
        center.x = boundingBox.x + boundingBox.width / 2.0;
        center.y = boundingBox.y + boundingBox.height / 2.0;
    }
    else
    {
        center = cv::Point2d(-1, -1);
        boundingBox = cv::Rect();
    }
}

// Fonction pour détecter les personnes dans le frame
void findPeople(const cv::Mat &frame, std::vector<cv::Rect> &people)
{
    cv::HOGDescriptor hog;
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
    hog.detectMultiScale(frame, people, 0, cv::Size(8, 8), cv::Size(16, 16), 1.05, 2);
}

int main()
{
    cv::VideoCapture vidCap("./people.mp4");
    if (!vidCap.isOpened())
    {
        std::cerr << "Erreur : Impossible de lire la vidéo." << std::endl;
        return -1;
    }

    cv::Mat frame, resizedFrame, frameHSV, mask;
    cv::Point2d ballCenter;
    cv::Rect ballBoundingBox;

    const int hmin = 0, smin = 50, vmin = 0;
    const int hmax = 30, smax = 255, vmax = 255;

    const int targetWidth = 800;
    const int targetHeight = 600;

    while (true)
    {
        vidCap.read(frame);
        if (frame.empty())
            break;

        // Redimensionnement de l'image
        double scaleX = (double)targetWidth / frame.cols;
        double scaleY = (double)targetHeight / frame.rows;
        cv::resize(frame, resizedFrame, cv::Size(targetWidth, targetHeight));

        // Détection des personnes
        std::vector<cv::Rect> people;
        findPeople(resizedFrame, people);
        for (const auto &person : people)
        {
            cv::rectangle(resizedFrame, person, cv::Scalar(255, 0, 0), 2);
        }

        // Conversion en HSV et création du masque
        cv::cvtColor(resizedFrame, frameHSV, cv::COLOR_BGR2HSV);
        cv::inRange(frameHSV, cv::Scalar(hmin, smin, vmin), cv::Scalar(hmax, smax, vmax), mask);

        // Nettoyage du masque pour réduire le bruit
        cv::erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);
        cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);

        // Détection de la balle
        // findTheBall(mask, ballCenter, ballBoundingBox);

        // Dessin des résultats de la détection de la balle
        // if (ballCenter.x != -1 && ballCenter.y != -1)
        // {
        //     cv::rectangle(resizedFrame, ballBoundingBox, cv::Scalar(0, 255, 0), 2);
        //     cv::circle(resizedFrame, ballCenter, 5, cv::Scalar(0, 0, 255), -1);
        // }

        // Affichage
        cv::imshow("video", resizedFrame);
        // cv::imshow("mask", mask);

        if (cv::waitKey(30) >= 0)
            break;
    }

    vidCap.release();
    cv::destroyAllWindows();
    return 0;
}
