#!/usr/bin/python
# 
# \file 3_install.py
# \brief Install interface
# \date 2009-03-10 13:13GMT
# \author Jan Boon (Kaetemi)
# Python port of game data build pipeline.
# Install interface
# 
# NeL - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
# Copyright (C) 2010  Winch Gate Property Limited
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

import time, sys, os, shutil, subprocess, distutils.dir_util
sys.path.append("../../configuration")
sys.path.append("../../configuration/project")
if os.path.isfile("log.log"):
	os.remove("log.log")
log = open("log.log", "w")
from scripts import *
from buildsite import *
from process import *
from tools import *
from directories import *

printLog(log, "")
printLog(log, "-------")
printLog(log, "--- Install interface")
printLog(log, "-------")
printLog(log, time.strftime("%Y-%m-%d %H:%MGMT", time.gmtime(time.time())))
printLog(log, "")

clientPath = ClientDataDirectory + "/" + InterfaceClientDirectory
mkPath(log, clientPath)

printLog(log, ">>> Install interface <<<")
mkPath(log, ExportBuildDirectory + "/" + InterfaceBuildDirectory)
copyFilesNoTreeIfNeeded(log, ExportBuildDirectory + "/" + InterfaceBuildDirectory, clientPath)

printLog(log, ">>> Install interface dxtc <<<")
mkPath(log, ExportBuildDirectory + "/" + InterfaceDxtcBuildDirectory)
copyFilesNoTreeIfNeeded(log, ExportBuildDirectory + "/" + InterfaceDxtcBuildDirectory, clientPath)

printLog(log, ">>> Install interface fullscreen <<<")
mkPath(log, ExportBuildDirectory + "/" + InterfaceFullscreenExportDirectory)
copyFilesNoTreeIfNeeded(log, ExportBuildDirectory + "/" + InterfaceFullscreenExportDirectory, clientPath)

printLog(log, ">>> Install interface 3d <<<")
mkPath(log, ExportBuildDirectory + "/" + Interface3DExportDirectory)
copyFilesNoTreeIfNeeded(log, ExportBuildDirectory + "/" + Interface3DExportDirectory, clientPath)

printLog(log, "")
log.close()


# end of file
