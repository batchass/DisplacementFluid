#include "DisplacementFluidApp.h"

void msaFluidParticlesApp::reymentaSetup()
{
	mBackgroundColor = ColorAf( 0.0f, 0.0f, 0.0f, 0.0f );
	mDisplayCount = 0;
	mRenderY = 0;
	for (auto display : Display::getDisplays() )
	{
		mDisplayCount++;
		std::cout << "Width:" << display->getWidth() << "\n"; 
		std::cout << "Height:" << display->getHeight() << "\n"; 
		mRenderWidth = display->getWidth();
		mRenderHeight = display->getHeight();

		//mRenderX mRenderY mRenderWidth mRenderHeight
	}
	std::cout << "Display Count:" << mDisplayCount << "\n";
	if ( mDisplayCount == 1 )
	{
		mRenderX = mMainDisplayWidth / 2;
		mRenderWidth /= 2;
		mRenderHeight /= 2;
	}
	else
	{
		mRenderX = mMainDisplayWidth;
	}
	setWindowSize( mRenderWidth, mRenderHeight );

	WindowRef rWin = app::getWindow();
	rWin->setPos(mRenderX, mRenderY);
	rWin->setBorderless();
	rWin->setAlwaysOnTop();

	HWND hWnd = (HWND)rWin->getNative();

	HRESULT hr = S_OK;
	// Create and populate the Blur Behind structure
	DWM_BLURBEHIND bb = {0};

	// Enable Blur Behind and apply to the entire client area
	bb.dwFlags = DWM_BB_ENABLE;
	bb.fEnable = true;
	bb.hRgnBlur = NULL;

	// Apply Blur Behind
	hr = DwmEnableBlurBehindWindow(hWnd, &bb);
	if (SUCCEEDED(hr))
	{
		HRESULT hr = S_OK;

		// Set the margins, extending the bottom margin.
		MARGINS margins = {-1};

		// Extend the frame on the bottom of the client area.
		hr = DwmExtendFrameIntoClientArea(hWnd,&margins);
	}
	receiver.setup( 10000 );
}

void msaFluidParticlesApp::setup()
{
	//console() << "ciMSAFluid Demo | (c) 2009 Mehmet Akten | www.memo.tv" << std::endl;
	reymentaSetup();
	// setup fluid stuff
	fluidSolver.setup(100, 100);
	fluidSolver.enableRGB(true).setFadeSpeed(0.004).setDeltaT(0.5).setVisc(0.00019).setColorDiffusion(0);
	fluidDrawer.setup( &fluidSolver );
	particleSystem.setFluidSolver( &fluidSolver );

	fluidCellsX			= 150;

	drawFluid			= true;
	drawParticles		= true;
	renderUsingVA		= true;

	width				= 1024;
	height				= 768;

	setFrameRate( 60.0f );
	setWindowSize(width, height);

	pMouse = getWindowCenter();
	resizeFluid			= true;

	gl::enableAlphaBlending();

	gl::Fbo::Format format;
	format.setSamples( 8 );
	//format.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	mFbo = gl::Fbo(width, height, format);

	drawFluidTex = false;

	loadShader();
}

void msaFluidParticlesApp::update()
{
	while( receiver.hasWaitingMessages() ) {
		osc::Message m;
		receiver.getNextMessage( &m );

		console() << "New message received" << std::endl;
		console() << "Address: " << m.getAddress() << std::endl;
		console() << "Num Arg: " << m.getNumArgs() << std::endl;
		// check for mouse moved message
		if(m.getAddress() == "/mouse/position"){
			// both the arguments are int32's
			Vec2i pos = Vec2i( m.getArgAsInt32(0), m.getArgAsInt32(1));
			Vec2f mouseNorm = Vec2f( pos ) / getWindowSize();
			Vec2f mouseVel = Vec2f( pos - pMouse ) / getWindowSize();
			addToFluid( mouseNorm, mouseVel, true, true );
			pMouse = pos;
			if ( m.getArgAsInt32(2) == 1 )
			{
				mMouseDown = true;
			}
			else
			{
				mMouseDown = false;
			}
			if ( mMouseDown )
			{
				mArcball.mouseDown( pos );
				mCurrentMouseDown = mInitialMouseDown = pos;
			}
		}
		// check for mouse button message
		else if(m.getAddress() == "/mouse/button"){
			// the single argument is a string
			Vec2i pos = Vec2i( m.getArgAsInt32(0), m.getArgAsInt32(1));
			mArcball.mouseDown( pos );
			mCurrentMouseDown = mInitialMouseDown = pos;
			if ( m.getArgAsInt32(2) == 1 )
			{
				mMouseDown = true;
			}
			else
			{
				mMouseDown = false;
			}
		}
		else if(m.getAddress() == "/fluid/drawfluid"){
			drawFluid = !drawFluid;
		}
		else if(m.getAddress() == "/fluid/drawfluidtex"){
			drawFluidTex = !drawFluidTex;
		}
		else if(m.getAddress() == "/fluid/drawparticles"){
			drawParticles = ! drawParticles;
		}
		else if(m.getAddress() == "/fluid/randomizecolor"){
			fluidSolver.randomizeColor();
		}
		else if(m.getAddress() == "/window/position"){
			// window position
			app::setWindowPos(m.getArgAsInt32(0), m.getArgAsInt32(1));
		}
		else if(m.getAddress() == "/window/setfullscreen"){
			// fullscreen
			setFullScreen( ! isFullScreen() );
		}		
		else if(m.getAddress() == "/quit"){
			quitProgram();
		}		
		else{
			// unrecognized message
			//cout << "not recognized:" << m.getAddress() << endl;

		}

	}

	if( resizeFluid ) {
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}

	fluidSolver.update();

	renderSceneToFbo();
}

