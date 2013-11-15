/*!
 * \file
 * \brief
 */

#ifndef DICENETWORK_HPP_
#define DICENETWORK_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"

#include <opencv2/opencv.hpp>
#include "../../SMILE/smile.h"

namespace Processors {
namespace DiceNetwork {

using namespace cv;

/*!
 * \class DiceNetwork
 * \brief DiceNetwork processor class.
 */
class DiceNetwork: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	DiceNetwork(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~DiceNetwork();


protected:

	DSL_network theNet;

	// Input data stream
	Base::DataStreamIn <Mat> in_img;

	Base::DataStreamIn <vector<vector<int> > > in_doubles;
	Base::DataStreamIn <vector<vector<int> > > in_triples;	

	Base::DataStreamIn <vector<vector<double> > > in_close;
	Base::DataStreamIn <vector<vector<double> > > in_parallel;

	// Output data stream
	Base::DataStreamOut <Mat> out_img;

	// Event handler function.	
	void onNewImage();

	// Event handler.
	Base::EventHandler <DiceNetwork> h_onNewImage;

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

};

}//: namespace DiceNetwork
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_COMPONENT("DiceNetwork", Processors::DiceNetwork::DiceNetwork);

#endif /* DICENETWORK_HPP_ */
