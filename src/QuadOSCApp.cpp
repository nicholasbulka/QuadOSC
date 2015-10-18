#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "OscListener.h"
#include "OscSender.h"
#include "cinder/Serial.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include <string>
#include <boost/lexical_cast.hpp>


using namespace ci;
using namespace ci::app;
using namespace std;

#define READ_INTERVAL 5.25

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
    
    std::string apiHexA = "7E 00 35 00 01 00 7D 33 A2 00 40 C3 44 DA 00";
    std::string apiHexB = "";
    std::string checkSum = "";
    
    //7E 00 35 00 01 00 7D 33 A2 00 40 C3 44 DA 00 3C 78 3E 30 2E 35 3C 2F 78 3E 3C 79 3E 30 2E 38 3C 2F 79 3E 3C 7A 3E 2D 30 2E 34 3C 2F 7A 3E 3C 77 3E 30 2E 30 3C 2F 77 3E 0A 58//
    uint8_t outBuffer[58]{126,0,53,0,1,0,125,51,162,0,64,195,68,218,0,60,120,62,48,46,53,60,47,120,62,60,121,62,48,46,56,60,47,121,62,60,122,62,45,48,46,52,60,47,122,62,60,119,62,48,46,48,60,47,119,62,10,88};
    
    //beginning of packet
  //  uint8_t outBuffer[75] {126,0,75,0,1,0,125,51,162,0,64,195,68,218,0};
    
    double sinceLastRead, lastUpdate;
  // void updateBuffer(char* outBuffer, int length);
  //  int charToInt(char digit);
    std::string outData;
    std::string strToChars(std::string str, int motor);
    int wDec[7], xDec[7], yDec[7], zDec[7];

};

void QuadOSCApp::setup()
{
  /*  // <
    outBuffer[15] = 60;
    // w
    outBuffer[16] = 119;
    // >
    outBuffer[17] = 62;
    // w debug placeholders 0.087465
    outBuffer[18] = 60;
    outBuffer[19] = 46;
    outBuffer[20] = 60;
    outBuffer[21] = 56;
    outBuffer[22] = 55;
    outBuffer[23] = 52;
    outBuffer[24] = 54;
    outBuffer[25] = 53;
    
    // <
    outBuffer[26] = 60;
    // /
    outBuffer[27] = 47;
    // w
    outBuffer[28] = 119;
    // >
    outBuffer[29] = 62;
    
    // <
    outBuffer[30] = 60;
    // x
    outBuffer[31] = 120;
    // >
    outBuffer[32] = 62;
    // x debug placeholders 0.087465
    outBuffer[33] = 60;
    outBuffer[34] = 46;
    outBuffer[35] = 60;
    outBuffer[36] = 56;
    outBuffer[37] = 55;
    outBuffer[38] = 52;
    outBuffer[39] = 54;
    outBuffer[40] = 53;
    
    // <
    outBuffer[41] = 60;
    // /
    outBuffer[42] = 47;
    // w
    outBuffer[43] = 119;
    // >
    outBuffer[44] = 62;
    
    // <
    outBuffer[45] = 60;
    // y
    outBuffer[46] = 121;
    // >
    outBuffer[47] = 62;
    // y debug placeholders 0.087465
    outBuffer[48] = 60;
    outBuffer[49] = 46;
    outBuffer[50] = 60;
    outBuffer[51] = 56;
    outBuffer[52] = 55;
    outBuffer[53] = 52;
    outBuffer[54] = 54;
    outBuffer[55] = 53;
    
    // <
    outBuffer[56] = 60;
    // /
    outBuffer[57] = 47;
    // w
    outBuffer[58] = 119;
    // >
    outBuffer[59] = 62;
    
    // <
    outBuffer[60] = 60;
    // z
    outBuffer[61] = 120;
    // >
    outBuffer[62] = 62;
    // z debug placeholders 0.087465
    outBuffer[63] = 60;
    outBuffer[64] = 46;
    outBuffer[65] = 60;
    outBuffer[66] = 56;
    outBuffer[67] = 55;
    outBuffer[68] = 52;
    outBuffer[69] = 54;
    outBuffer[70] = 53;
    
    // <
    outBuffer[71] = 60;
    // /
    outBuffer[72] = 47;
    // w
    outBuffer[73] = 119;
    // >
    outBuffer[74] = 104; */
    
    
    
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
    
    //serial.writeByte(ctr);
    
  //  console() << outBuffer << std::endl;
    while (mOscListener.hasWaitingMessages())
    {
        osc::Message msg;
        mOscListener.getNextMessage(&msg);

        if (msg.getNumArgs() == 3)
        {
            std::string wStr (msg.getArgAsString(0, TRUE));
            std::string xStr (msg.getArgAsString(0, TRUE));
            std::string yStr (msg.getArgAsString(1, TRUE));
            std::string zStr (msg.getArgAsString(2, TRUE));
            
            QuadOSCApp::strToChars(wStr, 1);
            QuadOSCApp::strToChars(xStr, 2);
            QuadOSCApp::strToChars(yStr, 3);
            QuadOSCApp::strToChars(zStr, 4);
            
            outData = "<w>" + boost::lexical_cast<std::string>(wDec);
            outData += "</w><x>" + boost::lexical_cast<std::string>(xDec);

            outData += "</x><y>" + boost::lexical_cast<std::string>(yDec);

            outData += "<y><z>"  + boost::lexical_cast<std::string>(zDec);

            outData += "</z>";
        }
        
        if(msg.getNumArgs() == 1)
        {
            outData = "<t>" + msg.getArgAsString(0, TRUE);
            outData += "</t>";
        }
        
        console() << outBuffer << std::endl;
        
        double now = getElapsedSeconds();
        double deltaTime = now - lastUpdate;
        lastUpdate = now;
        sinceLastRead += deltaTime;
        
        if(sinceLastRead > READ_INTERVAL)
        {
            /*reset the last read time */
            
            sinceLastRead = 0.0;
            
            /* Serial data written here */
            
            serial.writeBytes(outBuffer,58);
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
    for (std::string::iterator it=str.begin(); it!=str.end(); ++it)
    {
        int count = 0;
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
            case 4:
                zDec[count] = *it;
                break;
            default:
                console() << "no case registered";
        }
        ++count;
    }
    return str;
}

CINDER_APP_NATIVE( QuadOSCApp, RendererGl )
