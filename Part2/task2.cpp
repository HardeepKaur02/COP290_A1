#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <iostream>
#include <fstream>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
  if(argc!=2)
    {
      cout<<"Please run with video name as command line argument\n";
      return -1;		
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
  Ptr<BackgroundSubtractor> pBackSub2;
  pBackSub1 = createBackgroundSubtractorMOG2(500, 16, true); // history, varThreshold, detectShadows
  pBackSub2 = createBackgroundSubtractorMOG2(500, 16, true);
  
  Mat temp;
  
  pBackSub1->apply(back_img_cropped, temp, 1); // apply empty background image with learning rate 1
  pBackSub2->apply(back_img_cropped, temp, 1);

  int skip = 3 ;// process every 3rd frame to obtain 5 fps
  long long num_frames = 0;
 
  ofstream density_file;
  density_file.open ("out.txt");
  density_file<<"# Seconds    Queue Density    Dynamic Density\n";
 
  Mat frame, dynamic, queue, frame_projected, frame_cropped, queue_thresholded, dynamic_thresholded;

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
	pBackSub2->apply(frame_cropped, dynamic, 0.01); 
   
	imshow("Projected Frame", frame_projected);
     
	threshold(queue, queue_thresholded, 254, 255, 0); //threshold_value, max_BINARY_value, threshold_type
	threshold(dynamic, dynamic_thresholded, 245, 255, 0); 
	imshow( "Queue", queue_thresholded);
	imshow( "Dynamic", dynamic_thresholded);
     
	int non_zero_pixels_queue = countNonZero(queue_thresholded);
	int non_zero_pixels_dynamic = countNonZero(dynamic_thresholded);
	cout<<"framenum: "<<num_frames<<"; queue density: "<<non_zero_pixels_queue/(double)total_pixels<<"; dynamic density: "<<non_zero_pixels_dynamic/(double)total_pixels<<'\n';
	density_file<<num_frames/double(15.0129)<<'\t'<<non_zero_pixels_queue/(double)total_pixels<<'\t'<<non_zero_pixels_dynamic/(double)total_pixels<<'\n';

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
  
  density_file.close();
  return 0;
}
