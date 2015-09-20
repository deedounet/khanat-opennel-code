// NeL - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2015  Winch Gate Property Limited
// Author: Jan Boon <jan.boon@kaetemi.be>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <nel/misc/types_nl.h>
#include "assimp_shape.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#define NL_NODE_INTERNAL_TYPE aiNode
#define NL_SCENE_INTERNAL_TYPE aiScene
#include "scene_context.h"

#include <nel/misc/debug.h>
#include <nel/misc/path.h>
#include <nel/misc/tool_logger.h>

#include <nel/3d/mesh.h>

#include "assimp_material.h"

using namespace std;
using namespace NLMISC;
using namespace NL3D;

// TODO: buildParticleSystem ??
// TODO: buildWaveMakerShape ??
// TODO: buildRemanence ??
// TODO: buildFlare ??
// Probably specific settings we can only do in meta editor on a dummy node..
// TODO: pacs prim

// TODO: buildWaterShape specifics when node has water material

// TODO: CMeshMultiLod::CMeshMultiLodBuild multiLodBuild; export_mesh.cpp ln 228
// TODO: LOD MRM

// TODO: Skinned - reverse transform by skeleton root bone to align?

void assimpBuildBaseMesh(CMeshBase::CMeshBaseBuild &buildBaseMesh, CMeshUtilsContext &context, CNodeContext &nodeContext)
{
	const aiNode *node = nodeContext.InternalNode;
	// Reference CExportNel::buildBaseMeshInterface

	// Load materials
	buildBaseMesh.Materials.resize(node->mNumMeshes);

	for (unsigned int mi = 0; mi < node->mNumMeshes; ++mi)
	{
		const aiMesh *mesh = context.InternalScene->mMeshes[node->mMeshes[mi]];
		const aiMaterial *am = context.InternalScene->mMaterials[mesh->mMaterialIndex];

		aiString amname;
		if (am->Get(AI_MATKEY_NAME, amname) != aiReturn_SUCCESS)
		{
			tlerror(context.ToolLogger, context.Settings.SourceFilePath.c_str(),
				"Material used by node '%s' has no name", node->mName.C_Str()); // TODO: Maybe autogen names by index in mesh or node if this is actually a thing
			assimpMaterial(buildBaseMesh.Materials[mi], context, am);
		}
		else
		{
			buildBaseMesh.Materials[mi] = *context.SceneMeta.Materials[amname.C_Str()];
		}
	}
}

inline CVector convVector(const aiVector3D &av)
{
	return CVector(-av.x, av.z, av.y); // COORDINATE CONVERSION
}

inline CRGBA convColor(const aiColor4D &ac)
{
	return CRGBA(ac.r * 255.99f, ac.g * 255.99f, ac.b * 255.99f, ac.a * 255.99f);
}

