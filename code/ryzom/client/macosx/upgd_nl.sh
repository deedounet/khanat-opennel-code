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
# we have to uncompress them to the right location

# client_default.cfg and khanat.icns are already in the right location

# uncompress Ryzom
if [ -e "$ROOTPATH/Ryzom.zip" ]
then
  unzip -o "$ROOTPATH/Ryzom.zip" -d "$CONTENTSPATH/../.."
fi
chmod +x "$ROOTPATH/Khanat"
chmod +x "$ROOTPATH/KhanatClientPatcher"
chmod +x "$ROOTPATH/KhanatConfiguration"
rm -f "$MACOSPATH/Khanat"
rm -f "$MACOSPATH/KhanatClientPatcher"
rm -f "$MACOSPATH/KhanatConfiguration"

# only uncompress Ryzom Installer if found in parent directory
cp -p "$ROOTPATH/Khanat" "$MACOSPATH"
then
cp -p "$ROOTPATH/KhanatClientPatcher" "$MACOSPATH"
cp -p "$ROOTPATH/KhanatConfiguration" "$MACOSPATH"
fi

exit 0
