//
//  ParameterBag.cpp
//  From Nathan Selikoff on 4/26/14.
//
//

#include "ParameterBag.h"

using namespace ci;
using namespace std;
using namespace Reymenta;

ParameterBag::ParameterBag()
{
	// reset no matter what, so we don't miss anything
	reset();

	// check to see if ReymentaSettings.xml file exists and restore if it does
	fs::path params = getDocumentsDirectory() / "Reymenta" / "DisplacementFluid.xml";
	if (fs::exists(params))
		restore();
}

ParameterBagRef ParameterBag::create()
{
	return shared_ptr<ParameterBag>(new ParameterBag());
}

bool ParameterBag::save()
{
	string filename = "DisplacementFluid.xml";

	// attempt to create "Beautiful Chaos" directory in Documents directory
	fs::path directory = getDocumentsDirectory() / "Reymenta";
	if (!fs::exists(directory)) {
		if (!createDirectories(directory / filename)) {
			return false;
		}
	}

	fs::path path = directory / filename;

	XmlTree settings("settings", "");


	XmlTree OSCReceiverPort("OSCReceiverPort", "");
	OSCReceiverPort.setAttribute("value", toString(mOSCReceiverPort));
	settings.push_back(OSCReceiverPort);

	XmlTree OSCDestinationPort("OSCDestinationPort", "");
	OSCDestinationPort.setAttribute("value", toString(mOSCDestinationPort));
	settings.push_back(OSCDestinationPort);

	XmlTree OSCDestinationHost("OSCDestinationHost", "");
	OSCDestinationHost.setAttribute("value", toString(mOSCDestinationHost));
	settings.push_back(OSCDestinationHost);

	// TODO: test for successful writing of XML
	settings.write(writeFile(path));

	return true;
}

bool ParameterBag::restore()
{
	// check to see if ReymentaSettings.xml file exists
	fs::path params = getDocumentsDirectory() / "Reymenta" / "DisplacementFluid.xml";
	if (fs::exists(params)) {
		// if it does, restore
		const XmlTree xml(loadFile(params));

		if (!xml.hasChild("settings")) {
			return false;
		}
		else {
			const XmlTree settings = xml.getChild("settings");

			if (settings.hasChild("OSCReceiverPort")) {
				XmlTree OSCReceiverPort = settings.getChild("OSCReceiverPort");
				mOSCReceiverPort = OSCReceiverPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCReceiverPort")) {
				XmlTree OSCReceiverPort = settings.getChild("OSCReceiverPort");
				mOSCReceiverPort = OSCReceiverPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationPort")) {
				XmlTree OSCDestinationPort = settings.getChild("OSCDestinationPort");
				mOSCDestinationPort = OSCDestinationPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost")) {
				XmlTree OSCDestinationHost = settings.getChild("OSCDestinationHost");
				mOSCDestinationHost = OSCDestinationHost.getAttributeValue<string>("value");
			}
			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void ParameterBag::reset()
{

	// OSC
	mOSCDestinationHost = "127.0.0.1";// "192.168.0.18";
	mOSCDestinationPort = 7001;
	mOSCReceiverPort = 7000;
	OSCMsg = "OSC listening on port 7000";

	//audio
	// audio in multiplication factor
	mAudioMultFactor = 1.0;
	mUseLineIn = true;
	maxVolume = 0.0f;
	mData = new float[1024];
	for (int i = 0; i < 1024; i++)
	{
		mData[i] = 0;
	}
	for (int i = 0; i < 4; i++)
	{
		iFreqs[i] = i;
	}

	// midi and OSC
	for (int c = 0; c < 128; c++)
	{
		controlValues[c] = 0.01f;
	}
	// red
	controlValues[1] = 1.0f;
	// green
	controlValues[2] = 0.3f;
	// blue
	controlValues[3] = 0.0f;
	// Alpha 
	controlValues[4] = 1.0f;
	// background red
	controlValues[5] = 0.1f;
	// background green
	controlValues[6] = 0.1f;
	// background blue
	controlValues[7] = 0.1f;
	// background alpha
	controlValues[8] = 0.2f;
	// ratio
	controlValues[11] = 20.0f;
	// Speed 
	controlValues[12] = 12.0f;
	// zoom
	controlValues[13] = 1.0f;
	// exposure
	controlValues[14] = 1.0f;
	// Blendmode 
	controlValues[15] = 0.0f;
	// Steps
	controlValues[16] = 16.0f;
	// Pixelate
	controlValues[18] = 1.0f;
	// RotationSpeed
	controlValues[19] = 1.0f;
	// glitch
	controlValues[45] = 0.0f;
	// toggle
	controlValues[46] = 0.0f;
	// vignette
	controlValues[47] = 0.0f;
	// invert
	controlValues[48] = 0.0f;

}
