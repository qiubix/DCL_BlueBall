/*!
 * \file MS_Blueball_Decide.cpp
 * \brief
 * \author mstefanc
 * \date 2010-07-05
 */

#include <memory>
#include <string>

#include "MS_Blueball_Decide.hpp"
#include "Logger.hpp"

#include "Types/Ellipse.hpp"

namespace Processors {
namespace MS_Blueball {

// OpenCV writes hue in range 0..180 instead of 0..360
#define H(x) (x>>1)

MS_Blueball_Decide::MS_Blueball_Decide(const std::string & name) : Base::Component(name)
{
	LOG(LTRACE) << "Hello MS_Blueball_Decide\n";
	blobs_ready = hue_ready = false;
}

MS_Blueball_Decide::~MS_Blueball_Decide()
{
	LOG(LTRACE) << "Good bye MS_Blueball_Decide\n";
}

void MS_Blueball_Decide::prepareInterface()
{

    LOG(LTRACE) << "MS_Blueball_Decide::initialize\n";

    h_onNewImage.setup(this, &MS_Blueball_Decide::onNewImage);
    registerHandler("onNewImage", &h_onNewImage);

    h_onNewBlobs.setup(this, &MS_Blueball_Decide::onNewBlobs);
    registerHandler("onNewBlobs", &h_onNewBlobs);

    h_onNewCameraInfo.setup(this, &MS_Blueball_Decide::onNewCameraInfo);
    registerHandler("onNewCameraInfo", &h_onNewCameraInfo);

    // Register input streams.
    registerStream("in_blobs", &in_blobs);
    registerStream("in_hue", &in_hue);
    registerStream("in_cameraInfo", &in_cameraInfo);

//	found = registerEvent("Found");
    //notFound = registerEvent("NotFound");
    //newImage = registerEvent("newImage");

    // Register output streams.
    registerStream("out_balls", &out_balls);
    registerStream("out_imagePosition", &out_imagePosition);

}

bool MS_Blueball_Decide::onInit()
{
        return true;
}

bool MS_Blueball_Decide::onFinish()
{
	LOG(LTRACE) << "MS_Blueball_Decide::finish\n";

	return true;
}

bool MS_Blueball_Decide::onStep()
{
	LOG(LTRACE) << "MS_Blueball_Decide::step\n";

	blobs_ready = hue_ready = false;

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
			return true;
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
			

		double maxPixels = std::max(cameraInfo.size().width, cameraInfo.size().height);
		double diameter=std::max(r2.size.width, r2.size.height)/maxPixels;
		Types::ImagePosition imagePosition;
		std::cout << "Srednica: " <<  std::max(r2.size.width, r2.size.height)<<std::endl;
		// Change coordinate system hence it will return coordinates from (-1,1), center is 0.
		imagePosition.elements[0] = (r2.center.x - cameraInfo.size().width / 2) / maxPixels;
		imagePosition.elements[1] = (r2.center.y - cameraInfo.size().height / 2) / maxPixels;
		// Elipse factor
		imagePosition.elements[2] = diameter;
		// Rotation - in case of blueball - zero.
		imagePosition.elements[3] = wsp_elips;

		// Write to stream.
		out_imagePosition.write(imagePosition);
		// Raise events.
        //found->raise();
        //newImage->raise();

		return true;
	} catch (...) {
		LOG(LERROR) << "MS_Blueball_Decide::onNewImage failed\n";
		return false;
	}
}

bool MS_Blueball_Decide::onStop()
{
	return true;
}

bool MS_Blueball_Decide::onStart()
{
	return true;
}

void MS_Blueball_Decide::onNewImage()
{
	LOG(LTRACE) << "MS_Blueball_Decide::onNewImage\n";

	hue_ready = true;
	hue_img = in_hue.read();
	hue_img = hue_img.clone();
	if (blobs_ready && hue_ready)
		onStep();
}

void MS_Blueball_Decide::onNewBlobs()
{
	LOG(LTRACE) << "MS_Blueball_Decide::onNewBlobs\n";

	blobs = in_blobs.read();
	onStep();
}

void MS_Blueball_Decide::onNewCameraInfo()
{
	LOG(LTRACE) << "MS_Blueball_Decide::onNewCameraInfo\n";

	cameraInfo = in_cameraInfo.read();
	onStep();
}


}//: namespace MS_Blueball
}//: namespace Processors
