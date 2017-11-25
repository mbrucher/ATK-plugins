ATK-plugins
===========

Audio plugins based on the Audio Toolkit and JUCE for the newest plugins and WDL-OL for the older ones.

Build instructions
------------------

* On Windows

  * For WDL-OL, get WDL-OL from github, and copy this repository inside the WDL-OL folder. Set ATKROOT, ATK32ROOT and BOOSTROOT paths as environment variables.
  * For JUCE, get JUCE from juce.com. Set ATKROOT, ATK32ROOT, JUCEROOT and BOOSTROOT as environment variables. You may need to add also IPPROOT, VST3SDK and AAXSDK.
  
* On macOS:

  * For WDL-OL, get WDL-OL from github, and copy this repository inside the WDL-OL folder. Specify in WDL-OL's common.xcconfig the ATKROOT, ATK32ROOT and BOOSTROOT path.
  * For JUCE, get JUCE from juce.com. Specify in a file called common.xcconfig located in the parent folder the variables ATKROOT, ATK32ROOT and BOOSTROOT path. You may need to add also IPPROOT, VST3SDK and AAXSDK.

Once this is done, compile the plugin you want.