void msaFluidParticlesApp::draw()
{
	gl::clear( mBackgroundColor );
	//gl::clear( ColorA(45,48,20));
	CameraPersp cam( getWindowWidth(), getWindowHeight(), 60.0f );
	cam.setPerspective( 60, getWindowAspectRatio(), 1, 1500 );
	cam.lookAt( Vec3f( 2.6f, 1.6f, -2.6f ), Vec3f::zero() );
	gl::setMatrices( cam );
	gl::rotate( mArcball.getQuat() );

	// set the viewport to match our window
	gl::setViewport( getWindowBounds() );

	gl::enableDepthRead();
	gl::enableDepthWrite();

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK);

	//mFbo.bindTexture();
	gl::Texture txFluid = mFbo.getTexture(0);


	mShader->bind();
	txFluid.enableAndBind();
	mShader->uniform( "displacementMap", 0 );
	mShader->uniform( "colorMap", 0 );
	gl::drawSphere(Vec3f::zero(), 1.0, 254);
	txFluid.unbind();
	mShader->unbind();
	//mFbo.unbindTexture();

	gl::disable(GL_CULL_FACE);
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::setMatricesWindow( getWindowSize());
	if(drawFluidTex){
		glEnable( GL_TEXTURE_2D );
		txFluid.setFlipped(true);
		gl::draw( txFluid, Rectf( 0, 0, 256, 256));
		glDisable(GL_TEXTURE_2D);
	}
}


void msaFluidParticlesApp::resize()
{
	particleSystem.setWindowSize( app::getWindowSize());
	width  = app::getWindowWidth();
	height = app::getWindowHeight();

	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 150 );

	resizeFluid = true;
}

void msaFluidParticlesApp::keyDown( KeyEvent event )
{ 
	switch( event.getChar() ) {
	case 'a':
		drawFluid = !drawFluid;
		break;
	case 'd':
		drawFluidTex = !drawFluidTex;
		break;
	case 'f':
		setFullScreen( ! isFullScreen() );
		break;
	case ' ':
		fluidSolver.randomizeColor();
		break;
	case 'p':
		drawParticles = ! drawParticles;
		break;
	case 'b': {
		Timer timer;
		timer.start();
		const int ITERS = 1000;
		for( int i = 0; i < ITERS; ++i )
			fluidSolver.update();
		timer.stop();
		console() << ITERS << " iterations took " << timer.getSeconds() << " seconds." << std::endl;
			  }
			  break;
	case 's':
		writeImage( getHomeDirectory() / "cinder" / "saveImage_" / ( toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
		break;
	}
}

void msaFluidParticlesApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown( event.getPos() );
	mCurrentMouseDown = mInitialMouseDown = event.getPos();
}

void msaFluidParticlesApp::mouseMove( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() ) / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = event.getPos();
}

void msaFluidParticlesApp::mouseDrag( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() ) / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, false, true );
	pMouse = event.getPos();

	mArcball.mouseDrag( event.getPos() );
	mCurrentMouseDown = event.getPos();
}

void msaFluidParticlesApp::loadShader()
{
	try {
		mShader = gl::GlslProg::create( loadResource( RES_DISPLACEMENT_VERT ), loadResource( RES_DISPLACEMENT_FRAG ) );
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
}

void msaFluidParticlesApp::renderSceneToFbo()
{
	mFbo.bindFramebuffer();

	if( drawFluid ) {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, getWindowWidth(), getWindowHeight());
	}
	if( drawParticles )
		particleSystem.updateAndDraw( drawFluid );

	mFbo.unbindFramebuffer();
}

void msaFluidParticlesApp::fadeToColor( float r, float g, float b, float speed )
{
	glColor4f( r, g, b, speed );
	gl::drawSolidRect( getWindowBounds() );
}
void msaFluidParticlesApp::quitProgram()
{
	quit();
}
// add force and dye to fluid, and create particles
void msaFluidParticlesApp::addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce )
{
	float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
	if( speed > 0 ) {
		pos.x = constrain( pos.x, 0.0f, 1.0f );
		pos.y = constrain( pos.y, 0.0f, 1.0f );

		const float colorMult = 100;
		const float velocityMult = 30;

		if( addColor ) {
			Color drawColor( CM_HSV, ( getElapsedFrames() % 360 ) / 360.0f, 1, 1 );

			fluidSolver.addColorAtPos( pos, drawColor * colorMult );

			if( drawParticles )
				particleSystem.addParticles( pos * Vec2f( getWindowSize() ), 10 );
		}

		if( addForce )
			fluidSolver.addForceAtPos( pos, vel * velocityMult );

		if( ! drawFluid && getElapsedFrames()%5==0 )
			fadeToColor( 0, 0, 0, 0.1f );
	}
}

CINDER_APP_BASIC( msaFluidParticlesApp, RendererGl )
