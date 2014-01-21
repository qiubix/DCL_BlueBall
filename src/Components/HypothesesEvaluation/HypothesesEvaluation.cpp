/*!
 * \file
 * \brief
 */

#include <memory>
#include <string>
#include <cmath>

#include "HypothesesEvaluation.hpp"
#include "Common/Logger.hpp"

namespace Processors {
namespace Blueball {

using namespace cv;

HypothesesEvaluation::HypothesesEvaluation(const std::string & name) : Base::Component(name)
{
    LOG(LTRACE) << "Hello HypothesesEvaluation\n";
}

HypothesesEvaluation::~HypothesesEvaluation()
{
    LOG(LTRACE) << "Good bye HypothesesEvaluation\n";
}

void HypothesesEvaluation::prepareInterface()
{

    LOG(LTRACE) << "HypothesesEvaluation::initialize\n";

    // Register data streams, events and event handlers HERE!

    // Register handler.
    h_onNewImage.setup(this, &HypothesesEvaluation::onNewImage);
    registerHandler("onNewImage", &h_onNewImage);

    // Register data streams.
    registerStream("in_features", &in_features);
    addDependency("onNewImage", &in_features);

    registerStream("out_probabilities", &out_probabilities);

    initNetwork();

}

void HypothesesEvaluation::initNetwork()
{
    //int result = theNet.ReadFile("/home/qiubix/DCL/BlueBall/in_blueball_network.xdsl", DSL_XDSL_FORMAT);
    int result = theNet.ReadFile("/home/kkaterza/DCL/BlueBall/in_blueball_network.xdsl", DSL_XDSL_FORMAT);
    //createNetwork();
    LOG(LWARNING) << "Reading network file: " << result;
}

void HypothesesEvaluation::createNetwork()
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

    int nonflat = theNet.AddNode(DSL_CPT, "nonflat");
    outcomes.Flush();
    outcomes.Add("YES");
    outcomes.Add("NO");
    theNet.GetNode(nonflat)->Definition()->SetNumberOfOutcomes(outcomes);

    theNet.AddArc(ellipse, flat);
    theNet.AddArc(area, flat);

    theNet.AddArc(ellipse, nonflat);
    theNet.AddArc(area, nonflat);

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

    //DSL_sysCoordinates theCoordinates(*theNet.GetNode(nonflat)->Definition());
    theCoordinates.LinkTo(*theNet.GetNode(nonflat)->Value());
    theCoordinates.GoFirst();
    theCoordinates.UncheckedValue() = 0.05;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.95;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.35;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.65;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.4;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.6;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.99;
    theCoordinates.Next();
    theCoordinates.UncheckedValue() = 0.01;
    theCoordinates.Next();

    theNet.ClearAllEvidence();

    theNet.WriteFile("newNet.xdsl", DSL_XDSL_FORMAT);

}

bool HypothesesEvaluation::onInit()
{
    LOG(LTRACE) << "HypothesesEvaluation::onInit()\n";
        return true;
}

bool HypothesesEvaluation::onFinish()
{
    LOG(LTRACE) << "HypothesesEvaluation::finish\n";

    return true;
}

bool HypothesesEvaluation::onStep()
{
    LOG(LTRACE) << "HypothesesEvaluation::step\n";
    return true;
}

bool HypothesesEvaluation::onStop()
{
    return true;
}

bool HypothesesEvaluation::onStart()
{
    return true;
}

void HypothesesEvaluation::onNewImage()
{
    std::cout << "\n";
    theNet.SetDefaultBNAlgorithm(DSL_ALG_BN_LAURITZEN);

    std::vector<double> newFeatures = in_features.read();
    updateFeatureVector(newFeatures);

    calculateProbabilities();
    updateNetwork(newProbabilities);

    computeDecision();
}

void HypothesesEvaluation::updateFeatureVector(const std::vector<double> newFeatures)
{
    if(features.size() == 0) {
        vector <double> flatness;
        vector <double> area;
        features.push_back(flatness);
        features.push_back(area);
    }
    //double newDiameter = imagePosition.elements[2];
    //double newFlatness = imagePosition.elements[3];
    //double newArea = imagePosition.elements[2];
    double newFlatness = newFeatures[2];
    double newArea = newFeatures[3];

    //std::cout << "Diameter: " << newDiameter << "\t";
    //std::cout << "Flatness: " << newFlatness << "\t";
    //std::cout << "Area: " << newArea << "\t";

    features[0].push_back(newFlatness);
    features[1].push_back(newArea);
}

void HypothesesEvaluation::calculateProbabilities()
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
    double maxArea = currentArea;
    double current2MaxAreaRatio = 1;
    if(area.size() > 1) {
        maxArea = *std::max_element(area.begin(), area.end());
        current2MaxAreaRatio = currentArea/maxArea;
    }
    if(current2MaxAreaRatio < 0.4) {
        newAreaProbability = 0;
    } else {
        newAreaProbability = (current2MaxAreaRatio-0.4)/0.6;
    }


