#!/usr/bin/python
# 
# \file 2_build.py
# \brief Build ligo
# \date 2010-05-24 08:13GMT
# \author Jan Boon (Kaetemi)
# Python port of game data build pipeline.
# Build ligo
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
printLog(log, "--- Build ligo")
printLog(log, "-------")
printLog(log, time.strftime("%Y-%m-%d %H:%MGMT", time.gmtime(time.time())))
printLog(log, "")

if LigoExportLand != "":
	ExecTimeout = findTool(log, ToolDirectories, ExecTimeoutTool, ToolSuffix)
	LandExport = findTool(log, ToolDirectories, LandExportTool, ToolSuffix)

	printLog(log, ">>> Generate ligo zone <<<")
	if LandExport == "":
		toolLogFail(log, LandExportTool, ToolSuffix)
	elif ExecTimeout == "":
		toolLogfail(log, ExecTimeoutTool, ToolSuffix)
	else:
		subprocess.call([ ExecTimeout, str(LigoExportTimeout), LandExport, ActiveProjectDirectory + "/generated/land_exporter.cfg" ])

	printLog(log, ">>> Copy to zone builder <<<")
	dirSource = ExportBuildDirectory + "/" + LigoZoneBuildDirectory
	dirTarget = ExportBuildDirectory + "/" + ZoneExportDirectory
	mkPath(log, dirSource)
	mkPath(log, dirTarget)
	copyFilesExtReplaceNoTreeIfNeeded(log, dirSource, dirTarget, ".zonel", ".zone")
	copyFilesExtNoTreeIfNeeded(log, dirSource, dirTarget, ".zonenh")

log.close()


# end of file
