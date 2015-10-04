#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class QuadOSCApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void QuadOSCApp::setup()
{
}

void QuadOSCApp::mouseDown( MouseEvent event )
{
}

void QuadOSCApp::update()
{
}

void QuadOSCApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( QuadOSCApp, RendererGl )
