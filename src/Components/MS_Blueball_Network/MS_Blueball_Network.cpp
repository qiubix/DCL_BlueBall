/*!
 * \file
 * \brief
 */

#include <memory>
#include <string>

#include "MS_Blueball_Network.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace MS_Blueball {

using namespace cv;

MS_Blueball_Network::MS_Blueball_Network(const std::string & name) : Base::Component(name)
{
    LOG(LTRACE) << "Hello MS_Blueball_Network\n";
}

MS_Blueball_Network::~MS_Blueball_Network()
{
    LOG(LTRACE) << "Good bye MS_Blueball_Network\n";
}

void MS_Blueball_Network::prepareInterface()
{

LOG(LTRACE) << "MS_Blueball_Network::initialize\n";

    // Register data streams, events and event handlers HERE!

    // Register handler.
    h_onNewImage.setup(this, &MS_Blueball_Network::onNewImage);
    registerHandler("onNewImage", &h_onNewImage);

    // Register data streams.
    registerStream("in_img", &in_img);
    registerStream("in_imagePosition", &in_imagePosition);

    registerStream("out_img", &out_img);

    theNet.ReadFile("/home/qiubix/DCL/Blueball/blueball_network.xdsl", DSL_XDSL_FORMAT);

}

bool MS_Blueball_Network::onInit()
{
    LOG(LTRACE) << "MS_Blueball_Network::onInit()\n";
        return true;
}

bool MS_Blueball_Network::onFinish()
{
    LOG(LTRACE) << "MS_Blueball_Network::finish\n";

	return true;
}

bool MS_Blueball_Network::onStep()
{
    LOG(LTRACE) << "MS_Blueball_Network::step\n";
	return true;
}

bool MS_Blueball_Network::onStop()
{
	return true;
}

bool MS_Blueball_Network::onStart()
{
	return true;
}

void MS_Blueball_Network::onNewImage()
{

    Mat img = in_img.read();

    theNet.SetDefaultBNAlgorithm(DSL_ALG_BN_LAURITZEN);

    Types::ImagePosition imagePosition = in_imagePosition.read();

    updateVectorOfFeatures(imagePosition);

    double* newProbabilities = computeProbabilities();

    //FIXME: change names
    double highFlatnessProbability = newProbabilities[0];
    double highAreaProbability = newProbabilities[1];

    int ellipse = theNet.FindNode("ellipse");
    int area = theNet.FindNode("area");

    DSL_doubleArray theProbs;
    theProbs.SetSize(2);

    theProbs[0] = highFlatnessProbability;
    theProbs[1] = 1 - highFlatnessProbability;
    theNet.GetNode(ellipse) -> Definition() -> SetDefinition(theProbs);

    theProbs[0] = highAreaProbability;
    theProbs[1] = 1 - highAreaProbability;
    theNet.GetNode(area) -> Definition() -> SetDefinition(theProbs);

    theNet.UpdateBeliefs();

    //TODO: Compute decision

    out_img.write(img);

}

void MS_Blueball_Network::updateVectorOfFeatures(Types::ImagePosition imagePosition)
{
    if(features.size() == 0) {
        vector <double> flatness;
        vector <double> area;
        features.push_back(flatness);
        features.push_back(area);
    }
    double diameter = imagePosition.elements[2];
    double newFlatness = imagePosition.elements[3];
    //double newArea = imagePosition.elements[4];

    //FIXME: Use logger?
    std::cout << "Diameter: " << diameter << std::endl;
    std::cout << "Flatness: " << flatness << std::endl;

    features[0].push_back(newFlatness);
    features[1].push_back(newArea);
}

double* MS_Blueball_Network::computeProbabilities()
{
    //TODO: compute new probabilities using feature vector
    double probabilities[2] = {0.5, 0.5};
    return probabilities;
}

void MS_Blueball_Network::updateNetwork(double* newProbabilities)
{
    //FIXME: change names
    double highFlatnessProbability = newProbabilities[0];
    double highAreaProbability = newProbabilities[1];

    int ellipse = theNet.FindNode("ellipse");
    int area = theNet.FindNode("area");

    DSL_doubleArray theProbs;
    theProbs.SetSize(2);

    theProbs[0] = highFlatnessProbability;
    theProbs[1] = 1 - highFlatnessProbability;
    theNet.GetNode(ellipse) -> Definition() -> SetDefinition(theProbs);

    theProbs[0] = highAreaProbability;
    theProbs[1] = 1 - highAreaProbability;
    theNet.GetNode(area) -> Definition() -> SetDefinition(theProbs);

    theNet.UpdateBeliefs();
}

}//: namespace MS_Blueball
}//: namespace Processors
