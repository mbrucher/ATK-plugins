#!/usr/bin/python

# this script will update the versions in plist and installer files to match that in resource.h

import plistlib, os, datetime, fileinput, glob, sys, string
scriptpath = os.path.dirname(os.path.realpath(__file__))

def replacestrs(filename, s, r):
  files = glob.glob(filename)
  
  for line in fileinput.input(files,inplace=1):
    line = line.replace(s, r)
    sys.stdout.write(line)

def main():

  MajorStr = ""
  MinorStr = "" 
  BugfixStr = ""

  for line in fileinput.input(scriptpath + "/resource.h",inplace=0):
    if "#define PLUG_VER " in line:
      FullVersion = int(line.lstrip("#define PLUG_VER "), 16)
      major = FullVersion & 0xFFFF0000
      MajorStr = str(major >> 16)
      minor = FullVersion & 0x0000FF00
      MinorStr = str(minor >> 8)
      BugfixStr = str(FullVersion & 0x000000FF)
      
  
  FullVersionStr = MajorStr + "." + MinorStr + "." + BugfixStr
  
  today = datetime.date.today()
  CFBundleGetInfoString = FullVersionStr + ", Copyright MatthieuBrucher, " + str(today.year)
  CFBundleVersion = FullVersionStr
  
  print("update_version.py - setting version to " + FullVersionStr)
  print("Updating plist version info...")
  
  plistpath = scriptpath + "/resources/ATKSideChainExpander-VST2-Info.plist"
  print(plistpath)
  vst2 = plistlib.readPlist(plistpath)
  vst2['CFBundleGetInfoString'] = CFBundleGetInfoString
  vst2['CFBundleVersion'] = CFBundleVersion
  vst2['CFBundleShortVersionString'] = CFBundleVersion
  plistlib.writePlist(vst2, plistpath)
  replacestrs(plistpath, "//Apple//", "//Apple Computer//");
  
  plistpath = scriptpath + "/resources/ATKSideChainExpander-AU-Info.plist"
  au = plistlib.readPlist(plistpath)
  au['CFBundleGetInfoString'] = CFBundleGetInfoString
  au['CFBundleVersion'] = CFBundleVersion
  au['CFBundleShortVersionString'] = CFBundleVersion
  plistlib.writePlist(au, plistpath)
  replacestrs(plistpath, "//Apple//", "//Apple Computer//");
  
  plistpath = scriptpath + "/resources/ATKSideChainExpander-VST3-Info.plist"
  vst3 = plistlib.readPlist(plistpath)
  vst3['CFBundleGetInfoString'] = CFBundleGetInfoString
  vst3['CFBundleVersion'] = CFBundleVersion
  vst3['CFBundleShortVersionString'] = CFBundleVersion
  plistlib.writePlist(vst3, plistpath)
  replacestrs(plistpath, "//Apple//", "//Apple Computer//");
  
  plistpath = scriptpath + "/resources/ATKSideChainExpander-OSXAPP-Info.plist"
  app = plistlib.readPlist(plistpath)
  app['CFBundleGetInfoString'] = CFBundleGetInfoString
  app['CFBundleVersion'] = CFBundleVersion
  app['CFBundleShortVersionString'] = CFBundleVersion
  plistlib.writePlist(app, plistpath)
  replacestrs(plistpath, "//Apple//", "//Apple Computer//");
  
#  plistpath = scriptpath + "/resources/ATKSideChainExpander-RTAS-Info.plist"
#  rtas = plistlib.readPlist(plistpath)
#  rtas['CFBundleGetInfoString'] = CFBundleGetInfoString
#  rtas['CFBundleVersion'] = CFBundleVersion
#  rtas['CFBundleShortVersionString'] = CFBundleVersion
#  plistlib.writePlist(rtas, plistpath)
#  replacestrs(plistpath, "//Apple//", "//Apple Computer//");
#  
#  plistpath = scriptpath + "/resources/ATKSideChainExpander-AAX-Info.plist"
#  aax = plistlib.readPlist(plistpath)
#  aax['CFBundleGetInfoString'] = CFBundleGetInfoString
#  aax['CFBundleVersion'] = CFBundleVersion
#  aax['CFBundleShortVersionString'] = CFBundleVersion
#  plistlib.writePlist(aax, plistpath)
#  replacestrs(plistpath, "//Apple//", "//Apple Computer//");

#   plistpath = scriptpath + "/resources/ATKSideChainExpander-IOSAPP-Info.plist"
#   iosapp = plistlib.readPlist(plistpath)
#   iosapp['CFBundleGetInfoString'] = CFBundleGetInfoString
#   iosapp['CFBundleVersion'] = CFBundleVersion
#   iosapp['CFBundleShortVersionString'] = CFBundleVersion
#   plistlib.writePlist(iosapp, plistpath)
#   replacestrs(plistpath, "//Apple//", "//Apple Computer//");

  print("Updating Mac Installer version info...")
  
  plistpath = scriptpath + "/installer/ATKSideChainExpander.pkgproj"
  installer = plistlib.readPlist(plistpath)
  
  for x in installer['PACKAGES']:
    x['PACKAGE_SETTINGS']['VERSION'] = FullVersionStr
  
  plistlib.writePlist(installer, plistpath)
  replacestrs(plistpath, "//Apple//", "//Apple Computer//");
  
  print("Updating Windows Installer version info...")
  
  for line in fileinput.input(scriptpath + "/installer/ATKSideChainExpander.iss",inplace=1):
    if "AppVersion" in line:
      line="AppVersion=" + FullVersionStr + "\n"
    sys.stdout.write(line)

if __name__ == '__main__':
  main()
