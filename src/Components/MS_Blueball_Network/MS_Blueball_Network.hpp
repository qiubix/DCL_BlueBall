/*!
 * \file
 * \brief
 */

#ifndef MS_BLUEBALL_NETWORK_HPP_
#define MS_BLUEBALL_NETWORK_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "DataStream.hpp"

#include <opencv2/opencv.hpp>
#include "../../../lib/SMILE/smile.h"

#include "Types/ImagePosition.hpp"

namespace Processors {
namespace MS_Blueball {

using namespace cv;

/*!
 * \class MS_Blueball_Network
 * \brief MS_Blueball_Network processor class.
 */
class MS_Blueball_Network: public Base::Component
{
public:
    /*!
     * Constructor.
     */
    MS_Blueball_Network(const std::string & name = "");

    /*!
     * Destructor
     */
    virtual ~MS_Blueball_Network();


protected:

    DSL_network theNet;

    vector <vector <double> > features;

    double newProbabilities[2];

    // Input data stream
    //Base::DataStreamIn <Types::ImagePosition> in_imagePosition;
    //Base::DataStreamIn <Mat> in_img;
    Base::DataStreamIn < vector <double> > in_ellipse;

    // Output data stream
    Base::DataStreamOut < vector <double> > out_probabilities;
    //Base::DataStreamOut <Mat> out_img;

    // Event handler function.
    void onNewImage();

    // Event handler.
    Base::EventHandler <MS_Blueball_Network> h_onNewImage;

    // Event emited after the image is processed.
    //Base::Event * newImage;

    void prepareInterface();
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

private:

    void initNetwork();

    void createNetwork();

    void updateFeatureVector(const std::vector<double> ellipse);

    void calculateProbabilities();

    void updateNetwork(double* newProbabilities);

    int getOutcomePosition(int node, std::string outcome);

    double getOutcomeProbability(int node, std::string outcome);

    void computeDecision();

    void displayProbability(std::string message, double probability);

};

}//: namespace MS_Blueball
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_COMPONENT("MS_Blueball_Network", Processors::MS_Blueball::MS_Blueball_Network)

#endif /* MS_BLUEBALL_NETWORK_HPP_ */
