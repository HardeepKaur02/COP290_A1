#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <pthread.h>

using namespace std;
using namespace cv;

#define TYPE 0 // 0 => vertical splitting , 1 => horizontal splitting

// Homography matrix used from part 1
Mat h = (Mat_<double>(3,3) <<1.790891430036966, 3.444774961498641, -1614.184549699152,-0.2336338627856483, 6.143503613539651, -999.9471839473574,-0.0003106654249566934, 0.005222376441661926, 1 );

Rect crop_frame(472, 52, 328, 778); // x, y, width, height

vector<int> non_zero_pixels_queue;
vector<Ptr<BackgroundSubtractor>> pBackSub1; // vector of size = number of splits, for holding BackgroundSubtractors for each split, for queue density, i.e., having learning rate = 0

/*struct paramThread is used to pass parameter values ​​required by the thread*/
struct paramThread
{
	int id; // tells which division / split of image will be processed by thread
	Mat image; // the frame to be processed
};

void * threadProcess(void* args)
{
	paramThread *para = (paramThread *)args;
	int id = para->id;
	Mat image = para->image;

	Mat queue, queue_thresholded;
	pBackSub1.at(id)->apply(image, queue, 0); // learning rate is set to 0 so that the model doesn't change at all

	threshold(queue, queue_thresholded, 254, 255, 0); // threshold_value, max_BINARY_value, threshold_type
	non_zero_pixels_queue.at(id) = countNonZero(queue_thresholded);

	pthread_exit(NULL);
	return NULL;
}


//splitImage() achieves image segmentation, takes image, number of divisions and type as arguments and return a vector containing divisions of image
vector<Mat> splitImage(Mat image, int num, int type)
{
	int rows = image.rows;
	int cols = image.cols;
	vector<Mat> v;
	if(type == 0) {//Vertical split
		for (int i = 0; i < num; i++) {
			int start = rows/num * i;
			int end = rows/num * (i + 1);
			if (i == num - 1) {
				end = rows;
			}
			v.push_back(image.rowRange(start, end));
		}
	}
	else if(type == 1) {//Horizontal split
		for (int i = 0; i < num; i++) {
			int start = cols/num * i;
			int end = cols/num * (i + 1);
			if (i == num - 1) {
				end = cols;
			}
			/*Solve the horizontal split bug: must clone()*/
			v.push_back(image.colRange(start, end).clone());
		}
	}
	return  v;
}

//catImage() : realizes image stitching, type : 0 => vertical splitting , 1 => horizontal splitting
Mat catImage(vector<Mat> v, int type)
{
	Mat dest= v.at(0);
	for (size_t i = 1; i < v.size(); i++)
	{
		if (type == 0) // Vertical stitching
		{
			vconcat(dest, v.at(i), dest);
		}
		else if (type == 1) // Horizontal stitching
		{
			hconcat(dest, v.at(i), dest);
		}
	}
	return dest;
}

int main(int argc, char* argv[])
{
	double T0 = static_cast<double>(getTickCount());
	size_t THREAD_NUMS;

	if(argc!=3)
	{
		cout<<"Please run with video name and number of threads as command line arguments respectively\n";
		return -1;
	}

	string ext = ".mp4";
	string video_name = (string)argv[1]+ext;
	THREAD_NUMS = atoi(argv[2]);
	non_zero_pixels_queue = vector<int>(THREAD_NUMS,0);
	int total_non_zero_pixels_q;

	//open the video file for reading
	VideoCapture cap(video_name);
	//if not success, exit program
	if(cap.isOpened() == false)
	{
		cout << "Cannot open the video file\n";
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

	vector<Mat> back_img_splits = splitImage(back_img_cropped, THREAD_NUMS, TYPE);

	Mat temp;
	for(size_t i=0; i < THREAD_NUMS; i++)
	{
		pBackSub1.push_back(createBackgroundSubtractorMOG2(500, 16, true)); // history, varThreshold, detectShadows
		pBackSub1.at(i)->apply(back_img_splits[i], temp, 1); // apply empty background image with learning rate 1
	}

	int skip = 1;
	int num_frames = -1;
	ofstream density_file;
	string outfile_name = "method3_output_"+to_string(THREAD_NUMS);
	density_file.open (outfile_name);
	density_file<<"Frame number\tQueue Density\n";

	Mat frame, queue, frame_projected, frame_cropped, queue_thresholded;

	while(true)
	{
		bool bSuccess = cap.read(frame); // read a new frame from video
		num_frames++;

		// Breaking the while loop at the end of the video
		if(bSuccess == false)
		{
			cout << "Found the end of the video\n";
			break;
		}

		if(num_frames%skip==0)  // skip = 1, every frame will be processed
		{
			total_non_zero_pixels_q = 0;
			warpPerspective(frame, frame_projected, h, frame.size());
			frame_projected(crop_frame).copyTo(frame_cropped);

			vector<Mat> img_splits = splitImage(frame_cropped, THREAD_NUMS, TYPE); // 3rd parameter => type of spliting, vertical or horizontal

			/*Use multi-threaded image processing*/
			paramThread args[THREAD_NUMS];
			pthread_t pt[THREAD_NUMS]; // Create THREAD_NUMS child threads
			for(size_t i = 0; i < THREAD_NUMS; i++)
			{
				args[i].id = i;
				args[i].image = img_splits.at(i);
				pthread_create(&pt[i], NULL, &threadProcess, (void *)(&args[i]));
			}

			/*Wait for all child threads to finish processing*/
			for(size_t i = 0; i < THREAD_NUMS; i++)
			{
				pthread_join(pt[i], NULL);
				total_non_zero_pixels_q += non_zero_pixels_queue.at(i);
			}

			cout<<"Frame number: "<<num_frames<<"; queue density: "<<total_non_zero_pixels_q/(double)total_pixels<<'\n';
			density_file<<num_frames<<'\t'<<total_non_zero_pixels_q/(double)total_pixels<<'\n';

			/*uncomment this block to see the video when the code runs*/

			/*
			imshow("Projected Frame", frame_projected);
			//Mat dest = catImage(img_splits,TYPE); // When using vertical segmentation (type=0), catImage can be omitted because it is operated on the original image
			//imshow("dest", dest);
			imshow("Cropped Frame", frame_cropped);
			//wait for for 10 ms until any key is pressed.
			//If the 'Esc' key is pressed, break the while loop.
			//If the any other key is pressed, continue the loop
			//If any key is not pressed withing 10 ms, continue the loop
			if(waitKey(10) == 27)
			{
				cout << "Esc key is pressed by user. Stopping the video\n";
				break;
			}
			*/

		}
	}

	double T1 = static_cast<double>(getTickCount());
	cout<<"Method3  Run Time = "<<(T1 - T0)*1000 / getTickFrequency()<<" ms\t\t"<<"( no. of threads = "<<THREAD_NUMS<<", type = "<<TYPE<<" )\n";
	density_file<<"Method3  Run Time = "<<(T1 - T0)*1000 / getTickFrequency()<<" ms\t\t"<<"( no. of threads = "<<THREAD_NUMS<<", type = "<<TYPE<<" )\n";
	density_file.close();
	return 0;
}
