#! /bin/sh

#shell script to automate IPlug Project build, code-signing and packaging on OSX

BASEDIR=$(dirname $0)
cd $BASEDIR

#---------------------------------------------------------------------------------------------------------

#variables

VERSION=`echo | grep "#define JucePlugin_Version " JuceLibraryCode/AppConfig.h`
VERSION=${VERSION//\#define JucePlugin_Version }
VERSION=${VERSION//\'}

FULL_VERSION=$(echo "${VERSION}" | tr -d '[:space:]')

PLUGIN_NAME=`echo | grep "#define JucePlugin_Name " JuceLibraryCode/AppConfig.h`
PLUGIN_NAME=${PLUGIN_NAME//\#define JucePlugin_Name }
PLUGIN_NAME=${PLUGIN_NAME//\"}
PLUGIN_NAME=$(echo "${PLUGIN_NAME}" | tr -d '[:space:]')

# work out the paths to the binaries

PKG="installer/build-mac/$PLUGIN_NAME Installer.pkg"
PKG_US="installer/build-mac/$PLUGIN_NAME Installer.unsigned.pkg"

echo "making $PLUGIN_NAME version $FULL_VERSION mac distribution..."
echo ""

#---------------------------------------------------------------------------------------------------------

#call python script to update version numbers
./update_version.py

#here you can use the touch command to force xcode to rebuild

#---------------------------------------------------------------------------------------------------------

# build xcode project. Change target to build individual formats
echo "Build"
xcodebuild -project Builds/MacOSX/$PLUGIN_NAME.xcodeproj -xcconfig $PLUGIN_NAME.xcconfig -target "$PLUGIN_NAME - All" -configuration Release 2> ./build-mac.log

if [ -s build-mac.log ]
then
  echo "build failed due to following errors:"
  echo ""
  cat build-mac.log
  exit 1
else
 rm build-mac.log
fi

asciidoctor -r asciidoctor-pdf -b pdf manual/manual.adoc -o manual/${PLUGIN_NAME}_manual.pdf

#---------------------------------------------------------------------------------------------------------

# installer, uses Packages http://s.sudre.free.fr/Software/Packages/about.html
rm -R -f installer/$PLUGIN_NAME-mac.dmg

echo "building installer"
echo ""
chmod 0777 installer
packagesbuild installer/$PLUGIN_NAME.pkgproj

#echo "code-sign installer for Gatekeeper on 10.8"
#echo ""
#mv "${PKG}" "${PKG_US}"
#productsign --sign "Developer ID Installer: ""${CERT_ID}" "${PKG_US}" "${PKG}"
#
#rm -R -f "${PKG_US}"

#---------------------------------------------------------------------------------------------------------

# dmg, can use dmgcanvas http://www.araelium.com/dmgcanvas/ to make a nice dmg

echo "building dmg"
echo ""

if [ -d installer/$PLUGIN_NAME.dmgCanvas ]
then
  dmgcanvas installer/$PLUGIN_NAME.dmgCanvas installer/$PLUGIN_NAME-mac.dmg
else
  hdiutil create installer/$PLUGIN_NAME.dmg -srcfolder installer/build-mac/ -ov -anyowners -volname $PLUGIN_NAME
  
  if [ -f installer/$PLUGIN_NAME-mac.dmg ]
  then
   rm -f installer/$PLUGIN_NAME-mac.dmg
  fi
  
  hdiutil convert installer/$PLUGIN_NAME.dmg -format UDZO -o installer/$PLUGIN_NAME-mac.dmg
  rm -R -f installer/$PLUGIN_NAME.dmg
fi

rm -R -f installer/build-mac/

#---------------------------------------------------------------------------------------------------------

echo "done"
