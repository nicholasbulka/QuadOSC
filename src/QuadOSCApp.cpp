#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "OscListener.h"
#include "OscSender.h"
#include "cinder/Serial.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define BUFSIZE 80
#define READ_INTERVAL 5.25

class QuadOSCApp : public AppNative {
  public:
	void setup();
	void update();
	void draw();
    
    osc::Listener mOscListener;
    Vec2f mOscPosition;
    
    Serial serial;
    uint8_t			ctr;
    
    double sinceLastRead, lastUpdate;
    
};

void QuadOSCApp::setup()
{
    mOscListener.setup(8001);
    
    ctr = 78;

    sinceLastRead = 0.0;
    lastUpdate = 0.0;
    
    // print the devices
    const vector<Serial::Device> &devices( Serial::getDevices() );
    for( vector<Serial::Device>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
        console() << "Device: " << deviceIt->getName() << endl;
    }
    
    try {
        Serial::Device dev = Serial::findDeviceByNameContains("tty.usbserial");
        serial = Serial( dev, 9600);
        console() << "the try fired" << std::endl;
    }
    catch( ... ) {
        console() << "There was an error initializing the serial device!" << std::endl;
        exit( -1 );
    }
}

void QuadOSCApp::update()
{
    
    //serial.writeByte(ctr);
    
    console() << ctr << std::endl;
    while (mOscListener.hasWaitingMessages())
    {
        osc::Message msg;
        mOscListener.getNextMessage(&msg);
        
        //	console() << "Bytes available: " << serial.getNumBytesAvailable() << std::endl;
        
        double now = getElapsedSeconds();
        double deltaTime = now - lastUpdate;
        lastUpdate = now;
        sinceLastRead += deltaTime;
        
        if(sinceLastRead > READ_INTERVAL)
        {
            /*reset the last read time */
            
            sinceLastRead = 0.0;
            
            /* Serial data written here */
            
            serial.writeByte(ctr);
        }

    }
}

void QuadOSCApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( QuadOSCApp, RendererGl )
