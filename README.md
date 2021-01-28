# iPhoneXBeeArduino
This project uses the iPhone app TouchOSC, sending OSC messages based on the x y and z
positions of the iPhone through wifi to a computer running the Cinder app. Cinder has 
libraries to handle incoming osc data and this data is then transformed into serial
bits broadcast to an arduino via xbee radio. The serial bits are then transformed into
pulse width modulation to light up three LEDs depending on the orientation of the 
iphone.

Demonstration:
https://youtu.be/WYWtd4iobU8
