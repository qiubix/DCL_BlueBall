/*!
 * \file LUT.hpp
 * \brief
 * \author mstefanc
 * \date 2010-07-05
 */

#ifndef LUT_HPP_
#define LUT_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "DataStream.hpp"

#include "Property.hpp"

#include <opencv2/opencv.hpp>
#include <highgui.h>

namespace Processors {
namespace Blueball {

using namespace cv;

/*!
 * \class LUT
 * \brief Example processor class.
 */
class LUT: public Base::Component
{
public:
    /*!
     * Constructor.
     */
    LUT(const std::string & name = "");

    /*!
     * Destructor
     */
    virtual ~LUT();


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

    /// Event handler.
    Base::EventHandler <LUT> h_onNewImage;

    /// Input image
    Base::DataStreamIn <Mat> in_img;

    /// Output data stream - hue part with continous red
    Base::DataStreamOut <Mat> out_hue;

    /// Output data stream - segments
    Base::DataStreamOut <Mat> out_segments;

private:
    cv::Mat hue_img;
    cv::Mat segments;

    Base::Property<int> m_hue_threshold_1;
    Base::Property<int> m_hue_threshold_2;
    Base::Property<int> m_sat_threshold_1;
    Base::Property<int> m_val_threshold_1;
};

}//: namespace Blueball
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_COMPONENT("LUT", Processors::Blueball::LUT)

#endif /* LUT_HPP_ */

