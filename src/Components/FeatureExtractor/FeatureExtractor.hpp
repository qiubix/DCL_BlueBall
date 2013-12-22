/*!
 * \file FeatureExtractor.hpp
 * \brief
 * \author qiubix
 * \date
 */

#ifndef FEATURE_EXTRACTOR_HPP_
#define FEATURE_EXTRACTOR_HPP_

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
namespace Blueball {

using namespace cv;


/*!
 * \class FeatureExtractor
 * \brief Example processor class.
 */
class FeatureExtractor: public Base::Component
{
public:
    /*!
     * Constructor.
     */
    FeatureExtractor(const std::string & name = "");

    /*!
     * Destructor
     */
    virtual ~FeatureExtractor();

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
    Base::EventHandler <FeatureExtractor> h_onStep;
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

    Base::DataStreamOut < vector<double> > out_features;

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

}//: namespace Blueball
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_COMPONENT("FeatureExtractor", Processors::Blueball::FeatureExtractor)

#endif /* FEATURE_EXTRACTOR_HPP_ */

