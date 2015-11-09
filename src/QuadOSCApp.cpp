#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "OscListener.h"
#include "OscSender.h"
#include "cinder/Serial.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include <string>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <math.h>


using namespace ci;
using namespace ci::app;
using namespace std;

#define READ_INTERVAL .01

// #define TEST_PACKET 7E 00 0E 00 01 00 7D 33 A2 00 40 C3 44 DA 00 (6E 4E 6E) FE
// parentheses enclose the real message (RF Data)

class QuadOSCApp : public AppNative {
  public:
	void setup();
	void update();
	void draw();
    
    osc::Listener mOscListener;
    Vec2f mOscPosition;
    
    Serial serial;
    
    uint8_t outBuffer[61] {126,0,56,0,1,0,125,51,162,0,64,195,68,218,0,60,119,62,48,46,48,56,55,52,54,53,60,47,119,62,60,120,62,48,46,48,56,55,52,54,53,60,47,120,62,60,121,62,48,46,48,56,55,52,54,53,60,47,121,62, 27};
    
// packet 7E 00 38 00 01 00 7D 33 A2 00 40 C3 44 DA 00 3C 77 3E 30 2E 30 38 37 34 36 35 3C 2F 77 3E 3C 78 3E 30 2E 30 38 37 34 36 35 3C 2F 78 3E 3C 79 3E 30 2E 30 38 37 34 36 35 3C 2F 79 3E 1B
    
    //beginning of packet
   // uint8_t outBuffer[76] {126,0,71,0,1,0,125,51,162,0,64,195,68,218,0};
    
    double sinceLastRead, lastUpdate;

    std::string outData;
    std::string strToChars(std::string str, int motor);
    uint8_t wDec[8], xDec[8], yDec[8];
    uint8_t checkSum(u_int8_t* buffer);

};

void QuadOSCApp::setup()
{
    mOscListener.setup(8001);
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
    while (mOscListener.hasWaitingMessages())
    {
        osc::Message msg;
        mOscListener.getNextMessage(&msg);

        if (msg.getNumArgs() == 3)
        {
            std::string wStr (msg.getArgAsString(0, TRUE));
            std::string xStr (msg.getArgAsString(1, TRUE));
            std::string yStr (msg.getArgAsString(2, TRUE));
            
            console() << wStr << endl;
            console() << xStr << endl;
            console() << yStr << endl;
            
            QuadOSCApp::strToChars(wStr, 1);
            QuadOSCApp::strToChars(xStr, 2);
            QuadOSCApp::strToChars(yStr, 3);
            
            // override debug placeholders 0.087465 with osc data
            outBuffer[18] = wDec[0];
            outBuffer[19] = wDec[1];
            outBuffer[20] = wDec[2];
            outBuffer[21] = wDec[3];
            outBuffer[22] = wDec[4];
            outBuffer[23] = wDec[5];
            outBuffer[24] = wDec[6];
            outBuffer[25] = wDec[7];
            
            outBuffer[33] = xDec[0];
            outBuffer[34] = xDec[1];
            outBuffer[35] = xDec[2];
            outBuffer[36] = xDec[3];
            outBuffer[37] = xDec[4];
            outBuffer[38] = xDec[5];
            outBuffer[39] = xDec[6];
            outBuffer[40] = xDec[7];
            
            outBuffer[48] = yDec[0];
            outBuffer[49] = yDec[1];
            outBuffer[50] = yDec[2];
            outBuffer[51] = yDec[3];
            outBuffer[52] = yDec[4];
            outBuffer[53] = yDec[5];
            outBuffer[54] = yDec[6];
            outBuffer[55] = yDec[7];
           
            //checksum
            outBuffer[60] = QuadOSCApp::checkSum(outBuffer);
        }
        
        if(msg.getNumArgs() == 1)
        {
            std::string wStr (msg.getArgAsString(0, TRUE));
            QuadOSCApp::strToChars(wStr, 1);
            
            outBuffer[18] = wDec[0];
            outBuffer[19] = wDec[1];
            outBuffer[20] = wDec[2];
            outBuffer[21] = wDec[3];
            outBuffer[22] = wDec[4];
            outBuffer[23] = wDec[5];
            outBuffer[24] = wDec[6];
            outBuffer[25] = wDec[7];
            
            outBuffer[33] = wDec[0];
            outBuffer[34] = wDec[1];
            outBuffer[35] = wDec[2];
            outBuffer[36] = wDec[3];
            outBuffer[37] = wDec[4];
            outBuffer[38] = wDec[5];
            outBuffer[39] = wDec[6];
            outBuffer[40] = wDec[7];
            
            outBuffer[48] = wDec[0];
            outBuffer[49] = wDec[1];
            outBuffer[50] = wDec[2];
            outBuffer[51] = wDec[3];
            outBuffer[52] = wDec[4];
            outBuffer[53] = wDec[5];
            outBuffer[54] = wDec[6];
            outBuffer[55] = wDec[7];
            
            //checksum
            outBuffer[60] = QuadOSCApp::checkSum(outBuffer);
        }
        
     //   console() << hex << outBuffer[60] << std::endl;
        
        double now = getElapsedSeconds();
        double deltaTime = now - lastUpdate;
        lastUpdate = now;
        sinceLastRead += deltaTime;
        
        if(sinceLastRead > READ_INTERVAL)
        {
            /*reset the last read time */
            
            sinceLastRead = 0.0;
            
            /* Serial data written here */
            
            serial.writeBytes(outBuffer,61);
        }
    }
}

void QuadOSCApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}


std::string QuadOSCApp::strToChars(string str, int motor)
{
    int count = 0;
    for (std::string::iterator it=str.begin(); it!=str.end(); ++it)
    {
        switch ( motor )
        {
            case 1:
                wDec[count] = *it;
                break;
            case 2:
                xDec[count] = *it;
                break;
            case 3:
                yDec[count] = *it;
                break;
            default:
                console() << "no case registered";
        }
        ++count;
    }
    return str;
}

u_int8_t QuadOSCApp::checkSum(u_int8_t* buffer)
{
    unsigned long sum = 0;
    for ( int i=3; i<60; i++ ) //all bytes in packet except XBee API frame delimiter & size = final checkSum byte
    {
        unsigned long number = outBuffer[i];
        sum = (sum + number);
    }

    sum = 0xff - ((sum - 0x9d) % 256);
    
    return sum;
}

CINDER_APP_NATIVE( QuadOSCApp, RendererGl )
