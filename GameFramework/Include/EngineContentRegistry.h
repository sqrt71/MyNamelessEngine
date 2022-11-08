#pragma once

#include <wrl/client.h>

#include <map>
#include <string>
#include <vector>

class Actor;
class Game;
class MeshProxy;
class VertexShader;
class PixelShader;

struct Transform;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;

using namespace Microsoft::WRL;

template<class T>
class Singleton
{
public:
	static auto GetInstance() -> T* { return Instance; }

protected:
	Singleton()
	{
		Instance = static_cast<T*>(this);
	}

	static T* Instance;
};

template<class T>
T* Singleton<T>::Instance = nullptr;

class EngineContentRegistry : public Singleton<EngineContentRegistry>
{
public:

	EngineContentRegistry(Game* InGame);
	~EngineContentRegistry();

	auto CreateBasicActor(const std::string& BasicActorName, const Transform& transform)->Actor*;

	auto CreateBox(const Transform& transform) -> Actor*;
	auto CreatePointLight(const Transform& transform)->Actor*;
	auto CreateSphere(const Transform& transform)->Actor*;

	auto CreateNormalMapTextureFromFile(const wchar_t* fileName, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView) -> void;

	auto GetWhiteTexSRV()->ComPtr<ID3D11ShaderResourceView> { return WhiteTexSRV; }
	auto GetBasicNormalTexSRV()->ComPtr<ID3D11ShaderResourceView> { return BasicNormalTexSRV; }

	auto GetBasicActorNames() const-> const std::vector<std::string>& { return BasicActorNames; }

private:

	typedef Actor* (EngineContentRegistry::*CreateFuncType)(const Transform& transform);
	std::map<std::string, CreateFuncType> CreateActionsMap;
	std::vector<std::string> BasicActorNames;

private:

	Game* MyGame;

	ComPtr<ID3D11Resource> WhiteTex;
	ComPtr<ID3D11ShaderResourceView> WhiteTexSRV;

	ComPtr<ID3D11Resource> BasicNormalTex;
	ComPtr<ID3D11ShaderResourceView> BasicNormalTexSRV;

	MeshProxy* TexturedBoxMeshProxy;
	MeshProxy* SphereMeshProxy;

	VertexShader* DefaultVertexShader;
	PixelShader* DefaultPixelShader;
};