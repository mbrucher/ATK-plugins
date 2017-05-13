#!/usr/bin/python

# USAGE:
# duplicate.py [inputprojectname] [outputprojectname]


import fileinput, glob, string, sys, os, re, uuid
from shutil import copy, copytree, ignore_patterns, rmtree
from os.path import join

VERSION = "0.9"

# binary files that we don't want to do find and replace inside
FILTERED_FILE_EXTENSIONS = [".ico",".icns", ".pdf", ".png", ".zip", ".exe", ".wav", ".aif"]
# files that we don't want to duplicate
DONT_COPY = ("*.exe", "*.dmg", "*.pkg", "*.mpkg", "*.svn", "*.ncb", "*.suo", "*sdf", "ipch", "build-*", "*.layout", "*.depend", ".DS_Store", "*manual.pdf", "xcuserdata")

SUBFOLDERS_TO_SEARCH = [
  "resources",
  "installer",
  "manual",
  "xcschemes",
  "xcshareddata",
  "project.xcworkspace",
  "Builds",
  "Source",
  "MacOSX",
  "VisualStudio2017"
]

def checkdirname(name, searchproject):
  "check if directory name matches with the given pattern"
  print("")
  if name == searchproject:
    return True
  else:
    return False

def replacestrs(filename, s, r):
  files = glob.glob(filename)
  s_low = s.lower()
  r_low = r.lower()
  
  for line in fileinput.input(files,inplace=1):
    line = line.replace(s, r)
    line = line.replace(s_low, r_low)
    sys.stdout.write(line)

def dirwalk(dir, searchproject, replaceproject):
  for f in os.listdir(dir):
    fullpath = os.path.join(dir, f)
    
    if os.path.isdir(fullpath) and not os.path.islink(fullpath):
      if checkdirname(f, searchproject + ".xcodeproj"):
        os.rename(fullpath, os.path.join(dir, replaceproject + ".xcodeproj"))
        fullpath = os.path.join(dir, replaceproject + ".xcodeproj")
        
        print("recursing in main xcode project directory: ")
        for x in dirwalk(fullpath, searchproject, replaceproject):
          yield x
      elif checkdirname(f, searchproject + "-ios.xcodeproj"):
        os.rename(fullpath, os.path.join(dir, replaceproject + "-ios.xcodeproj"))
        fullpath = os.path.join(dir, replaceproject + "-ios.xcodeproj")
        
        print("recursing in ios xcode project directory: ")
        for x in dirwalk(fullpath, searchproject, replaceproject):
          yield x
      elif (f in SUBFOLDERS_TO_SEARCH):
        print('recursing in ' + f + ' directory: ')
        for x in dirwalk(fullpath, searchproject, replaceproject):
          yield x

    if os.path.isfile(fullpath):
      filename = os.path.basename(fullpath)
      newfilename = filename.replace(searchproject, replaceproject)
      base, extension = os.path.splitext(filename)
      
      if (not(extension in FILTERED_FILE_EXTENSIONS)):
        print("Replacing project name strings in file " + filename)
        replacestrs(fullpath, searchproject, replaceproject)
        
        print("Replacing captitalized project name strings in file " + filename)
        replacestrs(fullpath, searchproject.upper(), replaceproject.upper())
        
      else:
        print("NOT replacing name strings in file " + filename)
      
      if filename != newfilename:
        print("Renaming file " + filename + " to " + newfilename)
        os.rename(fullpath, os.path.join(dir, newfilename))
  
      yield f, fullpath 
    else:
      yield f, fullpath

def main():
  if len(sys.argv) < 3:
    print("Usage: duplicate.py inputprojectname outputprojectname")
    sys.exit(1)
  else:
    input=sys.argv[1]
    output=sys.argv[2]

    if ' ' in input:
      print("error: input project name has spaces")
      sys.exit(1)
      
    if ' ' in output:
      print("error: output project name has spaces")
      sys.exit(1)
    
    # remove a trailing slash if it exists
    if input[-1:] == "/":
      input = input[0:-1]
    
    if output[-1:] == "/":
      output = output[0:-1]
      
    #check that the folders are OK
    if os.path.isdir(input) == False:
      print("error: input project not found")
      sys.exit(1)
        
    if os.path.isdir(output):
      print("error: output folder allready exists")
      sys.exit(1)
    # rmtree(output)
        
    print("copying " + input + " folder to " + output)
    copytree(input, output, ignore=ignore_patterns(*DONT_COPY))
    cpath = os.path.join(os.getcwd(), output)

    #replace manufacturer name strings
    for dir in dirwalk(cpath, input, output):
      pass

if __name__ == '__main__':
  main()
