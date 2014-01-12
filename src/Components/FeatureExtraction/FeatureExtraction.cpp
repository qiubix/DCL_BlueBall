/*!
 * \file FeatureExtraction.cpp
 * \brief
 * \author mstefanc
 * \date 2010-07-05
 */

#include <memory>
#include <string>
#include <math.h>

#include "FeatureExtraction.hpp"
#include "Logger.hpp"

#include "Types/Ellipse.hpp"

namespace Processors {
namespace Blueball {

// OpenCV writes hue in range 0..180 instead of 0..360
#define H(x) (x>>1)

FeatureExtraction::FeatureExtraction(const std::string & name) : Base::Component(name)
{
    LOG(LTRACE) << "Hello FeatureExtraction\n";
    blobs_ready = hue_ready = false;
}

FeatureExtraction::~FeatureExtraction()
{
    LOG(LTRACE) << "Good bye FeatureExtraction\n";
}

void FeatureExtraction::prepareInterface()
{

    LOG(LTRACE) << "FeatureExtraction::initialize\n";

    h_onStep.setup(this, &FeatureExtraction::onStep);
    registerHandler("onStep", &h_onStep);

    // Register input streams.
    registerStream("in_blobs", &in_blobs);
    registerStream("in_hue", &in_hue);
    registerStream("in_cameraInfo", &in_cameraInfo);

    addDependency("onStep", &in_blobs);
    addDependency("onStep", &in_hue);
    addDependency("onStep", &in_cameraInfo);

    //	found = registerEvent("Found");
    //notFound = registerEvent("NotFound");
    //newImage = registerEvent("newImage");

    // Register output streams.
    registerStream("out_balls", &out_balls);
    registerStream("out_imagePosition", &out_imagePosition);
    registerStream("out_features", &out_features);

}

bool FeatureExtraction::onInit()
{
    return true;
}

bool FeatureExtraction::onFinish()
{
    LOG(LTRACE) << "FeatureExtraction::finish\n";

    return true;
}

void FeatureExtraction::onStep()
{
    LOG(LTRACE) << "FeatureExtraction::step\n";

    blobs_ready = hue_ready = false;


    blobs = in_blobs.read();
    cameraInfo = in_cameraInfo.read();
    hue_img = in_hue.read();

    try {
        int id = 0;
        int i;
        Types::Blobs::Blob currentBlob;
        Types::DrawableContainer Blueballs;
        // Check whether there is any blue blob detected.

        if (blobs.GetNumBlobs() <0) {
            LOG(LTRACE) << "Blue blob not found.\n";

            // Disregarding the fact - write output stream.
            out_balls.write(Blueballs);
            // Raise events.
            //notFound->raise();
            //newImage->raise();
        }

        blobs.GetNthBlob(Types::Blobs::BlobGetArea(), 0, currentBlob);


        // get blob bounding rectangle and ellipse
        CvBox2D r2 = currentBlob.GetEllipse();

        //std::cout << "Center: " << r2.center.x << "," << r2.center.y << "\n";
        ++id;

        // blob moments
        double m00, m10, m01, m11, m02, m20;
        double M11, M02, M20, M7, a, b, wsp_elips;

        // calculate moments
        m00 = currentBlob.Moment(0,0);
        m01 = currentBlob.Moment(0,1);
        m10 = currentBlob.Moment(1,0);
        m11 = currentBlob.Moment(1,1);
        m02 = currentBlob.Moment(0,2);
        m20 = currentBlob.Moment(2,0);

        M11 = m11 - (m10*m01)/m00;
        M02 = m02 - (m01*m01)/m00;
        M20 = m20 - (m10*m10)/m00;
        // for circle it should be ~0.0063
        M7 = (M20*M02-M11*M11) / (m00*m00*m00*m00);

        a=sqrt(2*(M20+M02+sqrt(M11*M11+(M20-M02)*(M20-M02))));
        b=sqrt(2*(M20+M02-sqrt(M11*M11+(M20-M02)*(M20-M02))));
        wsp_elips=b/a;

        Types::Ellipse* tmpball = new Types::Ellipse(Point(r2.center.x, r2.center.y), Size(r2.size.width, r2.size.height), r2.angle);

        // Add to list.
        Blueballs.add(tmpball);

        // Write blueball list to stream.
        out_balls.write(Blueballs);

        vector<double> features;

        double maxPixels = std::max(cameraInfo.width, cameraInfo.height);
        double diameter=std::max(r2.size.width, r2.size.height)/maxPixels;
        double _a = std::max(r2.size.width, r2.size.height)/2;
        double _b = std::min(r2.size.width, r2.size.height)/2;
        double convexity = _b/_a;
        double area = M_PI*4*_a*_b;

        features.push_back(_a);
        features.push_back(_b);
        features.push_back(convexity);
        features.push_back(area);
        out_features.write(features);

        //std::cout << a/maxPixels << "\t" << b/maxPixels << "\t" << area << std::endl;

        Types::ImagePosition imagePosition;
        //std::cout << "\n ================== New image ===================== \n";
        //std::cout << "a: " << _a <<std::endl;
        //std::cout << "b: " << _b <<std::endl;
        //std::cout << "b/a: " << _b/_a <<std::endl;
        //std::cout << "pole: " << M_PI*4*_a*_b <<std::endl;
        //std::cout << "Srednica: " <<  std::max(r2.size.width, r2.size.height)<<std::endl;
        // Change coordinate system hence it will return coordinates from (-1,1), center is 0.
        imagePosition.elements[0] = (r2.center.x - cameraInfo.width / 2) / maxPixels;
        imagePosition.elements[1] = (r2.center.y - cameraInfo.height / 2) / maxPixels;
        // Elipse factor
        imagePosition.elements[2] = diameter;
        // Rotation - in case of blueball - zero.
        imagePosition.elements[3] = wsp_elips;

        // Area of an object
        //imagePosition.elements[2] = area;

        // Write to stream.
        out_imagePosition.write(imagePosition);

    } catch (...) {
        LOG(LERROR) << "FeatureExtraction::onNewImage failed\n";
    }
}

bool FeatureExtraction::onStop()
{
    return true;
}

bool FeatureExtraction::onStart()
{
    return true;
}


}//: namespace Blueball
}//: namespace Processors
