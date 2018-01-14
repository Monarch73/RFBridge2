# RFBridge2
RFBridge with Phillips Hue Emulation

# Compile in Arduino Studio
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
Start Arduino IDE and configure the newly cloned "RFBridge2" directory as your sketchbook-folder in settings. (ctrl ,). Open RFBridge2.ino. Ignore popup about availability of updates for libraries. Don't update! The versions of the submodules are release verions. 

done
