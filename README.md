# DTC
TV Commercial Mute system

Arduino based audio mute button that works over a wireless 2.4Ghz RF link.
System consists of 2 units.  A handheld remote control (TX) and a fixed receiving unit (RX).

TV Commercials typically run in 30 second intervals.  A standard commercial break is 
usually 4 minutes long.  Sometimes a commercial break can go as long as 6 minutes or 
as short as 3 minutes.  A trick used on popular shows is to have all the commercials be 4 
minutes long except the last one which can be 5 or 6 minutes long.  

What the DTC does is to create a timed mute period of 4 minutes.  When the timer is up
the DTC un-mutes the audio.  So if you are out of the room you will hear the audio come 
back on and will know your show has resumed. The DTC also has a manual mute button.  This
allows you to mute the audio until you press the button a second time.  This is handly if
someone comes to the door or for a phone call.
The DTC provides some switches to allow you to customize the mute period by adding time.


