#include "AssetManager.h"

#include "DirectoryTree.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "StaticMesh.h"
#include "AlbedoTexture.h"
#include "NormalTexture.h"
#include <EngineContentRegistry.h>

#include "JsonInclude.h"
#include <fstream>


auto AssetManager::Initialize() -> void
{
	directoryTree.reset(new DirectoryTree(Path("Assets")));
	FillDirectoryTree();
}

auto AssetManager::FillDirectoryTree() -> void
{
	for (auto dir_entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
		const Path path = dir_entry.path().lexically_relative(assetsPath);
		const bool isDirectory = dir_entry.is_directory() || dir_entry.is_regular_file() && dir_entry.path().extension() == Path(".fbx");
		if (dir_entry.is_directory()) {
			directoryTree->AddNodeByPath(path, DirectoryTreeNodeType::Directory);
		}
		else if (dir_entry.is_regular_file())
		{
			
			if (IsAssetCollectionExtension(path.extension()))
			{
				Assimp::Importer importer;
				const aiScene* scene = importer.ReadFile(dir_entry.path().string(), aiProcess_FlipUVs | aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

				directoryTree->AddNodeByPath(path, DirectoryTreeNodeType::AssetCollection);
				
				// todo: think of how to make it better
				// at least save this data into meta files
				std::vector<const aiNode*> stack;
				stack.push_back(scene->mRootNode);
				while (!stack.empty())
				{
					const aiNode* node = stack.back();
					stack.pop_back();

					stack.insert(stack.end(), node->mChildren, node->mChildren + node->mNumChildren);

					if (node->mNumMeshes > 0)
					{
						for (size_t i = 0; i < node->mNumMeshes; ++i)
						{
							const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
							directoryTree->AddNodeByPath(path/Path(mesh->mName.C_Str()), DirectoryTreeNodeType::CollectionAsset);
						}
					}
				}
			}
			else
			{
				AssetType assetType = AssetType::Unspecified;
				if (dir_entry.path().extension().native() == L".json")
				{
					std::ifstream in(dir_entry.path());
					const json data = json::parse(in);
					assetType = data["AssetType"].get<AssetType>();
				}
				directoryTree->AddNodeByPath(path, DirectoryTreeNodeType::File, assetType);
			}
		}
	}
}

auto AssetManager::IsAssetCollectionExtension(const Path& extension) const -> bool
{
	return extension == Path(".fbx") || extension == Path(".obj");
}

auto AssetManager::LoadStaticMesh(const Path& path)->StaticMesh*
{
	auto res = LoadedAssetsMap.find(path);
	if (res != LoadedAssetsMap.end())
	{
		return dynamic_cast<StaticMesh*>(res->second);
	}

	StaticMesh* sm = new StaticMesh();

	sm->fullPath = path;

	if (!sm->Load())
	{
		return nullptr;
	}

	LoadedAssetsMap.insert({ path, sm });

	return sm;
}

auto AssetManager::LoadAlbedoTexture(const Path& path) -> AlbedoTexture*
{	
	auto res = LoadedAssetsMap.find(path);
	if (res != LoadedAssetsMap.end())
	{
		auto* stuff = res->second;
		return dynamic_cast<AlbedoTexture*>(res->second);
	}

	AlbedoTexture* at = new AlbedoTexture();

	at->fullPath = path;

	if (!at->Load())
	{
		//TODO: return white tex here

		return nullptr;
	}

	LoadedAssetsMap.insert({ path, at });

	return at;
}

auto AssetManager::LoadNormalTexture(const Path& path) -> NormalTexture*
{
	auto res = LoadedAssetsMap.find(path);
	if (res != LoadedAssetsMap.end())
	{
		auto* stuff = res->second;
		return dynamic_cast<NormalTexture*>(res->second);
	}

	NormalTexture* nt = new NormalTexture();

	nt->fullPath = path;

	if (!nt->Load())
	{
		//TODO: return white tex here

		return nullptr;
	}

	LoadedAssetsMap.insert({ path, nt });

	return nt;
}
