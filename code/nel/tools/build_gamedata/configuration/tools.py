#!/usr/bin/python
# 
# \file process.py
# \brief Tools configuration
# \date 2009-03-10 11:33GMT
# \author Jan Boon (Kaetemi)
# Python port of game data build pipeline.
# Tools configuration.
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

# *** PROCESS TIMEOUT ***
SkelExportTimeout = 600000
SwtExportTimeout = 600000
ShapeExportTimeout = 3600000
ZoneExportTimeout = 1800000
ZoneBuildDependTimeout = 1800000
ZoneBuildWeldTimeout = 60000
ZoneLightBuildTimeout = 600000
ZoneIgLightBuildTimeout = 600000
SmallbankBuildTimeout = 60000
FarbankBuildTimeout = 180000
AnimExportTimeout = 1800000
IgExportTimeout = 600000
MapsBuildTimeout = 10000
CmbExportTimeout = 60000
RbankBuildTesselTimeout = 6000000
RbankBuildSmoothTimeout = 6000000
RbankBuildProclocalTimeout = 6000000
RbankBuildProcglobalTimeout = 18000000
RbankBuildIndoorTimeout = 18000000
# WmapBuildTimeout = 60000
LigoExportTimeout = 3600000
LigoBuildTimeout = 1800000
PacsPrimExportTimeout = 600000

MaxShapeExportTimeout = 300000 # 5min

# *** TOOLS CONFIGURATION ***

TgaToDdsTool = "tga2dds"
BuildInterfaceTool = "build_interface"
ExecTimeoutTool = "exec_timeout"
BuildSmallbankTool = "build_smallbank"
BuildFarbankTool = "build_far_bank"
ZoneDependenciesTool = "zone_dependencies"
ZoneWelderTool = "zone_welder"
BuildRbankTool = "build_rbank"
BuildIndoorRbankTool = "build_indoor_rbank"
BuildIgBoxesTool = "build_ig_boxes"
AiBuildWmapTool = "ai_build_wmap"
GetNeighborsTool = "get_neighbors"
ZoneLighterTool = "zone_lighter"
ZoneIgLighterTool = "zone_ig_lighter"
IgLighterTool = "ig_lighter"
AnimBuilderTool = "anim_builder"
TileEditTool = "tile_edit"
BuildImagesetTool = "th_build_imageset"
MakeSheetIdTool = "make_sheet_id"
BuildSheetsTool = "th_build_sheets"
BuildSoundTool = "th_build_sound"
BuildCoarseMeshTool = "build_coarse_mesh"
LightmapOptimizerTool = "lightmap_optimizer"
BuildClodtexTool = "build_clodtex"
BuildShadowSkinTool = "build_shadow_skin"
PanoplyMakerTool = "panoply_maker"
HlsBankMakerTool = "hls_bank_maker"
LandExportTool = "land_export"
PrimExportTool = "prim_export"
IgElevationTool = "ig_elevation"
IgAddTool = "ig_add"
