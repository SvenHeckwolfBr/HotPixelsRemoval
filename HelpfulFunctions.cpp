// HelpfulFunctions.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//


#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>



/** show an image (cv::Mat) on a canvas, click on the image to close the canvas
*/
void showMatrix(cv::Mat mat);

/** print all the elements of a matrix (numbers) to the terminal
*/
void printMatrix(char* pMessage, cv::Mat& mat);

/** print thze type of a matrix to the terminal
*/
std::string openCVType2str(int type);

int main()
{
    // Parameters used:
    int fieldWidth = 10;
    int fieldHeight = 10;
    // Create a matrix filled with noise
    cv::Mat randomNoise = cv::Mat(fieldHeight, fieldWidth, CV_16UC1);
    cv::theRNG().fill(randomNoise, cv::RNG::UNIFORM, 0, 65535 );
    // Create a chess board matrix with 3x3 pixels per field "per pedes"
    cv::Mat whiteField = cv::Mat(cv::Size(fieldWidth, fieldHeight), CV_8UC1, cv::Scalar(255));
    //cv::Mat whiteField = cv::Mat::ones(cv::Size(fieldWidth, fieldHeight), CV_8UC1);
    cv::Mat blackField = cv::Mat::zeros(cv::Size(fieldWidth, fieldHeight), CV_8UC1);
    cv::Mat chessboard, blackStartStrip, whiteStartStrip;
    cv::hconcat(blackField, whiteField, blackStartStrip);
    cv::hconcat(whiteField, blackField, whiteStartStrip);
    do {
        cv::hconcat(blackStartStrip, blackStartStrip, blackStartStrip);
        cv::hconcat(whiteStartStrip, whiteStartStrip, whiteStartStrip);
        showMatrix(blackStartStrip);
    } while (blackStartStrip.size().width < (8*10) );
    cv::vconcat(blackStartStrip, whiteStartStrip, chessboard);
    do {
        cv::vconcat(chessboard, chessboard, chessboard);
        showMatrix(chessboard);
    } while (chessboard.size().height < (8 * 10) );

    // test showMatrix
    showMatrix(chessboard);
    showMatrix(randomNoise); 

    // test printMatrix
    char typeRef[13] = {"random Noise"};
    printMatrix(typeRef, randomNoise);

    // test openCVType2str
    std::string matType = openCVType2str(randomNoise.type());
    printf("Matrix: %s %dx%d \n", matType.c_str(), randomNoise.cols, randomNoise.rows);
    // x --> cols --> mat.size().width;
    // y --> rows --> mat.size().height;
	
	//Additional information:
	
	// ATTENTION: getting the intensity of a matrix element (the pixel value) normally works like this (for a cv::Mat of type CV_8U):
	// cv::Scalar intensity = img.at<uchar>(y, x);
	//BUT (and only in OpenCV in C++) it is possible to use:
	// cv::Scalar intensity = img.at<uchar>(cv::Point(x, y));
	// Example:
	int y = 1, x = 0;
	// 8 bits
	cv::Mat colorNoise8 = cv::Mat(2, 2, CV_8UC3);
    cv::theRNG().fill(colorNoise8, cv::RNG::UNIFORM, 0, 255 );
	Vec3b intensity = colorNoise8.at<Vec3b>(y, x);
	uchar blue8 = colorNoise8.val[0];
	uchar green8 = colorNoise8.val[1];
	uchar red8 = colorNoise8.val[2];
	std::cout << " blue: " << blue8 << " , green: " << green8 << " , red: " << red8 << std::endl;
	// 16 bits
	cv::Mat colorNoise16 = cv::Mat(2, 2, CV_8UC3);
    cv::theRNG().fill(colorNoise16, cv::RNG::UNIFORM, 0, 65535 );
	Vec3s intensity = colorNoise16.at<Vec3s>(y, x);
	ushort blue16 = colorNoise16.val[0];
	ushort green16 = colorNoise16.val[1];
	ushort red16 = colorNoise16.val[2];
	std::cout << " blue: " << blue16 << " , green: " << green16 << " , red: " << red16 << std::endl;
/*
 Mapping of Type to Number in OpenCV

        C1	C2	C3	C4
CV_8U	0	8	16	24
CV_8S	1	9	17	25
CV_16U	2	10	18	26
CV_16S	3	11	19	27
CV_32S	4	12	20	28
CV_32F	5	13	21	29
CV_64F	6	14	22	30
*/

} // end of main


void showMatrix(cv::Mat mat)
{
    cv::namedWindow("current Matrix", cv::WINDOW_AUTOSIZE);
    cv::imshow("current Matrix", mat);
    int wdt = mat.size().width, hgt = mat.size().height;
    cv::resizeWindow("current Matrix", wdt, hgt);
    cv::waitKey(0);
    cv::destroyWindow("current Matrix");
}


// https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv
// https://udayawijenayake.com/2021/06/07/opencv-data-types/
void printMatrix(char* pMessage, cv::Mat& mat)
{
    printf("%s\n", pMessage);

    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {

            switch (mat.depth())
            {
            case CV_8U:
            {
                printf("%*u ", 3, mat.at<uchar>(r, c));
                break;
            }
            case CV_8S:
            {
                printf("%*hhd ", 4, mat.at<schar>(r, c));
                break;
            }
            case CV_16U:
            {
                printf("%*hu ", 5, mat.at<ushort>(r, c));
                break;
            }
            case CV_16S:
            {
                printf("%*hd ", 6, mat.at<short>(r, c));
                break;
            }
            case CV_32S:
            {
                printf("%*d ", 6, mat.at<int>(r, c));
                break;
            }
            case CV_32F:
            {
                printf("%*.4f ", 10, mat.at<float>(r, c));
                break;
            }
            case CV_64F:
            {
                printf("%*.4f ", 10, mat.at<double>(r, c));
                break;
            }
            }
        } printf("\n");
    } printf("\n");
}


std::string openCVType2str(int type) 
{
    std::string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
        case CV_8U:  r = "8U"; break;   // Unsigned 8bits	uchar	0 ~ 255
        case CV_8S:  r = "8S"; break;   // Signed 8bits	    char	-128 ~ 127
        case CV_16U: r = "16U"; break;  // Unsigned 16bits	ushort	0 ~ 65535
        case CV_16S: r = "16S"; break;  // Signed 16bits	short	-32768 ~ 32767
        case CV_32S: r = "32S"; break;  // Signed 32bits	int	    -2147483648 ~ 2147483647
        case CV_32F: r = "32F"; break;  // Float 32bits	    float	-1.18e-38 ~ 3.40e-38
        case CV_64F: r = "64F"; break;  // Double 64bits	double	-1.7e+308 ~ +1.7e+308
        default:     r = "User"; break; // anything goes, e. g. Unsigned 1bit	bool	0 and 1	(=IPL_DEPTH_1U)
    }

    r += "C";
    r += (chans + '0');

    return r;
}


//https://stackoverflow.com/questions/1001307/detecting-endianness-programmatically-in-a-c-program
bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = { 0x01020304 };

    return bint.c[0] == 1;
}
