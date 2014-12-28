#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Json.h"
#include "cinder/Xml.h"

// webcam
#include "cinder/Capture.h"
#include "cinder/Surface.h"
#include "cinder/MayaCamUI.h"
// fonts
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta {

	typedef std::shared_ptr<class ParameterBag> ParameterBagRef;

	class ParameterBag
	{
	public:
		ParameterBag();
		static ParameterBagRef create();

		bool save();
		bool restore();
		void reset();

		std::string					mOSCDestinationHost;
		int							mOSCDestinationPort;
		int							mOSCReceiverPort;
		std::string					OSCMsg;

		// audio
		float						*mData;
		float						maxVolume;
		bool						mUseLineIn;
		float						mAudioMultFactor;
		float						iFreqs[4];


		// OSC/MIDI/JSON controlled UI and params
		map<int, float>				controlValues;

	};

}
