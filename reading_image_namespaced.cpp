#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main ( int argc, char **argv )
{
  String imageName("./images/orangepeel.png");
  if(argc > 1)
    {
      imageName = argv[1];
    }

  Mat image;


  image = imread(imageName, IMREAD_COLOR); //read the file
  if(image.empty())
    {
      cout << "Could not open or find the image" << endl;
      return -1;
    }

  namedWindow("Display window", WINDOW_AUTOSIZE); //create a window for display
  imshow("Display window", image);
  waitKey(0); //Wait for a keystroke in the window
  return 0;
}
      
