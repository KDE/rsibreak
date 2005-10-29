/**
   \mainpage RSIBreak Documentation 

   If you want to start hacking on RSIBreak, this page will get you going in no-time. 

   This documentation is written by Tom Albers, tomalbers@kde.nl, http://www.omat.nl
   
   RSIBreak is a simple program with just a few classes. You can click on Class List 
   to get the details, but roughly this is how it works: The settings are setup in 
   three classes, notthing special. Each tab has its own class and a general class
   setup the widget and connects to the different tabs.

   The RSIDock class is responsible for handling everything related to the docked icon, 
   so it handles the context menus and the content of those, for example the about boxes.

   Everything related to the breaks, activating them, maximizing, loading the right images
   and idle detection is done in the RSIWidget class. This can be considered the core of 
   RSIBreak.

   All public classes are documented, and it is mandartory to do that, just like honouring 
   the hacking style.

   For debugging purposes, you can add a DEBUG=true to your [General Settings] in the 
   rsibreakrc config file. This will change the minutes in the settings to seconds. 
   This is essential for testing purposes.

   You can contact me via email or on irc on the freenode network, channel \#rsibreak

   Have fun!
*/
