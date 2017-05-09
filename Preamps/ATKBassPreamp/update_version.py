#!/usr/bin/python

# this script will update the versions in plist and installer files to match that in resource.h

import plistlib, os, datetime, fileinput, glob, sys, string
scriptpath = os.path.dirname(os.path.realpath(__file__))

def replacestrs(filename, s, r):
  files = glob.glob(filename)
  
  for line in fileinput.input(files,inplace=1):
    line = line.replace(s, r)
    sys.stdout.write(line)

def update_plist(plistpath, CFBundleGetInfoString, CFBundleVersion):
  plist = plistlib.readPlist(plistpath)
  plist['CFBundleGetInfoString'] = CFBundleGetInfoString
  plist['CFBundleVersion'] = CFBundleVersion
  plist['CFBundleShortVersionString'] = CFBundleVersion
  plistlib.writePlist(plist, plistpath)
  replacestrs(plistpath, "//Apple//", "//Apple Computer//");

def main():

  FullVersionStr = ""
  for line in fileinput.input(scriptpath + "/Source/AppConfig.h",inplace=0):
    if "#define JucePlugin_Version " in line:
      FullVersionStr = line.lstrip("#define JucePlugin_Version ").strip()
    
  today = datetime.date.today()
  CFBundleGetInfoString = FullVersionStr + ", Copyright MatthieuBrucher, " + str(today.year)
  CFBundleVersion = FullVersionStr
  
  print("update_version.py - setting version to " + FullVersionStr)
  print("Updating plist version info...")
  
  import glob
  for plistpath in glob.glob(scriptpath + "/Builds/MacOSX/*.plist"):
    update_plist(plistpath, CFBundleGetInfoString, CFBundleVersion)

  print("Updating Mac Installer version info...")
  
  plistpath = scriptpath + "/installer/ATKBassPreamp.pkgproj"
  installer = plistlib.readPlist(plistpath)
  
  for x in installer['PACKAGES']:
    x['PACKAGE_SETTINGS']['VERSION'] = FullVersionStr
  
  plistlib.writePlist(installer, plistpath)
  replacestrs(plistpath, "//Apple//", "//Apple Computer//");
  
  print("Updating Windows Installer version info...")
  
  for line in fileinput.input(scriptpath + "/installer/ATKBassPreamp.iss",inplace=1):
    if "AppVersion" in line:
      line="AppVersion=" + FullVersionStr + "\n"
    sys.stdout.write(line)

if __name__ == '__main__':
  main()
