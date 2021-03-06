#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void right_click_function(int event, int x, int y, int flags, void* param)
{	
	if(event==EVENT_RBUTTONDOWN)
	{
		cout<<"Source Image: right-click on: ("<<x<<", "<<y<<")\n";
		vector<Point2f>* ptPtr = (vector<Point2f>*)param;
        ptPtr->push_back(Point2f(x,y));
	}
}

int main(int argc, char* argv[])
{
	if(argc!=2)
	{
		cout<<"Please run with one command line argument,i.e, the image name ( either empty or traffic)\n";
		return -1;		
	}
 
  	string ext = ".jpg";
  	string image_filename = (string)argv[1]+ext;       
	
  	Mat im_src = imread(image_filename, IMREAD_GRAYSCALE);
	
	if(im_src.empty()) 
	{ 
		cout<<"Error loading Source Image\n";
		cout<<"Check if the image is present in same directory or not\n";
		return -1; 
	}
	
	cout<<"Source Image "<<image_filename<<" loaded\n";
	cout<<"Source Image size is "<<im_src.size()<<'\n';
	
	cout<<"Right-click 4 points starting from top left corner in anticlockwise-direction\n";
	
	vector<Point2f> pts_src;
	namedWindow("Source Image", WINDOW_AUTOSIZE);	
	setMouseCallback("Source Image", right_click_function, (void*)&pts_src);
	imshow("Source Image", im_src);	
	cout<<"Press any key to continue after 4 points have been printed\n";
	waitKey(0);	
	
	if (pts_src.size()==4)
	{	
		vector<Point2f> pts_dst;
		pts_dst.push_back(Point2f(472,52));
		pts_dst.push_back(Point2f(472,830));
		pts_dst.push_back(Point2f(800,830));
		pts_dst.push_back(Point2f(800,52));
		
		cout<<"Projected Image points that are mapped to the selected points are: ";		
		for(int i=0; i<4; i++)
		{
			cout<<"("<<pts_dst.at(i).x<<", "<<pts_dst.at(i).y<<") ";
		}
		cout<<'\n';
	
		Mat h = findHomography(pts_src, pts_dst);
	
		Mat im_projected;
		warpPerspective(im_src, im_projected, h, im_src.size());
		
		cout<<"Projected Image generated\n";	
		namedWindow("Projected Image", WINDOW_AUTOSIZE);	
		imshow("Projected Image", im_projected);		
		cout<<"Press any key to continue\n";
		waitKey(0);
		
		string pjt = "_projected";
		string projected_filename = (string)argv[1]+pjt+ext;		
		imwrite(projected_filename, im_projected);
		cout<<"Projected Image written to "<<projected_filename<<'\n';
		
		cout<<"Cropping using the mapped points\n";
		
		Rect crop_frame(472, 52, 328, 778); // x, y, width, height
		Mat im_cropped;
		im_projected(crop_frame).copyTo(im_cropped);
		cout<<"Cropped Image generated\n";	
		namedWindow("Cropped Image", WINDOW_AUTOSIZE);	
		imshow("Cropped Image", im_cropped);
		cout<<"Press any key to continue\n";
		waitKey(0);
		
		string crp = "_cropped";	
		string cropped_filename = (string)argv[1]+crp+ext;	
		imwrite(cropped_filename, im_cropped);
		cout<<"Cropped Image written to "<<cropped_filename<<'\n';
	}
	else{
	  cout<<"Please click on exactly 4 points"<<'\n';
	}
	return 0;
}
