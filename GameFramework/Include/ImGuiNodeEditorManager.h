#pragma once

#include <vector>
#include <filesystem>
using Path = std::filesystem::path;

#include "ImGuiInclude.h"
#include "JsonInclude.h"
#include "ImGuiNodeEditorInclude.h"
#include "imgui_internal.h"

#include <memory>

class ImGuiSubsystem;

enum class PinType
{
	Flow
};

enum class PinKind
{
	Output,
	Input
};

enum class NodeType
{
	Tree,
	Comment
};

NLOHMANN_JSON_SERIALIZE_ENUM(NodeType, {
	{NodeType::Tree, "Tree"},
	{NodeType::Comment, "Comment"}
})

enum class NodeKind
{
	Root,
	Sequence,
	Selector,
	Task
};

NLOHMANN_JSON_SERIALIZE_ENUM(NodeKind, {
	{NodeKind::Root, "Root"},
	{NodeKind::Sequence, "Sequence"},
	{NodeKind::Selector, "Selector"},
	{NodeKind::Task, "Task"}
	})

struct Node;

struct Pin
{
	ned::PinId ID;
	std::string Name;
	PinType     Type;
	PinKind     Kind;
	Node* Node;

	Pin(ned::PinId id, const char* name, PinType type) :
		ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
	{
	}
};

struct Node {
	ned::NodeId ID;
	std::string Name;
	ImColor Color;
	NodeType Type;
	NodeKind Kind;
	ImVec2 Size;
	int Ordinal;
	json taskData;
	// need to add stuff that will allow to find out what type of node it is : root, sequence, selector, task (for task we need to remember what task it is
	// mb just a json object?

	std::vector<Pin> Inputs;
	std::vector<Pin> Outputs;

	Node(ned::NodeId id, const char* name, ImColor color = ImColor(255, 255, 255)) :
		ID(id), Name(name), Color(color), Type(NodeType::Tree), Size(0, 0), Kind(NodeKind::Selector), Ordinal(-1)
	{
	}
};

struct Link
{
	ned::LinkId ID;

	ned::PinId StartPinID;
	ned::PinId EndPinID;

	ImColor Color;

	Link(ned::LinkId id, ned::PinId startPinId, ned::PinId endPinId) :
		ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
	{
	}
};

struct NodeEditorData
{
	Path path;
	ned::EditorContext* context;

	std::vector<Node> nodes;
	std::vector<Link> links;

	ImGuiWindowClass windowClass;

	ned::Config config;

	bool isDirty = false;
	bool shouldRemainOpen = true;

	std::string editorData;

	// todo: should the following be here? - prolly yes
	bool createNewNode = false;
	Pin* newNodeLinkPin = nullptr;
	Pin* newLinkPin = nullptr;
	ned::NodeId contextNodeId = 0;
	ned::LinkId contextLinkId = 0;
	ned::PinId  contextPinId = 0;

	uintptr_t nextId = 1;

	~NodeEditorData();
};

static inline ImRect ImGui_GetItemRect()
{
	return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

class ImGuiNodeEditorManager
{
public:
	
	// Should the path be here or
	auto OpenEditor(const Path& path) -> void;

	

	auto DrawOpenEditors() -> void;

private:

	std::vector<std::unique_ptr<NodeEditorData>> openEditors;

	json taskTypes;

protected:

	auto GetImGuiSubsystem() -> ImGuiSubsystem&;

	auto GenerateWindowName(NodeEditorData& nodeEditorData) const->std::string;
	auto GenerateNodeEditorWindowName(NodeEditorData& nodeEditorData) const->std::string;
	auto GenerateDetailsWindowName(NodeEditorData& nodeEditorData) const->std::string;
	auto GenerateToolbarWindowName(NodeEditorData& nodeEditorData) const->std::string;

protected:

	auto SpawnRootNode(NodeEditorData& nodeEditorData, ned::NodeId nodeId = 0, ned::PinId outPinId = 0) -> Node&;
	auto SpawnSequenceNode(NodeEditorData& nodeEditorData, ned::NodeId nodeId = 0, ned::PinId inPinId = 0, ned::PinId outPinId = 0) -> Node&;
	auto SpawnSelectorNode(NodeEditorData& nodeEditorData, ned::NodeId nodeId = 0, ned::PinId inPinId = 0, ned::PinId outPinId = 0) -> Node&;
	auto SpawnTaskNode(NodeEditorData& nodeEditorData, const json& taskData, ned::NodeId nodeId = 0, ned::PinId inPinId = 0) -> Node&;
protected:

	auto GetNextId(NodeEditorData& data) -> int { return data.nextId++; }

	auto DrawBehaviorTreeEditorWindow(NodeEditorData& nodeEditorData) -> bool;
	auto DrawNodeEditorWindow(NodeEditorData& nodeEditorData) -> void;
	auto DrawDetailsWindow(NodeEditorData& nodeEditorData) -> void;
	auto DrawToolbarWindow(NodeEditorData& nodeEditorData) -> void;
	auto DrawDockspace(NodeEditorData& nodeEditorData) -> void;

	auto DrawNode(NodeEditorData& nodeEditorData, Node& node) -> void;

	auto TryCreateNewLink(NodeEditorData& nodeEditorData) -> void;

	auto TryDeleteNodesAndLinks(NodeEditorData& nodeEditorData) -> void;

	auto DrawNodeContextMenuPopup(NodeEditorData& nodeEditorData) -> void;
	auto DrawPinContextMenuPopup(NodeEditorData& nodeEditorData) -> void;
	auto DrawLinkContextMenuPopup(NodeEditorData& nodeEditorData) -> void;
	auto DrawCreateNodeContextMenuPopup(NodeEditorData& nodeEditorData, const ImVec2& newNodePosition) -> void;

	auto UpdateNodeOrdinals(NodeEditorData& data) -> void;
	auto UpdateNodeOrdinals_Exec(NodeEditorData& data, Node* curNode, int& curOrdinal) -> void;

	auto Load(NodeEditorData& data) -> bool;
	auto Save(NodeEditorData& data) -> void;
};