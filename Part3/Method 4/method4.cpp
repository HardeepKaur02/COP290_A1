#include <iostream>
#include <fstream>
#include <pthread.h>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


// Homography matrix used from part 1
Mat h = (Mat_<double>(3,3) <<1.790891430036966, 3.444774961498641, -1614.184549699152,-0.2336338627856483, 6.143503613539651, -999.9471839473574,-0.0003106654249566934, 0.005222376441661926, 1 );

Rect crop_frame(472, 52, 328, 778); // x, y, width, height

vector<Ptr<BackgroundSubtractor>> pBackSub_Vector;
vector<int> non_zero_pixels_queue_Vector;

struct paramThread
{
	int id; // determines which frame number will be processed by thread
	Mat image; // the frame to be processed
};

void * threadProcess(void* args)
{
	paramThread *para = (paramThread *)args;
	int id = para->id;
	Mat frame_cropped = para->image;

	Mat queue, queue_thresholded;
	pBackSub_Vector.at(id)->apply(frame_cropped, queue, 0); // learning rate is set to 0 so that the model doesn't change at all
	threshold(queue, queue_thresholded, 254, 255, 0); // threshold_value, max_BINARY_value, threshold_typ
	non_zero_pixels_queue_Vector.at(id) = countNonZero(queue_thresholded);

	pthread_exit(NULL);
	return NULL;
}

int main(int argc, char* argv[])
{
	double T0 = static_cast<double>(getTickCount());
	size_t THREAD_NUMS;

	if(argc!=3)
	{
		cout<<"Please run with video name and number of threads as command line argument\n";
		return -1;
	}

	string ext = ".mp4";
	string video_name = (string)argv[1] + ext;
	THREAD_NUMS = atoi(argv[2]);
	//open the video file for reading
	VideoCapture cap(video_name);
	//if not success, exit program
	if(cap.isOpened() == false)
	{
		cout<<"Cannot open the video file\n";
		cin.get(); //wait for any key press
		return -1;
	}

	double fps = cap.get(CAP_PROP_FPS);
	cout << "Frames per second : " << fps << '\n';

	Mat back_img, back_img_proj, back_img_cropped;
	back_img = imread("empty_new.png");
	if(back_img.empty())
	{
		cout<<"The empty background image \"empty_new.png\" not found in the same directory\n";
		return -1;
	}

	warpPerspective(back_img, back_img_proj, h, back_img.size());
	back_img_proj(crop_frame).copyTo(back_img_cropped);

	int total_pixels = back_img_cropped.total();
	cout<<"Total No. of Pixels in Cropped Image :"<<total_pixels<<'\n';

	Mat temp;
	for(int i=0; i< THREAD_NUMS; i++)
	{
		pBackSub_Vector.push_back(createBackgroundSubtractorMOG2(500, 16, true)); // history, varThreshold, detectShadows
		pBackSub_Vector.at(i)->apply(back_img_cropped, temp, 1); // apply empty background image with learning rate 1
	}

	int skip = 1;// process every frame
	int frame_num = -1;
	ofstream density_file;
	string outfile_name = "method4_output_"+to_string(THREAD_NUMS);
	density_file.open(outfile_name);
	density_file<<"Frame number\tQueue Density\n";

	Mat frame;
	vector<int> frame_nums;
	vector<Mat> frames;
	non_zero_pixels_queue_Vector = vector<int>(THREAD_NUMS,0);
	bool bSuccess = cap.read(frame); // read a new frame from video
	frame_num++;

	while(true)
	{
		// Breaking the while loop at the end of the video
		if(bSuccess == false)
		{
			cout<<"Found the end of the video\n";
			break;
		}

		if(frame_num%skip==0)
		{
			frame_nums.clear();
			frames.clear();
			for(size_t i = 0; i<THREAD_NUMS; i++)
			{
				frame_nums.push_back(frame_num);

				Mat frame_projected, frame_cropped;
				warpPerspective(frame, frame_projected, h, frame.size());
				frame_projected(crop_frame).copyTo(frame_cropped);

				frames.push_back(frame_cropped);

				for(int i=0; i<skip; i++)
				{
					bSuccess = cap.read(frame); // read a new frame from video
					frame_num++;
					// Breaking the while loop at the end of the video
					if(bSuccess == false)
					{
						//cout<<"Found the end of the video\n"; // this line is not needed as we only want to print this line once
						break;
					}
				}
				if(bSuccess == false)
				{
					break;
				}
			}
			/*Use multi-threaded image processing*/
			paramThread args[THREAD_NUMS];
			pthread_t pt[THREAD_NUMS]; // Create THREAD_NUMS child threads
			for(size_t i = 0; i < frame_nums.size(); i++)
			{
				args[i].id = i;
				args[i].image = frames.at(i);
				pthread_create(&pt[i], NULL, &threadProcess, (void *)(&args[i]));
			}
			/*Wait for all child threads to finish processing*/
			for(size_t i = 0; i < frame_nums.size(); i++)
			{
				pthread_join(pt[i], NULL);
			}
			/*Print and write queue densities*/
			for (size_t i = 0; i < frame_nums.size(); i++)
			{
				cout<<"framenum: "<<frame_nums.at(i)<<"; queue density: "<<(non_zero_pixels_queue_Vector.at(i))/(double)total_pixels<<'\n';
				density_file<<frame_nums.at(i)<<'\t'<<(non_zero_pixels_queue_Vector.at(i))/(double)total_pixels<<'\n';
			}
		}
	}

	double T1 = static_cast<double>(cv::getTickCount());
	cout<<"Method4  Run Time = "<<(T1 - T0)*1000 / getTickFrequency()<<" ms\t\t"<<"( no. of threads = "<<THREAD_NUMS<<" )\n";
	density_file<<"Method4  Run Time = "<<(T1 - T0)*1000 / getTickFrequency()<<" ms\t\t"<<"( no. of threads = "<<THREAD_NUMS<<" )\n";
	density_file.close();
	return 0;
}
