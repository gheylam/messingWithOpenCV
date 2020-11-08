#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
typedef struct cam_meta{
  int cam_number;
  cv::String cam_name; 
  cv::Mat frame;
  cv::VideoCapture cap;
}cam_meta;
    
using namespace cv;
using namespace std;

int main(int, char**)
{
  //First I will ge tthe list of all the /dev/video devices
  //And save it to a file that I can read
  
  //Check if the command process is available 
  if(!system(NULL)){
    cout << "Command process not available" << endl;
    return -1;
  }

  system("ls -ltrh /dev/video* | wc -l > cam_count.txt");

  string line;
  ifstream cam_count_file_ptr("cam_count.txt");
  if (cam_count_file_ptr.is_open()){
    getline(cam_count_file_ptr, line); 
    cam_count_file_ptr.close();
  }else{
    cout << "Could not get the cam_count file to open" << endl;
  }

  int num_cams = stoi(line);
  cout << "Number of possible cams: " << num_cams << endl;

  //After we can this value, we can remove the cam_count.txt file
  system("rm ./cam_count.txt");
  
  //vector for storing all the available cams
  vector<int> available_cams; 
  
  for(int i = 0; i < num_cams; i++){
    //for each possible cam we check if we can open them
    VideoCapture possibleCap;
    possibleCap.open(i);
    if(possibleCap.isOpened()){
      cout << "Cam number: " << i << " is a valid cam" << endl;
      available_cams.push_back(i);
    }
  }

  //After the above for loop, we will have all the available cams
  vector<cam_meta> cam_meta_list; 
  for(int cam : available_cams){
    cam_meta new_cam;
    new_cam.cam_number = cam;
    new_cam.cap.open(cam);
    ostringstream cam_name;
    cam_name << "cam_id: " << cam << endl;
    new_cam.cam_name = cam_name.str();
    cam_meta_list.push_back(new_cam);
  }

  //--- GRAB AND WRITE LOOP
  cout << "Start grabbing" << endl
       << "Press any key to terminate" << endl;
  
  while(1)
    {
      //For every available cam we read new frame into their respective cam
      for(auto cam: cam_meta_list){
	cam.cap.read(cam.frame);
	if(cam.frame.empty()){
	  cout << "Could not get frame from cam number: " << cam.cam_number << endl;
	}
	imshow(cam.cam_name, cam.frame);
      }
      int waitKeyValue = waitKey(50);
      if(waitKeyValue != 255 && waitKeyValue != -1){
	cout << waitKeyValue << endl;
	break;
      }
    }
  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
