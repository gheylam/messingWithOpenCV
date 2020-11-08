#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

void help(char *argv[]){
  cout << "You should get help" << endl;
}

int main(int argc, char* argv[]){
  int n_boards = 0; //The number of boards set by input list
  float image_sf = 0.5f;
  float delay = 1.f;
  int board_w = 0;
  int board_h = 0;

  if(argc < 4 | argc > 6){
    cout << "Error: Wrong number of input parameters\n" << endl;
    help(argv);
    return -1;
  }

  board_w = atoi(argv[1]);
  board_h = atoi(argv[2]);
  n_boards = atoi(argv[3]);
  if(argc > 4) delay = atof(argv[4]); //converts string to double
  if(argc > 5) image_sf = atof(argv[5]);

  int board_n = board_w * board_h;
  cv::Size board_sz = cv::Size(board_w, board_h);

  cv::VideoCapture capture(0);
  if(!capture.isOpened()){
    cout << "Couldn't open the camera" << endl;
    help(argv);
    return -1;
  }

  //Allocate Storage
  vector<vector<cv::Point2f>> image_points;
  vector<vector<cv::Point3f>> object_points;

  //Capture corner views: loop until we've got n_boards successful
  //captures (all covered on the board are found).

  double last_captured_timestamp = 0;
  cv::Size image_size;

  while(image_points.size() < (size_t)n_boards){
    cv::Mat image0, image;
    capture >> image0;
    //cv::cvtColor(image0, image0, cv::COLOR_RGB2GRAY);
    image_size = image0.size();
    cv::resize(image0, image, cv::Size(), image_sf, image_sf,
	       cv::INTER_LINEAR);
    //Find the board
    vector<cv::Point2f> corners;
    bool found = cv::findChessboardCorners(image, board_sz, corners, cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
    cout << "found: " << found << endl;
    //Draw it
    drawChessboardCorners(image, board_sz, corners, found);

    //If we got good board, add it to our data
    double timestamp = (double)clock()/CLOCKS_PER_SEC; //clock returns the number of clock ticks consumed by our program

    if(found && timestamp - last_captured_timestamp > 1) {
      last_captured_timestamp = timestamp;
      image ^= cv::Scalar::all(255);

      cv::Mat mcorners(corners);   // Do not copy the data
      mcorners *= (1./image_sf);   // Scale the corner coordinates.
      image_points.push_back(corners);
      object_points.push_back(vector<cv::Point3f>());
    
      vector<cv::Point3f> & opts = object_points.back();
      opts.resize(board_n);
      for(int j = 0; j < board_n; j++){
	opts[j] = cv::Point3f((float)(j/board_w), (float)(j%board_w), 0.f);
      }
      cout << "Collected our " << (int)image_points.size() <<
	" of " << n_boards << " needed chessboard images\n" << endl;
    }
    cv::imshow("Calibration", image); //show in color if we did collect the image
    if((cv::waitKey(30) & 255) == 27)
      return -1;
  }
  //end collection while loop
  
  cv::destroyWindow("Calibration");
  cout << "\n\n*** CALIBRATING THE CAMERA...\n" << endl;

  //Calibrating the camera!
  cv::Mat intrinsic_matrix, distortion_coeffs;
  double err = cv::calibrateCamera(
				   object_points,
				   image_points,
				   image_size,
				   intrinsic_matrix,
				   distortion_coeffs,
				   cv::noArray(),
				   cv::noArray(),
				   cv::CALIB_ZERO_TANGENT_DIST
				   );

  //saving the intrinsics and distortions
  cout << "***DONE!\n\nReprojection Error is " << err <<
    "\nStoring Intrinsics.xml and Distortions.xml files\n\n";
  cv::FileStorage fs("intrinsics.xml", cv::FileStorage::WRITE);

  fs << "image_width" << image_size.width << "image_height" << image_size.height
     << "camera_matrix" << intrinsic_matrix << "distortion_coefficients" << distortion_coeffs;
  fs.release();

  //Loading the intrinsics back in
  fs.open("intrinsics.xml", cv::FileStorage::READ);
  cout << "\nimage width: " << (int)fs["image_width"];
  cout << "\nimage height: " << (int)fs["image_height"];

  cv::Mat intrinsic_matrix_loaded, distortion_coeffs_loaded;
  fs["camera_matrix"] >> intrinsic_matrix_loaded;
  fs["distortion_coefficients"] >> distortion_coeffs_loaded;
  cout << "\nintrinsics matrix: " << intrinsic_matrix_loaded;
  cout << "\ndistortion coefficients: " << distortion_coeffs_loaded << endl;

  //Build the undistort map which we will use for all
  //subsequent frames

  cv::Mat map1, map2;
  cv::initUndistortRectifyMap(
			      intrinsic_matrix_loaded,
			      distortion_coeffs_loaded,
			      cv::Mat(),
			      intrinsic_matrix_loaded,
			      image_size,
			      CV_16SC2,
			      map1,
			      map2
			      );
  //Jusst run the camera to the screen, now showing the raw and the
  //undistorted image
  for(;;){
    cv::Mat image, image0;
    capture >> image0;
    if(image0.empty()){
      cout << "Error reading images from webcam" << endl;
      break;
    }
    cv::remap(
	      image0,
	      image,
	      map1,
	      map2,
	      cv::INTER_LINEAR,
	      cv::BORDER_CONSTANT,
	      cv::Scalar()
	      );
    cv::imshow("Undistorted", image);
    if((cv::waitKey(30) && 255) == 27) break;
  }
  return 0;
}
  
