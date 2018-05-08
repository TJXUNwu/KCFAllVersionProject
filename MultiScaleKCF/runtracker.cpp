#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <io.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "kcftracker.hpp"

//#include <dirent.h>

using namespace std;
using namespace cv;
void getFiles(string foler, vector<string>& files);
int main(int argc, char* argv[])
{

	if (argc > 5) return -1;

	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool SILENT = true;
	bool LAB = false;

	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "hog") == 0)
			HOG = true;
		if (strcmp(argv[i], "fixed_window") == 0)
			FIXEDWINDOW = true;
		if (strcmp(argv[i], "singlescale") == 0)
			MULTISCALE = false;
		if (strcmp(argv[i], "show") == 0)
			SILENT = false;
		if (strcmp(argv[i], "lab") == 0){
			LAB = true;
			HOG = true;
		}
		if (strcmp(argv[i], "gray") == 0)
			HOG = false;
	}


	SILENT = true;


	// Create KCFTracker object
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

	// Frame readed
	Mat frame;

	// Tracker results
	Rect result;

	string folder = "../data/David3/img";  //此处用的是斜杠，也可以用反斜  
	vector<string> files;
	getFiles(folder, files);  //files为返回的文件名构成的字符串向量组  

	ofstream fout;
	fout.open("images.txt", ios_base::out);
	if (fout.is_open())
	{
		for (int i = 0; i < files.size(); i++)
		{    //files.size()返回文件数量  

			//To do here  
			cout << files[i] << endl;
			fout << files[i] << endl;
		}
	}
	fout.close();
	//system("pause");

	// Path to list.txt
	////ifstream listFile;
	////string fileName = "images.txt";
	////listFile.open(fileName);

	// Read groundtruth for the 1st frame
	ifstream groundtruthFile;
	string groundtruth = "region.txt";
	groundtruthFile.open(groundtruth);
	string firstLine;
	getline(groundtruthFile, firstLine);
	groundtruthFile.close();

	istringstream ss(firstLine);

	// Read groundtruth like a dumb
	float x1, y1, x2, y2, x3, y3, x4, y4;
	char ch;
	ss >> x1;
	ss >> ch;
	ss >> y1;
	ss >> ch;
	ss >> x2;
	ss >> ch;
	ss >> y2;
	ss >> ch;
	ss >> x3;
	ss >> ch;
	ss >> y3;
	ss >> ch;
	ss >> x4;
	ss >> ch;
	ss >> y4;

	// Using min and max of X and Y for groundtruth rectangle
	float xMin = min(x1, min(x2, min(x3, x4)));
	float yMin = min(y1, min(y2, min(y3, y4)));
	float width = max(x1, max(x2, max(x3, x4))) - xMin;
	float height = max(y1, max(y2, max(y3, y4))) - yMin;


	// Read Images
	ifstream listFramesFile;
	string listFrames = "images.txt";
	listFramesFile.open(listFrames);
	string frameName;


	// Write Results
	ofstream resultsFile;
	string resultsPath = "output.txt";
	resultsFile.open(resultsPath);

	// Frame counter
	int nFrames = 0;


	while (getline(listFramesFile, frameName))
	{
		frameName = frameName;

		// Read each frame from the list
		frame = imread(frameName, CV_LOAD_IMAGE_COLOR);

		// First frame, give the groundtruth to the tracker
		if (nFrames == 0) 
		{
			tracker.init(Rect(xMin, yMin, width, height), frame);
			rectangle(frame, Point(xMin, yMin), Point(xMin + width, yMin + height), Scalar(0, 255, 255), 1, 8);
			resultsFile << xMin << "," << yMin << "," << width << "," << height << endl;
		}
		// Update
		else
		{
			result = tracker.update(frame);
			rectangle(frame, Point(result.x, result.y), Point(result.x + result.width, result.y + result.height), Scalar(0, 255, 255), 1, 8);
			resultsFile << result.x << "," << result.y << "," << result.width << "," << result.height << endl;
		}

		nFrames++;

		/*if (!SILENT)
		{*/
		imshow("Image", frame);
		waitKey(1);
		/*}*/
	}
	resultsFile.close();

	//listFile.close();
	getchar();
}

void getFiles(string path, vector<string>& files)
{
	//文件句柄  
	long hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;   //大家可以去查看一下_finddata结构组成                            
	//以及_findfirst和_findnext的用法，了解后妈妈就再也不用担心我以后不会编了  
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0); _findclose(hFile);
	}
}



















