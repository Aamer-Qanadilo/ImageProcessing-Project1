#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <ctime>
#include <chrono>
#include <iostream>
#include <stack>

using namespace cv;
using namespace std;

int threshold_value = 127, brightness_value = 0;
int threshold_type = 0;
int const max_value = 255;
int const max_binary_value = 255;
Mat src_gray, dst;
const char* window_name = "Threshold Demo";
const char* trackbar_value = "Value";
stack<Mat> imgHistory;
string path;


void commandDisplay() {
	cout << "Please enter the right commands" << endl;
	cout << "o -> Open and show a color input image" << endl;
	cout << "g -> Convert the image to Gray-Scale" << endl;
	cout << "b -> Modify the brightness (increasing and decreasing) " << endl;
	cout << "c -> Improve the contrast" << endl;
	cout << "t -> Threshold" << endl;
	cout << "u -> Undo Last Change" << endl;
	cout << "p -> Preview the image" << endl;
	cout << "s -> Save the processed image" << endl;
	cout << "e -> Exit" << endl;
	cout << "\033[1;31mPress any key to close the image\033[0m" << endl;
}

void main() {
	Mat	   img, newImg;
	char   command;
	double brightnessCoe = 1;
	bool   img2Gray = false, imgGrayCommand = false, commandNotOperation = false, isOpened = false;

	system("CLS");

	while (true) {

		commandDisplay();
		cout << "Please enter the command: ";
		cin >> command;

		if (command == 'e') return;

		commandNotOperation = false;
		imgGrayCommand = false;
		if (command == 'o') {
			if (!isOpened) {
				cout << "\033[1;31mPlease Enter The Image's File Name: \033[0m";
				path = "inputImg.jpg";
				cin >> path;
				img = imread(path);
				if (img.empty()) {
					commandNotOperation = true;
					system("CLS");
					cout << "\033[1;31m********************\nWrong image path entered \n********************\033[0m" << endl;
					continue;
				}
				isOpened = true;

				imshow("Image", img);
				waitKey(0);
				destroyAllWindows();
			}
			else {
				commandNotOperation = true;
				system("CLS");
				cout << "\033[1;31m********************\nImage is already opened \n********************\033[0m" << endl;
			}
		}
		// To make sure that we opened the image correctly first
		else if (img.empty()) {
			commandNotOperation = true;
			system("CLS");
			cout << "\033[1;31m********************\nPlease Open the Image First \n********************\033[0m" << endl;
			continue;
		}
		// If the entered char 's' which indicates SAVE
		else if (command == 's') {
			time_t now = time(0);

			String output1 = "OutputImg - ";
			String date = ctime(&now);

			// These next lines is to get the image's extension

			int i;

			for (i = path.size() - 1; i >= 0; i--) {
				if (path[i] == '.') break;
			}
			String imgExt = ".";

			for (int j = i + 1; j < path.size(); j++) {
				imgExt += path[j];
			}

			date[date.size() - 1] = 't'; // To remove the '\n' from the end of the day String

			String finalOutput = output1 + date + imgExt;

			replace(finalOutput.begin(), finalOutput.end(), ':', '_'); //To replace all ':' with '_' so it would be a valid image name

			imwrite(finalOutput, img);

			commandNotOperation = true;
			system("CLS");
			cout << "\033[1;31m********************\n Image Saved Successfully! \n********************\033[0m" << endl;
		}
		// So we can preview the image
		else if (command == 'p') {
			imshow("Image", img);
			waitKey(0);
			destroyAllWindows();
		}
		// If the entered char 'u' which indicates UNDO
		else if (command == 'u') {
			if (imgHistory.size() > 1) {
				commandNotOperation = true;
				imgHistory.pop();
				imgHistory.top().copyTo(img);
				if (imgHistory.size() == 1) {
					img2Gray = false; //So if the user entered N and the previos command was gray, the img2Gray variable will be reseted to false
				}

				imshow("Image", img);
				waitKey(0);
				destroyAllWindows();
				system("CLS");
			}
			else {
				commandNotOperation = true;
				system("CLS");
				cout << "\033[1;31m********************\n There is no changes to undo.. \n********************\033[0m" << endl;
			}
		}
		// If the entered char 'g' which indicates Convert to Gray
		else if (command == 'g') {
			if (!img2Gray) {
				cvtColor(img, img, COLOR_BGR2GRAY);

				imshow("Image", img);
				waitKey(0);
				destroyAllWindows();
				img2Gray = true;
				imgGrayCommand = true;
			}
			else {
				commandNotOperation = true;
				system("CLS");
				cout << "\033[1;31m********************\n Image is already Converted to Gray-Scale \n********************\033[0m \n";
			}
		}
		// To make sure that the image isn't converted to Gray Scale before moving to other operations
		else if (!img2Gray) {
			commandNotOperation = true;
			system("CLS");
			cout << "\033[1;31m********************\n Please Convert the Image Into Gray-Scale First \n********************\033[0m" << endl;
		}
		else if (command == 'b') {
			namedWindow("Brightness Trackbar", WINDOW_AUTOSIZE); // Create a window to display results

			brightness_value = 10;

			img.copyTo(src_gray);
			createTrackbar(trackbar_value,
				"Brightness Trackbar", &brightness_value,
				20, [](int, void*) {
					float temp = 0.0;
					if (brightness_value < 10)		temp = -(brightness_value / 20.0);
					else if (brightness_value > 10)	temp = brightness_value / 20.0;
					src_gray.convertTo(dst, -1, brightness_value / 10.0 + temp, 0.0); //pixel_value_of_input_image(x, y)* brightnessCoe + 0.0;
					imshow("Brightness Trackbar", dst);
				}
			); // Create a Trackbar to choose brightness value
			waitKey(0);
			destroyAllWindows();
			dst.copyTo(img);
		}
		// If the entered char 'c' which indicates Improving the contrast
		else if (command == 'c') {
			equalizeHist(img, img);

			imshow("Image", img);
			waitKey(0);
			destroyAllWindows();
		}
		// If the entered char 't' which indicates Threshold
		else if (command == 't') {
			namedWindow(window_name, WINDOW_AUTOSIZE); // Create a window to display results

			threshold_value = 127;

			img.copyTo(src_gray);
			createTrackbar(trackbar_value,
				window_name, &threshold_value,
				max_value, [](int, void*) {
					threshold(src_gray, dst, threshold_value, max_binary_value, threshold_type);
					imshow(window_name, dst);
				}
			); // Create a Trackbar to choose Threshold value

			waitKey(0);
			destroyAllWindows();
			dst.copyTo(img);
		}
		// If the entered char is not recognized
		else {
			commandNotOperation = true;
			system("CLS");
			cout << "\033[1;31m\n\n********************\n Unknown entered character.. \n********************\033[0m" << endl;
			continue;
		}

		if (!commandNotOperation) {
			newImg = Mat();
			img.copyTo(newImg);
			imgHistory.push(newImg);
			system("CLS");
		}
	}
}