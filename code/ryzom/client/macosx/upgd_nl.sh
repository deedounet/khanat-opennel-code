#!/bin/sh

if [ -z "$ROOTPATH" ]
then
  echo "upgd_nl.sh can only be launched from updt_nl.sh"
  exit 1
fi

# determine directory where all files reside
CONTENTSPATH=$(dirname "$ROOTPATH")
MACOSPATH=$(dirname "$KHANAT_CLIENT")
SIGNPATH=$CONTENTSPATH/_CodeSignature

# all files of original Bundle are in the same directory
# we have to copy them to the right location

# client_default.cfg and ryzom.icns are already in the right location

# PkgInfo usually doesn't change so don't copy it

# Info.plist contains updated version
cp -p "$ROOTPATH/Info.plist" "$CONTENTSPATH"

cp -p "$ROOTPATH/CodeResources" "$SIGNPATH"

# executable flag for all executables
chmod +x "$ROOTPATH/Khanat"
chmod +x "$ROOTPATH/CrashReport"
chmod +x "$ROOTPATH/KhanatClientPatcher"
chmod +x "$ROOTPATH/KhanatConfiguration"

# remove previous executables
rm -f "$MACOSPATH/Khanat"
rm -f "$MACOSPATH/CrashReport"
rm -f "$MACOSPATH/KhanatClientPatcher"
rm -f "$MACOSPATH/KhanatConfiguration"

# copy all binaries in MacOS directory
cp -p "$ROOTPATH/Khanat" "$MACOSPATH"
cp -p "$ROOTPATH/CrashReport" "$MACOSPATH"
cp -p "$ROOTPATH/KhanatClientPatcher" "$MACOSPATH"
cp -p "$ROOTPATH/KhanatConfiguration" "$MACOSPATH"

exit 0
