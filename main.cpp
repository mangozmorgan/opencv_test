#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp> 

// Fonction pour détecter les coordonnées de la balle dans l'image
void findTheBall(const cv::Mat &src, cv::Point2d &startPoint, cv::Point2d &endPoint)
{
    // Récupère la taille de l'image
    cv::Size imgSize = src.size();
    int hmax = 0, hmin = imgSize.height, wmax = 0, wmin = imgSize.width;

    // Parcourt chaque pixel de l'image
    for (int i = 0; i < imgSize.height; i++)
    {
        for (int j = 0; j < imgSize.width; j++)
        {
            // Si le pixel est blanc (valeur 255), cela signifie que c'est une partie de la balle
            if (src.at<uchar>(i, j) == 255)
            {
                // Met à jour les coordonnées du rectangle englobant
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

    // Assigne les coordonnées du rectangle à startPoint et endPoint
    startPoint.x = wmin;
    startPoint.y = hmin;
    endPoint.x = wmax;
    endPoint.y = hmax;
}

int main()
{
    // Ouverture du fichier vidéo
    cv::VideoCapture vidCap("./BouncingBall.mp4");
    cv::Mat frame, frameHSV, mask;

    // Points de départ et d'arrivée du rectangle englobant la balle
    cv::Point2d stPoint, enPoint;

    // Plage des valeurs HSV pour la détection de la balle
    const int hmin = 0, smin = 50, vmin = 0;
    const int hmax = 179, smax = 255, vmax = 255;

    unsigned int i = 0;
    // Boucle pour lire et traiter chaque image de la vidéo
    while (true)
    {
        vidCap.read(frame); // Lecture d'une nouvelle image

        // Conversion de l'image de BGR en HSV
        cv::cvtColor(frame, frameHSV, cv::COLOR_BGR2HSV);
        
        // Création d'un masque avec les valeurs HSV définies
        cv::inRange(frameHSV, cv::Scalar(hmin, smin, vmin), cv::Scalar(hmax, smax, vmax), mask);

        // Détection de la balle en fonction du masque
        findTheBall(mask, stPoint, enPoint);

        // Dessin du rectangle autour de la balle détectée
        cv::rectangle(frame, stPoint, enPoint, cv::Scalar(100, 255, 0), 3);

        // Affichage du frame avec le rectangle
        cv::imshow("video", frame);
        cv::waitKey(30); // Attente de 30ms pour l'affichage de l'image

        i++;
        // Limitation du nombre d'images à traiter
        if (i >= 100)
            break;
    }

    // Libération de la vidéo et fermeture des fenêtres
    vidCap.release();
    cv::destroyAllWindows();
}
