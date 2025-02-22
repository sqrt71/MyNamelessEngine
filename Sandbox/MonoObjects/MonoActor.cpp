﻿#include "MonoActor.h"

#include "MonoComponent.h"

#include "../Actor.h"

MonoActor::MonoActor(Actor* actor, const char* nameSpace, const char* className) : NameSpace(nameSpace), ClassName(className)
{
    auto mono = MonoSystem::GetInstance();
    auto klass = mono->FindClass(nameSpace, ClassName.c_str());
    
    Owner = actor;
    void *args [1];
    args [0] = &Owner;

    auto csInstance = mono->CreateClassInstance(klass, false);
    Handle =  mono_gchandle_new(csInstance, true);
    
    MonoMethod* method = mono->GetVirtualMethod("Scripts", BaseClassName, "SetCppInstance", Handle);
    mono->InvokeInstanceMethod(method, Handle, args, nullptr);
    
    mono_runtime_object_init(mono_gchandle_get_target (Handle));
}

void MonoActor::RegisterComponents()
{
    auto mono = MonoSystem::GetInstance();
    MonoMethod* method = MonoSystem::GetInstance()->GetMethod("Scripts", "Actor", "cpp_RegisterComponents");
    mono->InvokeInstanceMethod(method, Handle, nullptr, nullptr);
}

void MonoActor::Init()
{
    auto mono = MonoSystem::GetInstance();
    MonoMethod* method = MonoSystem::GetInstance()->GetMethod("Scripts", "Actor", "cpp_Init");
    mono->InvokeInstanceMethod(method, Handle, nullptr, nullptr);
}

MonoActor::~MonoActor()
{
    if(mono_gchandle_get_target(Handle) != nullptr)
    {
        auto mono = MonoSystem::GetInstance();
        MonoMethod* method = mono->GetVirtualMethod("Scripts", BaseClassName, "Dispose", Handle);
        mono->InvokeInstanceMethod(method, Handle, nullptr, nullptr);
    }
}

void MonoActor::AddComponent(Component* component)
{
    auto type = component->GetComponentType();
    auto monoComp = component->GetMonoComponent();
   
    auto mono = MonoSystem::GetInstance();
    
    void *args [1];
    args [0] = &type;

    //MonoMethod* method = mono->GetVirtualMethod("Scripts", BaseClassName, "AddComponent", mono_gchandle_get_target (Handle));
    MonoMethod* method = mono->GetVirtualMethod("Scripts", BaseClassName, "AddExternalComponent", Handle);
    MonoObject* result = mono->InvokeInstanceMethod(method, Handle, args, nullptr);

    monoComp->ConstructFromCsInstance(result, component);
}

void MonoActor::RemoveComponent(Component* component)
{
}

void MonoActor::Update(float deltaTime)
{
    auto mono = MonoSystem::GetInstance();
    void *args [1];
    args [0] = &deltaTime;
    
    //MonoMethod* method = mono->GetVirtualMethod("Scripts", ClassName, "Update", CsInstance);
    MonoMethod* method = mono->GetMethod(NameSpace.c_str(), ClassName.c_str(), "Update");
    if(method)
    {
        MonoObject* result = mono->InvokeInstanceMethod(method, Handle, args, nullptr);
    }
    
}

void MonoActor::OnBeginPlay()
{
    auto mono = MonoSystem::GetInstance();

    MonoMethod* onBeginPlay = mono->GetMethod(NameSpace.c_str(), ClassName.c_str(), "OnBeginPlay");
    
    if(onBeginPlay)
    {
        MonoObject* result = mono->InvokeInstanceMethod(onBeginPlay, Handle, nullptr, nullptr);
    }

    MonoMethod* onGui = mono->GetMethod(NameSpace.c_str(), ClassName.c_str(), "OnGUI");

    if(onGui)
    {
        mono->InvokeInstanceMethod(onGui, Handle, nullptr, nullptr);
    }
}

const char* MonoActor::GetInheritors()
{
    auto mono = MonoSystem::GetInstance();
    
    MonoMethod* method = mono->GetMethod("Scripts", ClassName.c_str(), "GetInheritors");
    MonoObject* result = mono->InvokeInstanceMethod(method, Handle, nullptr, nullptr);
    auto str = mono_string_to_utf8(mono_object_to_string(result, nullptr));
    return str;
}

MonoObject* MonoActor::GetCsInstance()
{
    return mono_gchandle_get_target (Handle);
}

const std::string& MonoActor::GetNamespace() const
{
    return NameSpace;
}

const std::string& MonoActor::GetClassname() const
{
    return ClassName;
}

void MonoActor::Overlap(Actor* otherActor)
{
    auto mono = MonoSystem::GetInstance();
    void* args[1];
	MonoActor* otherMonoActor = otherActor->GetMonoActor();
	if (!otherMonoActor)
	{
		// avoid calling anything if other actor doesn't have a mono actor
		return;
	}
	args[0] = otherMonoActor->GetCsInstance();

    MonoMethod* method = mono->GetMethod(NameSpace.c_str(), ClassName.c_str(), "Overlap");
    if(method)
    {
        MonoObject* result = mono->InvokeInstanceMethod(method, Handle, args, nullptr);
    }
}