    newProbabilities[0] = newFlatnessProbability;
    newProbabilities[1] = newAreaProbability;
}

void HypothesesEvaluation::updateNetwork(double* newProbabilities)
{
    theNet.UpdateBeliefs();

    double highFlatnessProbability = newProbabilities[0];
    double highAreaProbability = newProbabilities[1];

    //std::cout << " High flatness prob: " << highFlatnessProbability << "\t";
    int ellipse = theNet.FindNode("ellipse");
    int area = theNet.FindNode("area");
    //int flat = theNet.FindNode("flat");

    theNet.GetNode(ellipse)->Value()->ClearEvidence();
    theNet.GetNode(area)->Value()->ClearEvidence();

    DSL_doubleArray theProbs;
    theProbs.SetSize(2);

    theProbs[0] = highFlatnessProbability;
    theProbs[1] = 1 - highFlatnessProbability;
    theNet.GetNode(ellipse) -> Definition() -> SetDefinition(theProbs);

    theProbs[0] = highAreaProbability;
    theProbs[1] = 1 - highAreaProbability;
    theNet.GetNode(area) -> Definition() -> SetDefinition(theProbs);



    if (highFlatnessProbability > 0.9) {
        theNet.GetNode(ellipse)->Value()->SetEvidence(0);
    }


    theNet.UpdateBeliefs();
}


int HypothesesEvaluation::getOutcomePosition(int node, std::string outcome)
{
    DSL_idArray *theNames = theNet.GetNode(node)->Definition()->GetOutcomesNames();
    return theNames->FindPosition(outcome.c_str());
}

double HypothesesEvaluation::getOutcomeProbability(int node, std::string outcome)
{
    DSL_sysCoordinates theFlatnessCoordinates(*theNet.GetNode(node)->Value());
    theFlatnessCoordinates[0] = getOutcomePosition(node, outcome);
    theFlatnessCoordinates.GoToCurrentPosition();
    return theFlatnessCoordinates.UncheckedValue();
}

void HypothesesEvaluation::computeDecision()
{
    vector <double> resultingProbabilities;

    int ellipse = theNet.FindNode("ellipse");
    int area = theNet.FindNode("area");
    int flat = theNet.FindNode("flat");
    int nonflat = theNet.FindNode("nonflat");

    double ellipseProbability = getOutcomeProbability(ellipse, "HIGH");
    double areaProbability = getOutcomeProbability(area, "HIGH");
    double flatProbability = getOutcomeProbability(flat, "YES");
    double nonflatProbability = getOutcomeProbability(nonflat, "YES");

    displayProbability("ellipse cpt", ellipseProbability);
    displayProbability("area cpt", areaProbability);
    displayProbability("object is flat", flatProbability);
    displayProbability("object is not flat: ", nonflatProbability);

    //theNet.WriteFile("out_blueball_network.xdsl", DSL_XDSL_FORMAT);

    resultingProbabilities.push_back(flatProbability);
    resultingProbabilities.push_back(nonflatProbability);
    out_probabilities.write(resultingProbabilities);

}

void HypothesesEvaluation::displayProbability(std::string message, double probability)
{
    std::cout << " " << message << ": " << probability << "\t";
}

}//: namespace Blueball
}//: namespace Processors
