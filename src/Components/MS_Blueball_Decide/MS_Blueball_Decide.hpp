/*!
 * \file MS_Blueball_Decide.hpp
 * \brief
 * \author qiubix
 * \date
 */

#ifndef MS_BLUEBALL_DECIDE_HPP_
#define MS_BLUEBALL_DECIDE_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "DataStream.hpp"

#include <opencv2/opencv.hpp>
#include <highgui.h>

#include <vector>

#include "Types/BlobResult.hpp"
#include "Types/DrawableContainer.hpp"
#include "Types/ImagePosition.hpp"

namespace Processors {
namespace MS_Blueball {

using namespace cv;


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
    //Base::Props * getProperties()
    //{
     //   return &props;
    //}

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
    void onStep();

    /*!
     * Start component
     */
    bool onStart();

    /*!
     * Stop component
     */
    bool onStop();


    /// New image is waiting
    Base::EventHandler <MS_Blueball_Decide> h_onStep;
    /// Input blobs
    Base::DataStreamIn <Types::Blobs::BlobResult> in_blobs;

    /// Input hue image
    Base::DataStreamIn <cv::Mat> in_hue;

    /// Input data stream containing camera properties.
    Base::DataStreamIn <cv::Size> in_cameraInfo;

    /// Event raised, when data is processed
    //	Base::Event * newImage;

    /// Output data stream - list of ellipses around found Blueballs
    Base::DataStreamOut < Types::DrawableContainer > out_balls;

    /// Position of the blueball in image coordinates.
    Base::DataStreamOut <Types::ImagePosition> out_imagePosition;

    /// Properties
    //Props props;

private:
    cv::Mat hue_img;
    cv::Mat segments;

    bool blobs_ready;
    bool hue_ready;

    Types::Blobs::BlobResult blobs;

    // Data related to the utilized camera.
    cv::Size cameraInfo;
};

}//: namespace MS_Blueball
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_COMPONENT("MS_Blueball_Decide", Processors::MS_Blueball::MS_Blueball_Decide)

#endif /* MS_BLUEBALL_DECIDE_HPP_ */

