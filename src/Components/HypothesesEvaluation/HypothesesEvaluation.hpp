/*!
 * \file
 * \brief
 */

#ifndef HYPOTHESES_EVALUATION_HPP_
#define HYPOTHESES_EVALUATION_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "DataStream.hpp"

#include <opencv2/opencv.hpp>
#include "../../../lib/SMILE/smile.h"

#include "Types/ImagePosition.hpp"

namespace Processors {
namespace Blueball {

using namespace cv;

/*!
 * \class HypothesesEvaluation
 * \brief HypothesesEvaluation processor class.
 */
class HypothesesEvaluation: public Base::Component
{
public:
    /*!
     * Constructor.
     */
    HypothesesEvaluation(const std::string & name = "");

    /*!
     * Destructor
     */
    virtual ~HypothesesEvaluation();


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
    Base::EventHandler <HypothesesEvaluation> h_onNewImage;

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

}//: namespace Blueball
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_COMPONENT("HypothesesEvaluation", Processors::Blueball::HypothesesEvaluation)

#endif /* HYPOTHESES_EVALUATION_HPP_ */
