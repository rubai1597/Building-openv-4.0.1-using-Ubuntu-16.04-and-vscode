#include <iostream>
#include <opencv2/opencv.hpp>
 
int main()
{
    std::cout << "OpenCV Version : " << CV_VERSION << std::endl;
    cv::Mat img;
 
    img = cv::imread("cat.jpg");
    if (img.empty())
    {
        std::cout << "[!] You can NOT see the cat!" << std::endl;
        return -1;
    }
    cv::imshow("EXAMPLE01", img);
    cv::waitKey(0);
    cv::destroyWindow("EXAMPLE01");
    return 0;
}
