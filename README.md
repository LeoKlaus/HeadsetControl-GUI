A simply remake of [HeadsetControl-GUI](https://github.com/LeoKlaus/HeadsetControl-GUI) by LeoKlaus
# HeadsetControl-GUI
This is a GUI for [Sapds great HeadsetControl](https://github.com/Sapd/HeadsetControl/). It's just a frontend to graphically interact with the original HeadsetControl and has no functionality by itself.

I have to give a huge thank you to Sapd for doing all the heavy lifting and developing the command line tool HeadsetControl without which this project wouldn't be possible.

**Disclaimer**:
This program is in no way affiliated with Sapd or HeadsetControl.
All issues regarding the functionality of HeadsetControl (like compatiblity with devices) are beyond the scope of this project.

## Installation (Windows only for now)
Download the [latest release](https://github.com/LeoKlaus/HeadsetControl-GUI/releases/latest/) of HeadsetControl-GUI from the [releases section](https://github.com/nicola02nb/HeadsetControl-GUI/releases) of this page.
Download the corresponding version of [Sapds HeadsetControl from their GitHub page](https://github.com/Sapd/HeadsetControl/releases/).

Extract HeadsetControl-GUI to any folder of your choice and drop HeadsetControl into the same folder.
The finished folder should look something like this:

![Screenshot of the folder structure](https://i.imgur.com/bbymxL6.jpg "Screenshot of the folder structure")

You HAVE to download a version of the [original headsetcontrol](https://github.com/Sapd/HeadsetControl/releases/) and put it in the same folder.
The executable of headsetcontrol has to be called "HeadsetControl".
Only if both these requirements are met, the GUI can work.

## Usage
Start HeadsetControl-GUI by double-clicking "HeadsetControl-GUI.exe".
If your headset is supported and everything was set up correctly, you will be greeted by the only screen HeadsetControl-GUI has.

![image](https://github.com/nicola02nb/HeadsetControl-GUI/assets/61830443/f0d3b4d3-ea55-42b7-9bf9-fe039fbcd104)

Here you can adjust all settings supported by your headset.
In my experience, these changes persist even after rebooting the system or turning the headset off.
If you have a wired headset and are finished changing settings, you can close the GUI and call it day. Yay!

If you have a wireless headset with support for battery levels, you can also minimize HeadsetControl-GUI to the system tray.

![HeadsetControl-GUI in the system tray](https://i.imgur.com/83Apn66.jpg)

That way, you will be able to see the battery status at a glance and get a reminder when the batteries of your headset run low (below 30%).
Hovering over the tray icon will show you the current battery percentage. You can also right-click the tray icon to bring up a context menu with quick access to the light control. You can also open or completely close the GUI through the context menu.

![The tray icon context menu](https://i.imgur.com/2IWhbfa.jpg)

## Performance
While the concept of calling another app for every single interaction has some inherit overhead, HeadsetControl-GUI is very light on ressources.
Being open in the background, HeadsetControl-GUI consists of a single process that uses virtually no CPU time and less than 8MB of system memory.

![Screenshot of the background task in task manager](https://i.imgur.com/3PaxKF6.jpg)


## Building from source
To build HeadsetControl-GUI from source, you have to have a proper QT-ready development environment.
I developed, built and tested the program with Qt 6.0.3, though there's no apparent reason why it wouldn't work with older or newer versions of Qt.
Clone the source code, import the project into Qt creator or your favourite IDE and build it.

## Support for other platforms
I haven't used any platform-specific code, so generally, you should be able to build and run this app on Linux or MacOS just fine.
I haven't taken the time to build and test on neither Linux nor MacOS (yet), so I can't make any definitive claims on what is supported and what isn't.
If you are on Linux or Mac and try to build the app, I'd be happy to hear if it did or didn't work.

## Additional information
This was written in a day and I'm aware the code is pretty ugly. I plan to fix this and add some additional functionality like persistent settings later down the road.

This software comes with no warranty whatsoever. It's not properly tested for memory leakage and may or may not work with configurations other than those I've tested.
