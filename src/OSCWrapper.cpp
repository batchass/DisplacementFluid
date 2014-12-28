#include "OSCWrapper.h"

using namespace Reymenta;

OSC::OSC(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	for (int i = 0; i < 20; i++)
	{
		skeleton[i] = Vec4i::zero();
	}
	// OSC sender
	mOSCSender.setup(mParameterBag->mOSCDestinationHost, mParameterBag->mOSCDestinationPort);
	// OSC receiver
	mOSCReceiver.setup(mParameterBag->mOSCReceiverPort);
}

OSCRef OSC::create(ParameterBagRef aParameterBag)
{
	return shared_ptr<OSC>(new OSC(aParameterBag));
}

void OSC::update()
{
	// osc
	while (mOSCReceiver.hasWaitingMessages())
	{
		osc::Message message;
		mOSCReceiver.getNextMessage(&message);
		int arg0 = 0;
		int arg1 = 0;
		int arg2 = 0;
		int arg3 = 0;
		int skeletonIndex = 0;
		int jointIndex = 0;
		string oscAddress = message.getAddress();
		string oscArg0 = "";
		string oscArg1 = "";
		string oscArg2 = "";
		string oscArg3 = "";
		int numArgs = message.getNumArgs();
		if (oscAddress == "/midi")
		{
			for (int i = 0; i < message.getNumArgs(); i++) {
				cout << "-- Argument " << i << std::endl;
				cout << "---- type: " << message.getArgTypeName(i) << std::endl;
				if (message.getArgType(i) == osc::TYPE_INT32) {
					try {
						//cout << "------ int value: "<< message.getArgAsInt32(i) << std::endl;
						if (i == 0)
						{
							arg0 = message.getArgAsInt32(i);
							oscArg0 = toString(message.getArgAsInt32(i));
						}
						if (i == 1)
						{
							arg1 = message.getArgAsInt32(i);
							oscArg1 = toString(message.getArgAsInt32(i));
						}
						if (i == 2)
						{
							arg2 = message.getArgAsInt32(i);
							oscArg2 = toString(message.getArgAsInt32(i));
						}
						if (i == 3)
						{
							arg3 = message.getArgAsInt32(i);
							oscArg3 = toString(message.getArgAsInt32(i));
						}
					}
					catch (...) {
						cout << "Exception reading argument as int32" << std::endl;
					}
				}
			}
			if (arg0 < 0) arg0 = 0;
			if (arg1 < 0) arg1 = 0;
			if (arg0 > 4096) arg0 = 4096;
			if (arg1 > 4096) arg1 = 4096;
			float normalizedValue = lmap<float>(arg1, 0.0, 127.0, 0.0, 1.0);
			switch (arg0)
			{
			case 14:
				mParameterBag->controlValues[19] = (arg1 - 63.0) / 63.0;
				break;
			default:
				mParameterBag->controlValues[arg0] = normalizedValue;
				break;
			}

		}
		else if (oscAddress == "/centerXY")
		{
			float x = message.getArgAsFloat(0);
			float y = message.getArgAsFloat(1);
			// background green
			mParameterBag->controlValues[6] = y;
			// green
			mParameterBag->controlValues[2] = x;
		}
		else
		{
			console() << "OSC message received: " << oscAddress << std::endl;
			for (int i = 0; i < message.getNumArgs(); i++) {
				cout << "-- Argument " << i << std::endl;
				cout << "---- type: " << message.getArgTypeName(i) << std::endl;
				if (message.getArgType(i) == osc::TYPE_INT32) {
					try {
						//cout << "------ int value: "<< message.getArgAsInt32(i) << std::endl;
						if (i == 0)
						{
							arg0 = message.getArgAsInt32(i);
							oscArg0 = toString(message.getArgAsInt32(i));
						}
						if (i == 1)
						{
							arg1 = message.getArgAsInt32(i);
							oscArg1 = toString(message.getArgAsInt32(i));
						}
						if (i == 2)
						{
							arg2 = message.getArgAsInt32(i);
							oscArg2 = toString(message.getArgAsInt32(i));
						}
						if (i == 3)
						{
							arg3 = message.getArgAsInt32(i);
							oscArg3 = toString(message.getArgAsInt32(i));
						}
					}
					catch (...) {
						cout << "Exception reading argument as int32" << std::endl;
					}
				}
				else if (message.getArgType(i) == osc::TYPE_FLOAT) {
					try {
						cout << "------ float value: " << message.getArgAsFloat(i) << std::endl;
						if (i == 0)
						{
							oscArg0 = toString(message.getArgAsFloat(i));
						}
						if (i == 1)
						{
							oscArg1 = toString(message.getArgAsFloat(i));
						}
					}
					catch (...) {
						cout << "Exception reading argument as float" << std::endl;
					}
				}
				else if (message.getArgType(i) == osc::TYPE_STRING) {
					try {
						cout << "------ string value: " << message.getArgAsString(i).c_str() << std::endl;
						if (i == 0)
						{
							oscArg0 = message.getArgAsString(i).c_str();
						}
						if (i == 1)
						{

							oscArg1 = message.getArgAsString(i).c_str();
						}
					}
					catch (...) {
						cout << "Exception reading argument as string" << std::endl;
					}
				}
			}
			unsigned found = oscAddress.find_last_of("/");
			int name = atoi(oscAddress.substr(found + 1).c_str());
			if (name > 0)	oscProcessMessage(name, arg0, arg1);
		}
		string oscString = "osc from:" + message.getRemoteIp() + " adr:" + oscAddress + " 0: " + oscArg0 + " 1: " + oscArg1;
		//mUserInterface->labelOSC->setName( oscString );
		mParameterBag->OSCMsg = oscString;
		//stringstream oscString; 
		//oscString << "osc address: " << oscAddress << " oscArg0: " << oscArg0  << " oscArg1: " << oscArg1;
		//oscStatus->setLabel( oscString );
	}
}
void OSC::oscProcessMessage(int controlName, int arg0, int arg1)
{
	float normalizedValue = lmap<float>(arg0, 0.0, 127.0, 0.0, 1.0);

	if (arg0 < 0) arg0 = 0;
	if (arg1 < 0) arg1 = 0;
	if (arg0 > 4096) arg0 = 4096;
	if (arg1 > 4096) arg1 = 4096;

	switch (controlName)
	{
		//pad
	case 0:
		//mParameterBag->mRenderResoXY.x = lmap<float>(arg0, 0, 127, 0.0, 4096.0);
		//mParameterBag->mRenderResoXY.y = lmap<float>(arg1, 0, 127, 0.0, 4096.0);
		//pad->setValue(mMousePos);
		break;
	default:
		mParameterBag->controlValues[controlName] = normalizedValue;
		break;
	}
}

void OSC::rotaryChange(int name, float newValue)
{
	switch (name)
	{
	case 14:
		mParameterBag->controlValues[19] = (newValue*127.0 - 63.0) / 63.0;
		break;
	default:
		mParameterBag->controlValues[name] = newValue;
		break;
	}

}
void OSC::toggleChange(int name, float newValue)
{
	if (newValue > 1.0) newValue = 1.0; // for boolean in frag
	mParameterBag->controlValues[name] = newValue;
}
void OSC::sendOSCIntMessage(string controlType, int controlName, int controlValue0, int controlValue1)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addIntArg(controlName);
	m.addIntArg(controlValue0);
	mOSCSender.sendMessage(m);
}
void OSC::sendOSCStringMessage(string controlType, string controlString)
{
	osc::Message m;
	m.setAddress(controlType);
	m.addStringArg(controlString);
	mOSCSender.sendMessage(m);
}