bool assimpBuildMesh(CMesh::CMeshBuild &buildMesh, CMeshBase::CMeshBaseBuild &buildBaseMesh, CMeshUtilsContext &context, CNodeContext &nodeContext)
{
	// TODO
	// *** If the mesh is skined, vertices will be exported in world space.
	// *** If the mesh is not skined, vertices will be exported in offset space.

	// TODO Support skinning

	const aiNode *node = nodeContext.InternalNode;

	// Basic validations before processing starts
	for (unsigned int mi = 0; mi < node->mNumMeshes; ++mi)
	{
		// TODO: Maybe needs to be the same count too for all meshes, so compare with mesh 0
		const aiMesh *mesh = context.InternalScene->mMeshes[node->mMeshes[mi]];
		if (mesh->GetNumColorChannels() > 2)
		{
			tlerror(context.ToolLogger, context.Settings.SourceFilePath.c_str(),
				"(%s) mesh->GetNumColorChannels() > 2", node->mName.C_Str());
			return false;
		}
		if (mesh->GetNumUVChannels() > CVertexBuffer::MaxStage)
		{
			tlerror(context.ToolLogger, context.Settings.SourceFilePath.c_str(),
				"(%s) mesh->GetNumUVChannels() > CVertexBuffer::MaxStage", node->mName.C_Str());
			return false;
		}
		if (!mesh->HasNormals())
		{
			tlerror(context.ToolLogger, context.Settings.SourceFilePath.c_str(),
				"(%s) !mesh->HasNormals()", node->mName.C_Str());
			return false;
		}
	}

	// Default vertex flags
	buildMesh.VertexFlags = CVertexBuffer::PositionFlag | CVertexBuffer::NormalFlag;

	// TODO: UV Channels
	for (uint i = 0; i < CVertexBuffer::MaxStage; ++i)
		buildMesh.UVRouting[i] = i;

	// Meshes in assimp are separated per material, so we need to re-merge them for the mesh build process
	// This process also deduplicates vertices
	sint32 numVertices = 0;
	for (unsigned int mi = 0; mi < node->mNumMeshes; ++mi)
		numVertices += context.InternalScene->mMeshes[node->mMeshes[mi]]->mNumVertices;
	buildMesh.Vertices.resize(numVertices);
	numVertices = 0;
	map<CVector, sint32> vertexIdentifiers;
	vector<vector<sint32> > vertexRemapping;
	vertexRemapping.resize(node->mNumMeshes);
	for (unsigned int mi = 0; mi < node->mNumMeshes; ++mi)
	{
		const aiMesh *mesh = context.InternalScene->mMeshes[node->mMeshes[mi]];
		vertexRemapping[mi].resize(mesh->mNumVertices);
		for (unsigned int vi = 0; vi < mesh->mNumVertices; ++vi)
		{
			CVector vec = convVector(mesh->mVertices[vi]);
			map<CVector, sint32>::iterator vecit = vertexIdentifiers.find(vec);
			if (vecit == vertexIdentifiers.end())
			{
				buildMesh.Vertices[numVertices] = vec;
				vertexIdentifiers[vec] = numVertices;
				vertexRemapping[mi][vi] = numVertices;
				++numVertices;
			}
			else
			{
				vertexRemapping[mi][vi] = vecit->second;
			}
		}
	}
	buildMesh.Vertices.resize(numVertices);

	// Process all faces
	// WONT IMPLEMENT: Radial faces generation... is linked to smoothing group... 
	// leave radial normals generation to modeling tool for now...
	sint32 numFaces = 0;
	for (unsigned int mi = 0; mi < node->mNumMeshes; ++mi)
		numFaces += context.InternalScene->mMeshes[node->mMeshes[mi]]->mNumFaces;
	buildMesh.Faces.resize(numFaces);
	numFaces = 0;
	for (unsigned int mi = 0; mi < node->mNumMeshes; ++mi)
	{
		const aiMesh *mesh = context.InternalScene->mMeshes[node->mMeshes[mi]];
		unsigned int numColorChannels = mesh->GetNumColorChannels(); // TODO: Maybe needs to be same on all mesh parts
		for (unsigned int fi = 0; fi < mesh->mNumFaces; ++fi)
		{
			const aiFace &af = mesh->mFaces[fi];
			if (af.mNumIndices != 3)
			{
				tlerror(context.ToolLogger, context.Settings.SourceFilePath.c_str(),
					"(%s) Face %i on mesh %i has %i faces", node->mName.C_Str(), fi, mi, af.mNumIndices);
				continue; // return false; Keep going, just drop the face for better user experience
			}
			CMesh::CFace &face = buildMesh.Faces[numFaces];
			face.MaterialId = mi;
			face.SmoothGroup = 0; // No smoothing groups (bitfield)
			face.Corner[0].Vertex = vertexRemapping[mi][af.mIndices[0]];
			face.Corner[1].Vertex = vertexRemapping[mi][af.mIndices[1]];
			face.Corner[2].Vertex = vertexRemapping[mi][af.mIndices[2]];
			face.Corner[0].Normal = convVector(mesh->mNormals[af.mIndices[0]]);
			face.Corner[1].Normal = convVector(mesh->mNormals[af.mIndices[1]]);
			face.Corner[2].Normal = convVector(mesh->mNormals[af.mIndices[2]]);
			// TODO: If we want normal maps, we need to add tangent vectors to CFace and build process
			// TODO: UV
			if (numColorChannels > 0) // TODO: Verify
			{
				face.Corner[0].Color = convColor(mesh->mColors[0][af.mIndices[0]]);
				face.Corner[1].Color = convColor(mesh->mColors[0][af.mIndices[1]]);
				face.Corner[2].Color = convColor(mesh->mColors[0][af.mIndices[2]]);
				if (numColorChannels > 1) // TODO: Verify
				{
					face.Corner[0].Specular = convColor(mesh->mColors[1][af.mIndices[0]]);
					face.Corner[1].Specular = convColor(mesh->mColors[1][af.mIndices[1]]);
					face.Corner[2].Specular = convColor(mesh->mColors[1][af.mIndices[2]]);
				}
			}
			// TODO: Color modulate, alpha, use color alpha for vp tree, etc
			++numFaces;
		}
	}
	buildMesh.Faces.resize(numFaces);

	// clear for MRM info
	buildMesh.Interfaces.clear();
	buildMesh.InterfaceLinks.clear();

	// TODO: Export VP
	buildMesh.MeshVertexProgram = NULL;
	
	return true;
}

bool assimpShape(CMeshUtilsContext &context, CNodeContext &nodeContext)
{
	// Reference: export_mesh.cpp, buildShape
	nodeContext.Shape = NULL;

	const aiNode *node = nodeContext.InternalNode;
	nlassert(node->mNumMeshes);

	// Fill the build interface of CMesh
	CMeshBase::CMeshBaseBuild buildBaseMesh;
	assimpBuildBaseMesh(buildBaseMesh, context, nodeContext);

	CMesh::CMeshBuild buildMesh;
	if (!assimpBuildMesh(buildMesh, buildBaseMesh, context, nodeContext))
		return false;

	// Make a CMesh object
	CMesh *mesh = new CMesh();

	// Build the mesh with the build interface
	mesh->build(buildBaseMesh, buildMesh);

	// TODO
	// Reference: export_mesh.cpp, buildShape
	// Must be done after the build to update vertex links
	// Pass to buildMeshMorph if the original mesh is skinned or not
	// buildMeshMorph(buildMesh, node, time, nodeMap != NULL);
	// mesh->setBlendShapes(buildMesh.BlendShapes);

	// optimize number of material
	// mesh->optimizeMaterialUsage(materialRemap);

	// Store mesh in context
	nodeContext.Shape = mesh;
	return true;
}

/* end of file */
