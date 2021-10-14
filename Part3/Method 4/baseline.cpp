#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
  bool imshow_on = false;
  double T0 = static_cast<double>(cv::getTickCount());
  if(argc < 2 || argc > 3)
    {
      cout<<"Please run with video name and (optional) imshow mode as command line arguments\n";
      return -1;
    }
  if(argc == 3 and atoi(argv[2]) == 1){
    imshow_on = true;
  }

  string ext = ".mp4";
  string video_name = (string)argv[1]+ext;

  //open the video file for reading
  VideoCapture cap(video_name);

  //if not success, exit program
  if (cap.isOpened() == false)
  {
    cout << "Cannot open the video file" << endl;
    cin.get(); //wait for any key press
    return -1;
  }

  double fps = cap.get(CAP_PROP_FPS);
  cout << "Frames per second : " << fps << endl;

  //Homography matrix used from part 1
  Mat h = (Mat_<double>(3,3) <<1.790891430036966, 3.444774961498641, -1614.184549699152,-0.2336338627856483, 6.143503613539651, -999.9471839473574,-0.0003106654249566934, 0.005222376441661926, 1 );

  Mat back_img, back_img_proj, back_img_cropped;
  back_img = imread("empty_new.png");
  if (back_img.empty())
  {
    cout<<"The empty background image \"empty_new.png\" not found in the same directory"<<endl;
    return -1;
  }
  warpPerspective(back_img, back_img_proj, h, back_img.size() );
  Rect crop_frame(472, 52, 328, 778); // x, y, width, height
  back_img_proj(crop_frame).copyTo(back_img_cropped);
  int total_pixels = back_img_cropped.total();
  cout<<"Total No. of Pixels in Cropped Image :"<<total_pixels<<'\n';

  Ptr<BackgroundSubtractor> pBackSub1;
  pBackSub1 = createBackgroundSubtractorMOG2(500, 16, true); // history, varThreshold, detectShadows

  Mat temp;

  pBackSub1->apply(back_img_cropped, temp, 1); // apply empty background image with learning rate 1

  int skip = 1 ;// process every frame
  long long num_frames = -1;

  ofstream density_file;
  density_file.open ("baseline_output.txt");
  density_file<<"Frame number\tQueue Density\n";

  Mat frame, queue, frame_projected, frame_cropped, queue_thresholded;

  while (true)
  {
    bool bSuccess = cap.read(frame); // read a new frame from video
    num_frames++;

    //Breaking the while loop at the end of the video
    if (bSuccess == false)
	  {
	     cout << "Found the end of the video" << endl;
	      break;
	  }

    if(num_frames%skip==0)
    {
      warpPerspective(frame, frame_projected, h, frame.size());
      frame_projected(crop_frame).copyTo(frame_cropped);

      //absdiff(frame_cropped, back_img_cropped, queue);
      pBackSub1->apply(frame_cropped, queue, 0); // learning rate is set to 0 so that the model doesn't change at all
      threshold(queue, queue_thresholded, 254, 255, 0); //threshold_value, max_BINARY_value, threshold_type

      int non_zero_pixels_queue = countNonZero(queue_thresholded);
      cout<<"Frame number: "<<num_frames<<"; queue density: "<<non_zero_pixels_queue/(double)total_pixels<<'\n';
      density_file<<num_frames<<'\t'<<non_zero_pixels_queue/(double)total_pixels<<'\n';
      if(imshow_on){
        imshow("Projected Frame", frame_projected);
        imshow( "Queue", queue_thresholded);
        //wait for for 10 ms until any key is pressed.
        //If the 'Esc' key is pressed, break the while loop.
        //If the any other key is pressed, continue the loop
        //If any key is not pressed withing 10 ms, continue the loop
        if (waitKey(10) == 27)
        {
          cout << "Esc key is pressed by user. Stopping the video" << endl;
          break;
        }
      }
    }
  }
  double T1 = static_cast<double>(getTickCount());
  cout<<"Baseline Run Time = "<<(T1 - T0)*1000 / getTickFrequency()<<" ms\t"<<"( process each frame for queue density )\n";
  density_file<<"Baseline Run Time = "<<(T1 - T0)*1000 / getTickFrequency()<<" ms\n";
  density_file.close();
  return 0;
}
