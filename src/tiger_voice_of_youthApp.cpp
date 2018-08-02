#include "cinder/app/AppNative.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Font.h"
#include "cinder/app/AppImplMsw.h"
#include "boost/thread/mutex.hpp"

#if defined(WIN32) && !defined(_DEBUG)
#include "AppVerify.h"
#endif

#include "Config.h"
#include "Debug.h"
#include "BitException.h"
#include "BitExceptionHandler.h"
#include "BitShortcutKey.h"

#include "AClass.h"
#include "mainPage.h"

#include "Spout.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class tiger_voice_of_youthApp : public AppNative {
public:
	void prepareSettings(Settings *settings);
	void setup();
	void shutdown();
	void mouseDown(MouseEvent event);
	void keyDown(KeyEvent event);
	void update();
	void draw();
	void emitClose();

private:
#if defined(WIN32) && !defined(_DEBUG)
	AppVerify  appVerify_;
#endif
	void toggleFullscreen();
	void toggleDisplayParameters();
	void checkExceptionFromThread();

	Bit::Config config_;
	bool      borderless_;

	// debugging values
	bool      debug_;
	bool      paramsOn_;
	Bit::ShortcutKey shortcutKey_;
	bool setupCompleted_;

	mainPage mp_;

	AClass aClass_;
};

void tiger_voice_of_youthApp::prepareSettings(Settings *settings)
{
#if defined(WIN32) && !defined(_DEBUG)
	//if( !appVerify_.isValid() ) {
	//	showCursor();
	//	MessageBox( NULL, L"Application has expired.", L"ERROR", MB_OK );	
	//	exit( 1 );
	//}
#endif

	// initialize debugging values
	debug_ = false;
	paramsOn_ = false;
	setupCompleted_ = false;

	try {
		// register shutdown function to exception handler
		Bit::ExceptionHandler::registerShutdownFunction(std::bind(&tiger_voice_of_youthApp::emitClose, this));
		// read settings for window setup
		config_.readConfig();
		Bit::Config::DisplayConfig appConfig = config_.getDisplayConfig();
		// setup window 
		settings->setWindowSize(appConfig.windowSize.x, appConfig.windowSize.y);
		settings->setWindowPos(appConfig.windowPos.x, appConfig.windowPos.y);
		settings->setAlwaysOnTop(appConfig.alwaysOnTop);
		settings->setBorderless(appConfig.borderless);
		borderless_ = appConfig.borderless;
		// setup cursor
		if (appConfig.hideCursor)
			//hideCursor();
			showCursor();
		else
			showCursor();
	}
	catch (std::exception& e) {
		Bit::ExceptionHandler::handleException(&e);
	}
}

void tiger_voice_of_youthApp::setup()
{
	// setup shortcut keys
	shortcutKey_.setupDisplayDialog("Shortcut list", Vec2i(400, 200), ColorA(0.3f, 0.3f, 0.3f, 0.4f));
	shortcutKey_.addShortcut(KeyEvent::KEY_ESCAPE, std::bind(&tiger_voice_of_youthApp::emitClose, this), "close application");
	shortcutKey_.addShortcut('d', &debug_, "toggle display debug mode");
	shortcutKey_.addShortcut('f', std::bind(&tiger_voice_of_youthApp::toggleFullscreen, this), "toggle fullscreen mode");
	shortcutKey_.addShortcut('p', std::bind(&tiger_voice_of_youthApp::toggleDisplayParameters, this), "toggle display parameters dialog");
	shortcutKey_.addShortcut(KeyEvent::KEY_F1, std::bind(&Bit::ShortcutKey::toggleDisplay, &shortcutKey_), "toggle display shortcut keys list");

	try {
		config_.setup();

		// setup everything here
		//config_.readConfigurableConfig(aClass_, "aClassConfig");	// this will eventually calls AClass::readConfig() with the Bit::JsonTree* node named "aClassConfig" as argument
		//config_.readConfigurableParams(aClass_, "aClassParams");	// this will eventually calls AClass::readParams() with the Bit::JsonTree* node named "aClassParams" as argument

		// mark setup complete at the end of setup.

		config_.readConfigurableConfig(mp_, "mainPage");
		config_.readConfigurableParams(mp_, "mainPage");
		mp_.setup();

		setupCompleted_ = true;
	}
	catch (std::exception& e) {
		Bit::ExceptionHandler::handleException(&e);
	}
}

void tiger_voice_of_youthApp::emitClose()
{
	// if setup is donw (we have window), emit the same event like clicking windows' close button (X button) on upper right corner
	// TODO: we need to handle multiple windows later
	if (setupCompleted_){
		WindowImplMsw* impl = reinterpret_cast<WindowImplMsw*>(::GetWindowLongPtr((HWND)ci::app::getWindow()->getNative(), GWLP_USERDATA));
		impl->getWindow()->emitClose();
		impl->privateClose();
		delete impl;
		// quit will call shutdown() for clean up and close the app
		quit();
	}
	else{	// otherwise, simply exit
		exit(Bit::Exception::getExitCode());
	}
}

void tiger_voice_of_youthApp::shutdown()
{
	//int exitCode = Bit::Exception::getExitCode();
	//exit( exitCode );	// we can not exit() here as memory leaks will occur
	mp_.shutdown();
}

void tiger_voice_of_youthApp::toggleFullscreen()
{
	setFullScreen(!isFullScreen());
}

void tiger_voice_of_youthApp::toggleDisplayParameters()
{
	paramsOn_ = !paramsOn_;
	if (paramsOn_) {
		showCursor();

		config_.showParams();
	}
	else {
		hideCursor();

		config_.hideParams();
	}
}

void tiger_voice_of_youthApp::keyDown(KeyEvent event)
{
	shortcutKey_.keyDown(event);
}

void tiger_voice_of_youthApp::mouseDown(MouseEvent event)
{
}

void tiger_voice_of_youthApp::update()
{
	try {
		// check if there is any exception from thread, for more info see Bit::ExceptionHandler::checkExceptionFromThread
		Bit::ExceptionHandler::checkExceptionFromThread();

		// added update part here
		showCursor();
		mp_.update();

	}
	catch (std::exception& e) {
		Bit::ExceptionHandler::handleException(&e);
	}
}

void tiger_voice_of_youthApp::draw()
{
	if (!setupCompleted_)
		return;

	// clear out the window with black
	ci::gl::clear(ColorA(0.0, 0.0, 0.0,0.0));

	// draw everything here

	mp_.draw();

	// all debugging things 

	if (debug_) {
		// draw fps
		ci::gl::drawString(toString(getAverageFps()), Vec2f(10, 10), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	}

	if (paramsOn_) {
		config_.drawParams();
	}

	// draw all shortcut keys to dialog
	shortcutKey_.draw();
}

CINDER_APP_NATIVE(tiger_voice_of_youthApp, RendererGl)
