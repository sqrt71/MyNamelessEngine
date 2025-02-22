#pragma once
#include <string>
#include <vector>
#include <filesystem>

#include <iterator>

#include "JsonInclude.h"

using Path = std::filesystem::path;

enum class AssetType
{
	Unspecified,
	BehaviorTree,
	Level
};

NLOHMANN_JSON_SERIALIZE_ENUM(AssetType, {
	{AssetType::Unspecified, "Unspecified"},
	{AssetType::BehaviorTree, "BehaviorTree"},
	{AssetType::Level, "Level"}
})

enum class DirectoryTreeNodeType
{
	// Root, //?
	Directory, // this has to be the first 
	AssetCollection, // this has to be the second
	File,
	CollectionAsset,
	Max // this has to be the last
};

class DirectoryTreeNode{
	friend class DirectoryTree;
private:
	Path name;
	DirectoryTreeNodeType nodeType = DirectoryTreeNodeType::File;
	AssetType assetType = AssetType::Unspecified;

	DirectoryTreeNode* parent = nullptr;
	std::vector<DirectoryTreeNode*> children = {};

	size_t counts[static_cast<size_t>(DirectoryTreeNodeType::Max)];
private:
	auto AddChildNode(DirectoryTreeNode* node)->void;
	DirectoryTreeNode(Path p, DirectoryTreeNodeType nodeType);

public: 
	auto GetName() const -> const Path& { return name; }
	//auto GetDirectoryType() const -> DirectoryTreeNodeType { return nodeType; }
	auto AddFile(DirectoryTreeNode* leaf)->void;

	auto GetAssetType() const -> AssetType { return assetType; }
public:
	
	auto GetChildren() const -> const std::vector<DirectoryTreeNode*>& { return children; }

	auto GetDirectChildByName(const Path& name) const -> DirectoryTreeNode*;

	auto IsDirectory() const -> bool { return nodeType == DirectoryTreeNodeType::Directory; }
	auto IsDirectoryOrAssetCollection() const -> bool { return nodeType == DirectoryTreeNodeType::Directory || nodeType == DirectoryTreeNodeType::AssetCollection; }
	auto IsAssetCollection() const -> bool { return nodeType == DirectoryTreeNodeType::AssetCollection; }
	auto IsAssetFromCollection() const -> bool { return nodeType == DirectoryTreeNodeType::CollectionAsset; }

	auto GetPathFromTreeRoot() const->Path;

	auto IsLeafNode() const -> bool { return GetNumDirectories() == 0; }

	auto GetNumDirectories() const -> size_t { return counts[static_cast<size_t>(DirectoryTreeNodeType::Directory)] + counts[static_cast<size_t>(DirectoryTreeNodeType::AssetCollection)]; }

	~DirectoryTreeNode();
};


class DirectoryTree 
{
	friend class AssetManager;
private:

	DirectoryTreeNode* root;
	auto AddNodeByPath(const Path& path, DirectoryTreeNodeType nodeType, AssetType assetType = AssetType::Unspecified) -> DirectoryTreeNode*;
	
public:

	DirectoryTree(Path path) { root = new DirectoryTreeNode(path, DirectoryTreeNodeType::Directory); }
	auto GetRootNode() const -> DirectoryTreeNode* { return root; }

	auto GetDirectoryByPath(const Path& path)->DirectoryTreeNode*;

	~DirectoryTree();

	// TODO:
	//class DirectoryOnlyIterator
	//{
	//	friend class DirectoryTree;
	//	DirectoryTreeNode* node;
	//	std::vector<DirectoryTreeNode*> stack;
	//	explicit DirectoryOnlyIterator(DirectoryTreeNode* _node = nullptr) : node(_node) { stack.push_back(node); }
	//public:
	//	auto operator++() -> DirectoryOnlyIterator&;
	//	//DirectoryOnlyIterator operator++(int) { DirectoryOnlyIterator retval = *this; ++(*this); return retval; }
	//	auto operator==(const DirectoryOnlyIterator& other) const -> bool { return node == other.node; }
	//	bool operator!=(const DirectoryOnlyIterator& other) const { return !(*this == other); }
	//	DirectoryTreeNode* operator*() const { return node; }

	//	auto Pop() -> void{ if (node) node = node->parent; }
	//};

	//auto GetDirectoryIterator() -> DirectoryOnlyIterator { return DirectoryOnlyIterator(GetRootNode()); }

	//auto end() -> DirectoryOnlyIterator { return DirectoryOnlyIterator(nullptr); }
};

