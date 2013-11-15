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

        registerStream("in_doubles", &in_doubles);
        registerStream("in_triples", &in_triples);
        registerStream("in_close", &in_close);
        registerStream("in_parallel", &in_parallel);

        registerStream("out_img", &out_img);

    theNet.ReadFile("/home/qiubix/DCL/blueball_network.xdsl", DSL_XDSL_FORMAT);

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

	vector<vector<int> > doubles = in_doubles.read();
	vector<vector<int> > triples = in_triples.read();	

	vector<vector<double> > close = in_close.read();
	vector<vector<double> > parallel = in_parallel.read();

	for (int i=0; i<triples.size(); i++)
	{
		int iA,iB,iC;
		double AcB, AcC, BcC;
		double ApB, ApC, BpC;

		iA = triples[i][0];
		iB = triples[i][1];
		iC = triples[i][2];

		AcB = close[iA][iB];
		AcC = close[iA][iC];
		BcC = close[iB][iC];

//		AcB = 0.8;
//		AcC = 0.6;
//		BcC = 0.6;

		LOG(LWARNING) << "AcB: " << AcB << " BcC: " << BcC << " AcC: " << AcC << "\n";

		ApB = parallel[iA][iB];
		ApC = parallel[iA][iC];
		BpC = parallel[iB][iC];

//		ApB = 1.0;
//		ApC = 0.8;
//		BpC = 0.8;
		
		LOG(LWARNING) << "ApB: " << ApB << " BpC: " << BcC << " ApC: " << ApC << "\n";

		int closeAB = theNet.FindNode("closeAB");
		int closeAC = theNet.FindNode("closeAC");		
		int closeBC = theNet.FindNode("closeBC");

		int parallelAB = theNet.FindNode("parallelAB");
		int parallelAC = theNet.FindNode("parallelAC");		
		int parallelBC = theNet.FindNode("parallelBC");

 		DSL_doubleArray theProbs;
    		theProbs.SetSize(2);

    		theProbs[0] = AcB;
    		theProbs[1] = 1 - AcB;
    		theNet.GetNode(closeAB)->Definition()->SetDefinition(theProbs);

    		theProbs[0] = AcC;
    		theProbs[1] = 1 - AcC;
    		theNet.GetNode(closeAC)->Definition()->SetDefinition(theProbs);

    		theProbs[0] = BcC;
    		theProbs[1] = 1 - BcC;
    		theNet.GetNode(closeBC)->Definition()->SetDefinition(theProbs);

    		theProbs[0] = ApB;
    		theProbs[1] = 1 - ApB;
    		theNet.GetNode(parallelAB)->Definition()->SetDefinition(theProbs);

    		theProbs[0] = ApC;
    		theProbs[1] = 1 - ApC;
    		theNet.GetNode(parallelAC)->Definition()->SetDefinition(theProbs);

    		theProbs[0] = BpC;
    		theProbs[1] = 1 - BpC;
    		theNet.GetNode(parallelBC)->Definition()->SetDefinition(theProbs);
/*
		theNet.GetNode(closeAB)->Value()->SetEvidence(AcB);
		theNet.GetNode(closeAC)->Value()->SetEvidence(AcC);
		theNet.GetNode(closeBC)->Value()->SetEvidence(BcC);

		theNet.GetNode(parallelAB)->Value()->SetEvidence(ApB);
		theNet.GetNode(parallelAC)->Value()->SetEvidence(ApC);
		theNet.GetNode(parallelBC)->Value()->SetEvidence(BpC);
*/		
		theNet.UpdateBeliefs();

		int single_side = theNet.FindNode("1 side");
		int double_side = theNet.FindNode("2 sides");
		int triple_side = theNet.FindNode("3 sides");

		int dice = theNet.FindNode("dice");

		double P_s_side; 
		double P_d_side;
		double P_t_side;

		double P_dice;

		DSL_sysCoordinates theCoordinates(*theNet.GetNode(dice)->Value());
		DSL_idArray *theNames;
		theNames = theNet.GetNode(dice)->Definition()->GetOutcomesNames();

		int moderateIndex = theNames->FindPosition("YES");
		theCoordinates[0] = moderateIndex;
		theCoordinates.GoToCurrentPosition();

		P_dice = theCoordinates.UncheckedValue();

		LOG(LWARNING) << "P_dice: " << P_dice << "\n";

	}


	//DSL_idArray *theNames;
	//theNames = theNet.GetNode(moderateIndex)->Definition()->GetOutcomesNames();
	
	//theNet.WriteFile("newNet.xdsl", DSL_XDSL_FORMAT);

	out_img.write(img);

}

}//: namespace MS_Blueball
}//: namespace Processors
