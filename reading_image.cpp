#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;

int main ( int argc, char **argv )
{
  std::cout << CV_MAJOR_VERSION << std::endl;
  std::cout << CV_MINOR_VERSION << std::endl;
  cv::String imageName("./images/orangepeel.png");
  if(argc > 1)
    {
      imageName = argv[1];
    }

  cv::Mat image;

  image = cv::imread(imageName, cv::IMREAD_COLOR); //read the file
  if(image.empty())
    {
      std::cout << "Could not open or find the image" << std::endl;
      return -1;
    }

  cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE); //create a window for display
  cv::imshow("Display window", image);
  cv::waitKey(0); //Wait for a keystroke in the window
  return 0;
}
      
