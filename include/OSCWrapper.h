#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Utilities.h"

#include "OscListener.h"
#include "OSCSender.h"
#include "Resources.h"
#include "ParameterBag.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace Reymenta 
{

	typedef std::shared_ptr<class OSC> OSCRef;

	class OSC 
	{

	public:
		OSC( ParameterBagRef aParameterBag );
		static	OSCRef create( ParameterBagRef aParameterBag );

		void	update();

		void	sendOSCIntMessage( string controlType, int controlName, int controlValue0, int controlValue1 );
		void	sendOSCStringMessage( string controlType, string controlString );
		void	rotaryChange( int name, float newValue );
		void	toggleChange(int name, float newValue);
		Vec4i	skeleton[20];
	private:

		void oscProcessMessage( int controlName, int arg0, int arg1 );
		// parameters
		ParameterBagRef mParameterBag;
		osc::Listener 				mOSCReceiver;
		osc::Sender					mOSCSender;

	};
}