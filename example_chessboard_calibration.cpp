#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
int main(int argc, char* argv[]){
  cv::String imageName("./chessboard.jpg");
  cv::Size patternsize(7,7); //interior number of corners
  cv::Mat gray = cv::imread(imageName, cv::IMREAD_GRAYSCALE); //source image
  if(gray.empty()){
    cout << "Failed to open image" << endl;
  }else{
    cout << "Opened image" << endl;
  }
  vector<cv::Point2f> corners; //this will be filled by the detected corners
  //CALIB_CB_FAST_CHECK saves a lot of time on images
  //that do not contain any chessboard corners
  bool patternfound = cv::findChessboardCorners(gray, patternsize, corners);

						//  bool patternfound = cv::findChessboardCorners(gray, patternsize, corners,
						//cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
  if(patternfound){
    cv::drawChessboardCorners(gray, patternsize, cv::Mat(corners), patternfound);
  }
  cv::imshow("image", gray);
  cv::waitKey(0);
  return 0;
}
