# RFBridge2
This Project uses an esp8266 board as a hub to control RF-Switches via Amazon Echo (Alexa) by emulating a Phillips Hue Bridge.

Status of this dokumentation is incomplete. Parts that still need work are marked by @@todo


# Flashing the release.

The release file is designed to run on D1-Mini-R2, NodeMCU and ESP-01 compatible esp8266 boards. 
Just download the esp8266 flash tool from [github](https://github.com/nodemcu/nodemcu-flasher)
and flash the latest released rfbridge2.bin. 

Check [here](https://github.com/Monarch73/RFBridge2/releases)

Sometimes it might be nessesary to erase the flash storage that is hooked up to you esp8266. In this case I recommend the Esp8266-Download-tool from Expressif.

# Wiring

Connect a [RF-Transmitter Module](https://www.amazon.com/gp/product/B017AYH5G0) to your esp8266 device.
The data-connector should be connected to D4 (GPIO2). If you are using a different board, you need to look up GPIO2 in the pinout chart of your board and recompile the project for that board.

To use IR-Functionality, connect the annode of an IR-LED to RX (GPIO3) and cathode through a resistor to GND. The value of the resistor is dependent on the forward-voltage of your IR-Led and the output voltage of your GPIOs. In any case 220 Ohms should be working ok.

# Usage

The first time rfbridge2 runs on your esp8266, it starts off acting as an AP named "EasyAlexa". Connect to it and open a browser to open http://192.168.4.1
You will be asked to enter a ESSID and Password. Optionaly you can also format and reinitialize SPIFFS storage. This is the area where RFBridge stores configuration data permanently. Click Save and reset the device.

The esp8266 will now connect to the network. To identify the IP of your device, You can either use a network scanner (ie "Net Scan" for Android Devices) or a usp port monitor.
Browse to the ip of your device.

Add a switch
In the "Switch Configuration" section, specify a name for the switch. This name will be recognized by the Echo later on so it's best to specify a distinct name.

There are 4 ways to control devices that RFBridge2 supports:

Option 1.) Remote Control Sockets with DIP-Switch configuration

RF-Switches usaly have 10 dip-switches. The first 5 switches specify a "housecode" and the last 5 specify the device Code. The position of the switches need to be translated to 0 (=off position) and 1 (=on postion) IE.

Housecode    00100
Code         10011

Option  2) 
if your RF Switches don't have dip-switches, you can specify so called "TriState Codes". These TriState Codes can be taken from the output of rc-switch/AdvancedReceiveDemo-example. You need a receiver-module for this procedure.

option 3)
if you wan't to trigger other devices on your smarthome network, your can specify URLs that will be fired when alexa triggers a switch.

Option 4) Infrared Control
this option needs a Infrared LED wired up to your esp8266 module as described above. A standard LED might work too. This function leverages the sendRaw()-Method of the IRRemote-Library taken from https://github.com/markszabo/IRremoteESP8266 . 

Please note, you can only use one option at a time! If you configured more than one option, RFBridge2 might not behave as you'd expect.
 
Hit Save and start the device recover function in the alexa app. The above configured device should be discovered.


# Setting up development environment.

##  Compile in Arduino Studio
- Requirements
	- Arduino IDE 1.8.x (https://www.arduino.cc/en/Main/Software)
	- Python 2.7!
	- Git (git for windows)
	
- Directions

Clone repository recursivly
```
git clone --recursive https://github.com/Monarch73/RFBridge2.git
```

execute get.py in hardware/esp8266/esp8266/tools

```bash
cd hardware/esp8266/esp8266/tools
python get.py
```
Start Arduino IDE and configure the newly cloned "RFBridge2" directory as your sketchbook-folder in settings. (ctrl ,). Also please remove the urls for the boardmanager that you might have added previously. RFBridge2 doesn't require these settings. Hit ok and ignore popup about availability of updates for libraries. Don't update! The versions of the submodules are release verions.

Now you should be ready open RFBridge2.ino, compile and flash it.

## Visual Studio Community 2017

Requirements for Windows users

- [Visual Studio 2017](https://www.visualstudio.com/vs/whatsnew/) Any Edtition will do. Visual Studio 2017 Community is for free for individuals.
- Make sure you have the c++ and git extension enabled in the Visual Studio Installer (can be launched any time from the start menu).
- [Visual Micro extension](http://www.visualmicro.com/page/Arduino-Visual-Studio-Downloads.aspx)
- [git for windows](https://git-scm.com/download/win)
- python2.7 for windows. I recommend using [chocolaty](https://chocolatey.org/install) to install the python2-package for windows.

Setup the project

- recursivly clone the project and all submodules to an empty directory (git clone --recursive https://github.com/Monarch73/RFBridge2.git)
- change the directory to RFBridge2\hardware\esp8266\esp8266\tools directory and run the get.py-file

```sh
c:
cd \
mkdir work
cd work
git clone --recursive https://github.com/Monarch73/RFBridge.git
cd RFBridge\hardware\esp8266\esp8266\tools
python get.py
```
- launch Visual Studio 2017
- goto "tools->visualmicro->Configure arduiono ide locations"
- select "Visual Micro(noide)" from drop down
- in "sketchbook-locations" enter the directory where you cloned the repository (c:\work\RFBridge);
- click "OK"
- select "tools->visualstudio->rescan toolchains and libraries".

Visual Studio should now be prepared to open the solution file (.sln) and compile the project.
