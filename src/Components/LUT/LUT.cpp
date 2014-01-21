/*!
 * \file LUT.cpp
 * \brief
 * \author mstefanc
 * \date 2010-07-05
 */

#include <memory>
#include <string>

#include "LUT.hpp"
#include "Logger.hpp"

namespace Processors {
namespace Blueball {

// OpenCV writes hue in range 0..180 instead of 0..360
#define H(x) (x>>1)

LUT::LUT(const std::string & name) : Base::Component(name),
    m_hue_threshold_1("hue_thr_1", 180, "range"),
    m_hue_threshold_2("hue_thr_2", 240, "range"),
    m_sat_threshold_1("sat_thr_1", 100, "range"),
    m_val_threshold_1("val_thr_1", 100, "range")
{
    m_hue_threshold_1.addConstraint("0");
    m_hue_threshold_1.addConstraint("360");


    m_hue_threshold_2.addConstraint("0");
    m_hue_threshold_2.addConstraint("360");

    m_sat_threshold_1.addConstraint("0");
    m_sat_threshold_1.addConstraint("255");

    m_val_threshold_1.addConstraint("0");
    m_val_threshold_1.addConstraint("255");

    registerProperty(m_hue_threshold_1);
    registerProperty(m_hue_threshold_2);
    registerProperty(m_sat_threshold_1);
    registerProperty(m_val_threshold_1);

    LOG(LTRACE) << "Hello LUT\n";
}

LUT::~LUT()
{
    LOG(LTRACE) << "Good bye LUT\n";
}

void LUT::prepareInterface()
{

    LOG(LTRACE) << "LUT::initialize\n";

    h_onNewImage.setup(this, &LUT::onNewImage);
    registerHandler("onNewImage", &h_onNewImage);

    registerStream("in_img", &in_img);
    addDependency("onNewImage", &in_img);

    registerStream("out_hue", &out_hue);
    registerStream("out_segments", &out_segments);

}

bool LUT::onInit()
{
    return true;
}

bool LUT::onFinish()
{
    LOG(LTRACE) << "LUT::finish\n";

    return true;
}

bool LUT::onStep()
{
    LOG(LTRACE) << "LUT::step\n";
    return true;
}

bool LUT::onStop()
{
    return true;
}

bool LUT::onStart()
{
    return true;
}

void LUT::onNewImage()
{
    LOG(LTRACE) << "LUT::onNewImage\n";
    try {
        cv::Mat hsv_img = in_img.read();

        cv::Size size = hsv_img.size();

        hue_img.create(size, CV_8UC1);
        segments.create(size, CV_8UC1);

        // Check the arrays for continuity and, if this is the case,
        // treat the arrays as 1D vectors
        if (hsv_img.isContinuous() && segments.isContinuous() && hue_img.isContinuous()) {
            size.width *= size.height;
            size.height = 1;
        }
        size.width *= 3;

        for (int i = 0; i < size.height; i++) {
            // when the arrays are continuous,
            // the outer loop is executed only once
            // if not - it's executed for each row
            const uchar* hsv_p = hsv_img.ptr <uchar> (i);
            uchar* seg_p = segments.ptr <uchar> (i);
            uchar* hue_p = hue_img.ptr <uchar> (i);

            int j, k = 0;
            for (j = 0; j < size.width; j += 3) {
                uchar hue = hsv_p[j];
                uchar sat = hsv_p[j + 1];
                uchar val = hsv_p[j + 2];

                // label colors
                if (hue < H(m_hue_threshold_1))
                    hue = 0;
                else if (hue < H(m_hue_threshold_2))
                    hue = 255; //blue
                else
                    hue = 0;

                // exclude undersaturated areas (gray levels)
                if (sat < m_sat_threshold_1)
                    hue = 0;

                // exclude too dark areas
                if ((val < m_val_threshold_1))
                    hue = 0;

                seg_p[k] = hue;

                ++k;
            }
        }

        out_hue.write(hue_img);
        out_segments.write(segments);

    }
    catch (Common::DisCODeException& ex) {
        LOG(LERROR) << ex.what() << "\n";
        ex.printStackTrace();
        exit(EXIT_FAILURE);
    }
    catch (const char * ex) {
        LOG(LERROR) << ex;
    }
    catch (...) {
        LOG(LERROR) << "LUT::onNewImage failed\n";
    }
}

}//: namespace Blueball
}//: namespace Processors
