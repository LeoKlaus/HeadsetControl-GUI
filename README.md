A simply remake of [HeadsetControl-GUI](https://github.com/LeoKlaus/HeadsetControl-GUI) by @LeoKlaus
# HeadsetControl-GUI [![Github All Releases](https://img.shields.io/github/downloads/nicola02nb/headsetcontrol-gui/total.svg)]() [![license](https://img.shields.io/github/license/nicola02nb/HeadsetControl-GUI)]()
This is a GUI for [Sapds great HeadsetControl](https://github.com/Sapd/HeadsetControl/).</br>
It's just a frontend to graphically interact with the original HeadsetControl and has no functionality by itself.</br>


## Platforms

OS | Compiled | Tested 
:------------ | :-------------| :-------------
Windows | ✅ |  ✅
Linux | ✅ | ❌
MacOS | ❌ | ❌

If you are on Linux or Mac and try to build the app and test it, I'd be happy to hear if it did or didn't work.

## Installation (Windows)
1. Download the [latest release](https://github.com/nicola02nb/HeadsetControl-GUI/releases/latest/) of HeadsetControl-GUI from the [releases section](https://github.com/nicola02nb/HeadsetControl-GUI/releases) of this page.
2. Extract HeadsetControl-GUI to any folder.

The finished folder should look something like this:

![image](https://github.com/user-attachments/assets/0145ca37-6e59-4170-ba26-804e8856dbc8)

### Usage
Start HeadsetControl-GUI by double-clicking "HeadsetControl-GUI.exe", and if your headset is supported and everything was set up correctly, you will be greeted by the following screen HeadsetControl-GUI has..

If you don't find some features in you ui, probably it's not supported by your headset or it has not been implemented by [HeadsetControl](https://github.com/Sapd/HeadsetControl/).

![image](https://github.com/nicola02nb/HeadsetControl-GUI/assets/61830443/ce6a9628-4705-4a79-a262-8c43db2c92b0)

Here you can adjust all settings supported by your headset.
Changes may or may not persist even after rebooting the system or turning the headset off(It depends on how headsets stores their own settings).

If you have a wireless headset with support for battery levels, you can also minimize HeadsetControl-GUI to the system tray.

![image](https://github.com/nicola02nb/HeadsetControl-GUI/assets/61830443/ea327c0a-e39a-4035-aa99-bc6325724571)

That way, you will be able to see the battery status at a glance and get a reminder when the batteries of your headset run low (below 15%).
Hovering over the tray icon will show you the current battery percentage. You can also right-click the tray icon to bring up a context menu with quick access to the light control. You can also open or completely close the GUI through the context menu.

![image](https://github.com/nicola02nb/HeadsetControl-GUI/assets/61830443/0213a37c-806c-44d5-b8d7-5cc6b5d69407)

### Performance
While the concept of calling another app for every single interaction has some inherit overhead, HeadsetControl-GUI is very light on ressources.
Being open in the background, HeadsetControl-GUI consists of a single process that uses virtually no CPU time and about 8-10MB of system memory.

![image](https://github.com/user-attachments/assets/3171e62d-8a0c-49b6-88bd-e5b03393c7fe)

## Building from source
To build HeadsetControl-GUI from source, you have to have a proper QT-ready development environment.</br>
I developed, built and tested the program with Qt 6.7.0 and [Qt Creator](https://www.qt.io/product/development-tools) as IDE.</br>
Clone the source code, import the project into [Qt Creator](https://www.qt.io/product/development-tools) or your favourite IDE and build it.

## Additional information
This software comes with no warranty whatsoever.</br>
It's not properly tested for memory leakage and may or may not work with configurations other than those I've tested.

**Disclaimer**:
This program is in no way affiliated with Sapd or HeadsetControl.
All issues regarding the functionality of HeadsetControl (like compatiblity with devices) are beyond the scope of this project.
