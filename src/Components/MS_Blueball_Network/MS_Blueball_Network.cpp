/*!
 * \file
 * \brief
 */

#include <memory>
#include <string>
#include <cmath>

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

    //addDependency("onNewImage", &in_img);
    addDependency("onNewImage", &in_imagePosition);

    registerStream("out_img", &out_img);

    theNet.ReadFile("/home/kkaterza/DCL/BlueBall/in_blueball_network.xdsl", DSL_XDSL_FORMAT);
    //theNet.ReadFile("/home/kkaterza/DCL/BlueBall/flatness2.xdsl", DSL_XDSL_FORMAT);
    //createNetwork();

}

void MS_Blueball_Network::createNetwork()
{
    DSL_idArray outcomes;

    int ellipse = theNet.AddNode(DSL_CPT, "ellipse");
    outcomes.Add("HIGH");
    outcomes.Add("LOW");
    theNet.GetNode(ellipse)->Definition()->SetNumberOfOutcomes(outcomes);

    int flat = theNet.AddNode(DSL_CPT, "flat");
    outcomes.Flush();
    outcomes.Add("YES");
    outcomes.Add("NO");
    theNet.GetNode(flat)->Definition()->SetNumberOfOutcomes(outcomes);

    theNet.AddArc(ellipse,flat);

    DSL_doubleArray theProbs;
    theProbs.SetSize(2);

    theProbs[0] = 0.5;
    theProbs[1] = 0.5;
    theNet.GetNode(ellipse)->Definition()->SetDefinition(theProbs);

    DSL_sysCoordinates theCoordinates(*theNet.GetNode(flat)->Definition());
    do {
      theCoordinates.UncheckedValue() = 0.5;
    }
    while (theCoordinates.Next() != DSL_OUT_OF_RANGE);

    theNet.ClearAllEvidence();

    theNet.WriteFile("newNet.xdsl", DSL_XDSL_FORMAT);

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

    //Mat img = in_img.read();

    theNet.SetDefaultBNAlgorithm(DSL_ALG_BN_LAURITZEN);

    Types::ImagePosition imagePosition = in_imagePosition.read();

    updateFeatureVector(imagePosition);

    calculateProbabilities();

    updateNetwork(newProbabilities);

    //TODO: Compute decision


    //out_img.write(img);

}

void MS_Blueball_Network::updateFeatureVector(Types::ImagePosition imagePosition)
{
    if(features.size() == 0) {
        vector <double> flatness;
        vector <double> area;
        features.push_back(flatness);
        features.push_back(area);
    }
    double newDiameter = imagePosition.elements[2];
    double newFlatness = imagePosition.elements[3];
    double newArea = imagePosition.elements[4];

    //std::cout << "Diameter: " << newDiameter << "\n";
    std::cout << "\nFlatness: " << newFlatness << "\t";
    //std::cout << "Area: " << newArea << "\n";

    features[0].push_back(newFlatness);
    features[1].push_back(newArea);
}

void MS_Blueball_Network::calculateProbabilities()
{
    double newFlatnessProbability;
    double newAreaProbability;
    vector <double> flatness = features[0];
    vector <double> area = features[1];
    double currentFlatness = flatness.back();
    double currentArea = area.back();

    if(currentFlatness <= 0.8) {
        newFlatnessProbability = 0;
    } else {
        newFlatnessProbability = 1 - (1-currentFlatness)/0.2;
    }
    //TODO: compute probability for area
    newAreaProbability = 0.5;

    newProbabilities[0] = newFlatnessProbability;
    newProbabilities[1] = newAreaProbability;
}

void MS_Blueball_Network::updateNetwork(double* newProbabilities)
{
    theNet.SetDefaultBNAlgorithm(DSL_ALG_BN_LAURITZEN);
    theNet.UpdateBeliefs();
    //theNet.WriteFile("newNet.xdsl", DSL_XDSL_FORMAT);

    //FIXME: change names
    double highFlatnessProbability = newProbabilities[0];
    double highAreaProbability = newProbabilities[1];

    std::cout << " High flatness prob: " << highFlatnessProbability << "\t";
    int ellipse = theNet.FindNode("ellipse");
    int area = theNet.FindNode("area");
    int flat = theNet.FindNode("flat");

    DSL_doubleArray theProbs;
    theProbs.SetSize(2);
/*
    theProbs[0] = highFlatnessProbability;
    theProbs[1] = 1 - highFlatnessProbability;
    theNet.GetNode(ellipse) -> Definition() -> SetDefinition(theProbs);
    if (highFlatnessProbability != 0) {
        theNet.GetNode(ellipse)->Value()->SetEvidence(0);
    }
    else {
        theNet.GetNode(ellipse)->Value()->ClearEvidence();
    }
*/

/*    theProbs[0] = highAreaProbability;
    theProbs[1] = 1 - highAreaProbability;
    theNet.GetNode(area) -> Definition() -> SetDefinition(theProbs);
*/

    int flatnessEvidence;
    if(highFlatnessProbability != 0) {
        flatnessEvidence = 0;
    } else {
        flatnessEvidence = 1;
    }
    theNet.GetNode(ellipse)->Value()->SetEvidence(flatnessEvidence);


    theNet.UpdateBeliefs();


    displayProbability(ellipse, "HIGH", "ellipse cpt: ");
    displayProbability(flat, "YES", "object is flat ");


    theNet.WriteFile("out_blueball_network.xdsl", DSL_XDSL_FORMAT);
}


void MS_Blueball_Network::displayProbability(int node, std::string outcome, std::string message)
{
    DSL_sysCoordinates theFlatnessCoordinates(*theNet.GetNode(node)->Value());
    theFlatnessCoordinates[0] = getOutcomePosition(node, outcome);
    theFlatnessCoordinates.GoToCurrentPosition();
    double nodeCpt = theFlatnessCoordinates.UncheckedValue();
    std::cout << " " << message << nodeCpt << "\t";
}

int MS_Blueball_Network::getOutcomePosition(int node, std::string outcome)
{
    DSL_idArray *theNames = theNet.GetNode(node)->Definition()->GetOutcomesNames();
    return theNames->FindPosition(outcome.c_str());
}

}//: namespace MS_Blueball
}//: namespace Processors
