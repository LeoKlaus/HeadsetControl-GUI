# HeadsetControl-GUI [![Github All Releases](https://img.shields.io/github/downloads/LeoKlaus/headsetcontrol-gui/total.svg)]() [![license](https://img.shields.io/github/license/LeoKlaus/HeadsetControl-GUI)]()
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

### WinGet
```powershell
winget install LeoKlaus.HeadsetControl-GUI
```
After that to you can reboot powershell an you can run the program with the command:
```powershell
HeadsetControl-GUI
```

### Manual
1. Download the [latest release](https://github.com/LeoKlaus/HeadsetControl-GUI/releases/latest/) of HeadsetControl-GUI from the [releases section](https://github.com/nicola02nb/HeadsetControl-GUI/releases) of this page.
2. Extract HeadsetControl-GUI to any folder.

The finished folder should look something like this:

![361239376-0145ca37-6e59-4170-ba26-804e8856dbc8](https://github.com/user-attachments/assets/36233a85-1500-4789-9368-1573ff8f4fed)

## Usage
Start HeadsetControl-GUI by double-clicking "HeadsetControl-GUI.exe", and if your headset is supported and everything was set up correctly, you will be greeted by the following screen HeadsetControl-GUI has..

If you don't find some features in you ui, probably it's not supported by your headset or it has not been implemented by [HeadsetControl](https://github.com/Sapd/HeadsetControl/).

![Videosenzatitolo-RealizzatoconClipchamp-ezgif com-crop](https://github.com/user-attachments/assets/9a25de13-deca-45e0-aeb5-2a9d3876e9b2)

Here you can adjust all settings supported by your headset.
Changes may or may not persist even after rebooting the system or turning the headset off(It depends on how headsets stores their own settings).

If you have a wireless headset with support for battery levels, you can also minimize HeadsetControl-GUI to the system tray.

![338270796-ea327c0a-e39a-4035-aa99-bc6325724571](https://github.com/user-attachments/assets/b71d5cb6-c3f6-4ffb-b276-b4e8934ace2c)

That way, you will be able to see the battery status at a glance and get a reminder when the batteries of your headset run low (below 15%).
Hovering over the tray icon will show you the current battery percentage. You can also right-click the tray icon to bring up a context menu with quick access to the light control. You can also open or completely close the GUI through the context menu.

![Screenshot 2024-06-10 183803](https://github.com/user-attachments/assets/1bcf625a-e18c-4df9-b3a4-973075e3c335)

### Performance
While the concept of calling another app for every single interaction has some inherit overhead, HeadsetControl-GUI is very light on ressources.
Being open in the background, HeadsetControl-GUI consists of a single process that uses virtually no CPU time and about 8-10MB of system memory.

![349140526-3171e62d-8a0c-49b6-88bd-e5b03393c7fe](https://github.com/user-attachments/assets/a3b2af01-165e-46c1-90ec-75b579f95e33)

## Building from source
To build HeadsetControl-GUI from source, you have to have a proper QT-ready development environment.</br>
I developed, built and tested the program with Qt 6.7.0 and [Qt Creator](https://www.qt.io/product/development-tools) as IDE.</br>
Clone the source code, import the project into [Qt Creator](https://www.qt.io/product/development-tools) or your favourite IDE and build it.

## Additional information
This software comes with no warranty whatsoever.</br>
It's not properly tested for memory leakage and may or may not work with configurations other than those I've tested.

**Disclaimer**:
This program is in no way affiliated with Sapd or HeadsetControl.
All issues regarding the functionality of HeadsetControl (like [compatiblity with devices](https://github.com/Sapd/HeadsetControl/?tab=readme-ov-file#supported-headsets)) are beyond the scope of this project.
