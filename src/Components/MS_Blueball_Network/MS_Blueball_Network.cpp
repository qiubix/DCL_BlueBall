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
    registerStream("in_imagePosition", &in_imagePosition);

    //addDependency("onNewImage", &in_img);
    addDependency("onNewImage", &in_imagePosition);

    registerStream("out_probabilities", &out_probabilities);

    initNetwork();

}

void MS_Blueball_Network::initNetwork()
{
    int result = theNet.ReadFile("/home/qiubix/DCL/BlueBall/in_blueball_network.xdsl", DSL_XDSL_FORMAT);
    //theNet.ReadFile("/home/kkaterza/DCL/BlueBall/in_blueball_network.xdsl", DSL_XDSL_FORMAT);
    //createNetwork();
    LOG(LWARNING) << "Reading network file: " << result;
}

void MS_Blueball_Network::createNetwork()
{
    DSL_idArray outcomes;

    int ellipse = theNet.AddNode(DSL_CPT, "ellipse");
    outcomes.Add("HIGH");
    outcomes.Add("LOW");
    theNet.GetNode(ellipse)->Definition()->SetNumberOfOutcomes(outcomes);

    int area = theNet.AddNode(DSL_CPT, "area");
    outcomes.Flush();
    outcomes.Add("HIGH");
    outcomes.Add("LOW");
    theNet.GetNode(area)->Definition()->SetNumberOfOutcomes(outcomes);

    int flat = theNet.AddNode(DSL_CPT, "flat");
    outcomes.Flush();
    outcomes.Add("YES");
    outcomes.Add("NO");
    theNet.GetNode(flat)->Definition()->SetNumberOfOutcomes(outcomes);

    theNet.AddArc(ellipse, flat);
    theNet.AddArc(area, flat);

    DSL_doubleArray theProbs;
    theProbs.SetSize(2);
    theProbs[0] = 0.5;
    theProbs[1] = 0.5;

    theNet.GetNode(ellipse)->Definition()->SetDefinition(theProbs);
    theNet.GetNode(area)->Definition()->SetDefinition(theProbs);

    DSL_sysCoordinates theCoordinates(*theNet.GetNode(flat)->Definition());
    theCoordinates.UncheckedValue() = 0.95;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.05;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.65;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.35;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.6;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.4;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.01;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.99;
    theCoordinates.Next();

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
    std::cout << "\n";
    theNet.SetDefaultBNAlgorithm(DSL_ALG_BN_LAURITZEN);

    Types::ImagePosition imagePosition = in_imagePosition.read();
    updateFeatureVector(imagePosition);

    calculateProbabilities();
    updateNetwork(newProbabilities);

    computeDecision();
}

void MS_Blueball_Network::updateFeatureVector(Types::ImagePosition imagePosition)
{
    if(features.size() == 0) {
        vector <double> flatness;
        vector <double> area;
        features.push_back(flatness);
        features.push_back(area);
    }
    //double newDiameter = imagePosition.elements[2];
    double newFlatness = imagePosition.elements[3];
    double newArea = imagePosition.elements[2];

    //std::cout << "Diameter: " << newDiameter << "\t";
    std::cout << "Flatness: " << newFlatness << "\t";
    std::cout << "Area: " << newArea << "\t";

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
    theNet.UpdateBeliefs();

    double highFlatnessProbability = newProbabilities[0];
    double highAreaProbability = newProbabilities[1];

    //std::cout << " High flatness prob: " << highFlatnessProbability << "\t";
    int ellipse = theNet.FindNode("ellipse");
    int area = theNet.FindNode("area");
    int flat = theNet.FindNode("flat");

    theNet.GetNode(ellipse)->Value()->ClearEvidence();
    theNet.GetNode(area)->Value()->ClearEvidence();

    DSL_doubleArray theProbs;
    theProbs.SetSize(2);
    theProbs[0] = highFlatnessProbability;
    theProbs[1] = 1 - highFlatnessProbability;
    theNet.GetNode(ellipse) -> Definition() -> SetDefinition(theProbs);


    /*
    theProbs[0] = highAreaProbability;
    theProbs[1] = 1 - highAreaProbability;
    theNet.GetNode(area) -> Definition() -> SetDefinition(theProbs);
    */

    /*
    if (highFlatnessProbability != 0) {
        theNet.GetNode(ellipse)->Value()->SetEvidence(0);
    }
    */

    theNet.UpdateBeliefs();
}


int MS_Blueball_Network::getOutcomePosition(int node, std::string outcome)
{
    DSL_idArray *theNames = theNet.GetNode(node)->Definition()->GetOutcomesNames();
    return theNames->FindPosition(outcome.c_str());
}

double MS_Blueball_Network::getOutcomeProbability(int node, std::string outcome)
{
    DSL_sysCoordinates theFlatnessCoordinates(*theNet.GetNode(node)->Value());
    theFlatnessCoordinates[0] = getOutcomePosition(node, outcome);
    theFlatnessCoordinates.GoToCurrentPosition();
    return theFlatnessCoordinates.UncheckedValue();
}

void MS_Blueball_Network::computeDecision()
{
    vector <double> resultingProbabilities;

    int ellipse = theNet.FindNode("ellipse");
    int area = theNet.FindNode("area");
    int flat = theNet.FindNode("flat");

    double ellipseProbability = getOutcomeProbability(ellipse, "HIGH");
    double areaProbability = getOutcomeProbability(area, "HIGH");
    double flatProbability = getOutcomeProbability(flat, "YES");

    displayProbability("ellipse cpt", ellipseProbability);
    displayProbability("area cpt", areaProbability);
    displayProbability("object is flat", flatProbability);

    theNet.WriteFile("out_blueball_network.xdsl", DSL_XDSL_FORMAT);

    resultingProbabilities.push_back(flatProbability);
    out_probabilities.write(resultingProbabilities);

}

void MS_Blueball_Network::displayProbability(std::string message, double probability)
{
    std::cout << " " << message << ": " << probability << "\t";
}

}//: namespace MS_Blueball
}//: namespace Processors
