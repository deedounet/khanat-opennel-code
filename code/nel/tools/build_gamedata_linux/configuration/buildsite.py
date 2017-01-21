#!/usr/bin/python
# 
# \file site.py
# \brief Site configuration
# \date 2017-01-19-23-06-GMT
# \author Jan Boon (Kaetemi)
# Python port of game data build pipeline.
# Site configuration.
# 
# NeL - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
# Copyright (C) 2009-2014  by authors
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 


# *** SITE INSTALLATION ***

# Use '/' in path name, not ''
# Don't put '/' at the end of a directory name


# Quality option for this site (1 for BEST, 0 for DRAFT)
BuildQuality = 1

ToolDirectories = ['/Volumes/SIELA/Khaganat/khaganat/code/build_tools/bin', '/Volumes/SIELA/Khaganat/external/bin']
ToolSuffix = ""

# Build script directory
ScriptDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/nel/tools/build_gamedata_linux"
WorkspaceDirectory = "/Volumes/SIELA/Khaganat/khanat-data/workspace"

# Data build directories
DatabaseDirectory = "/Volumes/SIELA/Khaganat/khanat-assets/database"
ExportBuildDirectory = "/Volumes/SIELA/Khaganat/khanat-build/export"

# Install directories
InstallDirectory = "/Volumes/SIELA/Khaganat/khanat-build/install"
ClientDevDirectory = "/Volumes/SIELA/Khaganat/khanat-build/client_dev"
ClientPatchDirectory = "/Volumes/SIELA/Khaganat/khanat-build/client_patch"
ClientInstallDirectory = "/Volumes/SIELA/Khaganat/khanat-build/client_install"
ShardInstallDirectory = "/Volumes/SIELA/Khaganat/khanat-build/shard"
WorldEditInstallDirectory = "/Volumes/SIELA/Khaganat/khanat-build/worldedit"

# Utility directories
WorldEditorFilesDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/ryzom/common/data_leveldesign/leveldesign/world_editor_files"

# Leveldesign directories
LeveldesignDirectory = "/Volumes/SIELA/Khaganat/khanat-data/leveldesign"
LeveldesignDfnDirectory = "/Volumes/SIELA/Khaganat/khanat-data/leveldesign/DFN"
LeveldesignWorldDirectory = "/Volumes/SIELA/Khaganat/khanat-data/leveldesign/world"
PrimitivesDirectory = "/Volumes/SIELA/Khaganat/khanat-data/primitives"
LeveldesignDataCommonDirectory = "/Volumes/SIELA/Khaganat/khanat-data/common"
LeveldesignDataShardDirectory = "/Volumes/SIELA/Khaganat/khanat-data/shard"
TranslationDirectory = "/Volumes/SIELA/Khaganat/khanat-data/translation"

# Misc data directories
GamedevDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/ryzom/client/data/gamedev"
DataCommonDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/ryzom/common/data_common"
DataShardDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/ryzom/server/data_shard"
WindowsExeDllCfgDirectories = ['C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/redist/x86', 'D:/libraries/external/bin', 'R:/build/dev/bin/Release', 'R:/code/ryzom/client', 'R:/code/nel/lib', 'R:/code/ryzom/bin', 'R:/code/ryzom/tools/client/client_config/bin']
LinuxServiceExecutableDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/build_linux_server/bin"
LinuxClientExecutableDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/build_linux_client/bin"
PatchmanCfgAdminDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/ryzom/server/patchman_cfg/admin_install"
PatchmanCfgDefaultDirectory = "/Volumes/SIELA/Khaganat/khaganat/code/ryzom/server/patchman_cfg/default"
PatchmanBridgeServerDirectory = "/Volumes/SIELA/Khaganat/khanat-build/bridge_server"

# 3dsMax directives
MaxAvailable = 0
MaxDirectory = "C:/Program Files (x86)/Autodesk/3ds Max 2010"
MaxUserDirectory = "C:/Users/Kaetemi/AppData/Local/Autodesk/3dsMax/2010 - 32bit/enu"
MaxExecutable = "3dsmax.exe"


# end of file
