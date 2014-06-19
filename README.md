ATK-plugins
===========

Audio plugins based on the Audio Toolkit and WDL-OL

Build instructions
------------------

Get WDL-OL from github, and copy this repository besides WDL-OL. Specify in common.xcconfig the ATKROOT and BOOSTROOT path.

Once this is done, compile the plugin you want.

Boss SD1
--------

This plugin emulates almost all stages of the Boss SD1 overdrive, namely:
* the overdrive stage
* the tone stage
* the level stage

The input stage and output stage are not emulated, as they depend on previous and after elements like guitars, pedals...

It uses a 32x oversampling to limit aliasing (which is why the plugin uses so much CPU).

