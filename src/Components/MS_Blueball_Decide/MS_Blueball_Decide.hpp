/*!
 * \file MS_Blueball_Decide.hpp
 * \brief
 * \author mstefanc
 * \date 2010-07-05
 */

#ifndef MS_BLUEBALL_DECIDE_HPP_
#define MS_BLUEBALL_DECIDE_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
//#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"

#include <opencv2/opencv.hpp>
#include <highgui.h>

#include <vector>

#include "Types/BlobResult.hpp"
#include "Types/DrawableContainer.hpp"
#include "Types/ImagePosition.hpp"
#include "Types/CameraInfo.hpp"

namespace Processors {
namespace MS_Blueball {

using namespace cv;

/*!
 * \brief MS_Blueball_Decide properties
 */
struct Props: public Base::Props
{
	/*!
	 * \copydoc Base::Props::load
	 */
	void load(const ptree & pt)
	{
	}

	/*!
	 * \copydoc Base::Props::save
	 */
	void save(ptree & pt)
	{
	}
};

/*!
 * \class MS_Blueball_Decide
 * \brief Example processor class.
 */
class MS_Blueball_Decide: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	MS_Blueball_Decide(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~MS_Blueball_Decide();

	/*!
	 * Return window properties
	 */
	Base::Props * getProperties()
	{
		return &props;
	}

    void prepareInterface();

protected:

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();


	/*!
	 * Event handler function.
	 */
	void onNewImage();

	/// New image is waiting
	Base::EventHandler <MS_Blueball_Decide> h_onNewImage;

	/*!
	 * Event handler function.
	 */
	void onNewBlobs();

	/// New set of blobs is waiting
	Base::EventHandler <MS_Blueball_Decide> h_onNewBlobs;

	/// Event handler function.
	void onNewCameraInfo();

	/// New camera info is waiting.
	Base::EventHandler <MS_Blueball_Decide> h_onNewCameraInfo;

	/// Input blobs
	Base::DataStreamIn <Types::Blobs::BlobResult> in_blobs;

	/// Input hue image
	Base::DataStreamIn <cv::Mat> in_hue;

	/// Input data stream containing camera properties.
	Base::DataStreamIn <Types::CameraInfo> in_cameraInfo;

	/// Event raised, when data is processed
//	Base::Event * newImage;

	/// Output data stream - list of ellipses around found Blueballs
	Base::DataStreamOut < Types::DrawableContainer > out_balls;

	/// Position of the blueball in image coordinates.
	Base::DataStreamOut <Types::ImagePosition> out_imagePosition;

	/// Raised when object has been located on the image.
    //Base::Event *found;

	/// Raised when object has not been located on the image.
    //Base::Event *notFound;

	/// Properties
	Props props;

private:
	cv::Mat hue_img;
	cv::Mat segments;

	bool blobs_ready;
	bool hue_ready;

	Types::Blobs::BlobResult blobs;

	// Data related to the utilized camera.
	Types::CameraInfo cameraInfo;
};

}//: namespace MS_Blueball
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_COMPONENT("MS_Blueball_Decide", Processors::MS_Blueball::MS_Blueball_Decide)

#endif /* MS_BLUEBALL_DECIDE_HPP_ */

