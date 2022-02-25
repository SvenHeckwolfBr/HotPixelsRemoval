// HotPixelsRemoval.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

int main() // (int argc, char** argv) // 
{
    const std::string keys =
        "{help h usage ? |      | print this message   }"
        "{inputPath      |.     | path to image        }"
        "{outputPath     |.     | path to save image   }"
        ;

    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Application name v1.0.0");

    if (parser.has("help")) {
        parser.printMessage();
    }
 
    std::string inputPath = parser.get<cv::String>(1); // read @image (mandatory, error if not present)
    std::string outputPath = parser.get<cv::String>(2); // read @image (mandatory, error if not present)

    // Read the image as original bpp
    cv::Mat img = cv::imread( inputPath, cv::IMREAD_ANYDEPTH );

    // Be sure that the image is loaded
    if (img.empty())
    {   // No image found at provided location
        return -1;
    }
    // Be sure that the image is 16bpp and single channel
    if (img.type() != CV_16U || img.channels() != 1)
    {   // Wrong image depth or channels
        return -2;
    }
    auto e1 = cv::getTickCount();

    auto newImg = img.clone();
    // loop over every pixel in the image:
    for (int i = 2; i < img.size().width - 2; i++) {
        for (int j = 2; j < img.size().height - 2; j++) {
            // declare parameters for a "near" and a "far" neighborhood 
            //( = a square with all 3x3 or 5x5 pixels around the currently inspected pixel) 
            double nearNeigh = 0.0;
            double farNeigh = 0.0;
            // look at all of the surrounding pixels - first only evaluate the "near" neighborhood:
            for (int m = (j - 1); m < (j + 2); m++) {
                for (int n = (i - 1); n < (i + 2); n++) {
                    if (!(m == j && n == i))
                        nearNeigh += img.at<unsigned short>(m, n);
                }
            }
            nearNeigh /= 8;
            // look at all of the surrounding pixels - now evaluate the "far" neighborhood:
            for (int m = (j - 2); m < (j + 3); m++) {
                for (int n = (i - 2); n < (i + 3); n++) {
                    if (!(m == j && n == i))
                        farNeigh += img.at<unsigned short>(m, n);
                }
            }
            farNeigh /= 24;
            // Now test whether the currently inspected pixel is much brighter than the average of its neighbeorhood:
            // if (img.at<unsigned short>(j, i) < /*4095  (1.2 * nearNeigh) && frame.at<unsigned short>(i, j) <*/ (1.5 * farNeigh)) {
            if (img.at<unsigned short>(j, i) < (1.5 * farNeigh)) { // TODO: Find a more dynamic way to check for brightness ("Otsu-likeish", maybe? - median does not work better...) 
                //int newValS = img.at<unsigned short>(j, i);
                //newImg.at<unsigned short>(j, i) = newValS;
                continue; // do nothing, newImg is already a clone of img.
            }
            else { // if the currently inspected pixel is "too bright", replace it with the mean intensity of its surrounding:
//                double valInt1 = 0;
                double valInt2 = 0;
//                for (int m = (j - 1); m < (j + 2); m++) {
//                    for (int n = (i - 1); n < (i + 2); n++) {
//                        if (!(m == j && n == i))
//                            valInt1 += img.at<unsigned short>(m, n);
//                    }
//                }
//                valInt1 /= 8;
                for (int m = (j - 2); m < (j + 3); m++) {
                    for (int n = (i - 2); n < (i + 3); n++) {
                        if (!(m == j && n == i))
                            valInt2 += img.at<unsigned short>(m, n);
                    }
                } // Testing shwowed that using the wider neighborhood yields smoother results:
                valInt2 /= 24;
                newImg.at<unsigned short>(j, i) = round(valInt2);
            }
        }
    }
    std::vector<int> compression_params;
    compression_params.push_back(cv::ImwriteFlags::IMWRITE_TIFF_COMPRESSION);
    //compression_params.push_back(9);

    bool result = false;
    try
    {
        result = cv::imwrite( outputPath, newImg, compression_params );
    }
    catch (const cv::Exception& ex)
    {
        fprintf(stderr, "Exception converting image to TIFF format: %s\n", ex.what());
    }
    if (result)
        printf("Saved TIFF file with changes.\n");
    else
        printf("ERROR: Can't save TIFF file.\n");

    auto e2 = cv::getTickCount();
    auto t = (e2 - e1) / cv::getTickFrequency();

    std::cout << "time for processing the image of size : width: " << img.size().width << " , height: " << img.size().height << " : " << t << " sec." << std::endl;

    return 0;
}
