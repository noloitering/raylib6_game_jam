#pragma once

#include <memory>
#include <variant>
#include <cmath>

#include "raylib.h"

enum class BuildingType {NONE=0, MONUMENT=1};
enum class BuildingState {CONSTRUCTION=0, BUILT=1, ACTIVE=2};
enum class WorkerState {ROAM=0, WALK=1, HEAL=2};
enum class SpawnType {ENEMY=0, WORKER=1};
const std::vector< float > BUILDINGCOSTS = {0.0f, 20.0f};

class CInterface
{
public:
	bool owned = false;
};
	
class CTransform2D : public CInterface
{
public:
	CTransform2D(const Vector2 &p = Vector2{0.0f, 0.0f}, const Vector2 &s = Vector2{1.0f, 1.0f}, float a = 0.0f)
		: pos(p), scale(s), angle(a) {}
	Vector2 pos = {0, 0};
	Vector2 scale = {1, 1};
	float angle = 0;
};

class CTransform3D : public CInterface
{
public:
	CTransform3D(const Vector3 &p = Vector3{0.0f, 0.0f, 0.0f}, const Vector3 &s = Vector3{1.0f, 1.0f, 1.0f}, const Vector3 &aAxis = Vector3{0.0f, 1.0f, 0.0f}, float a = 0.0f)
		: pos(p), scale(s), axis(aAxis), angle(a) {}
	Vector3 pos = {0, 0, 0};
	Vector3 scale = {1.0f, 1.0f, 1.0f};
	Vector3 axis = {0.0f, 1.0f, 0.0f};
	float angle = 0.0f;
};

class CModel : public CInterface
{
public:
	// CModel(std::shared_ptr< Model > modelData=nullptr)
		// : model(modelData) {}
	// std::shared_ptr< Model > model = nullptr;
	CModel(std::shared_ptr< Model > m=nullptr) 
		: model(m) {}
	std::shared_ptr< Model > model;
};

class CHealth : public CInterface
{
public:
	CHealth(float full=100.0f, float initial=0.0f)
		: max(full), hp(initial) {}
	float max = 100.0f;
	float hp = 0.0f;
};

class CCost : public CInterface
{
public:
	CCost(float cost=0.0f)
		: mana(cost) {}
	float mana = 0.0f;
};

class CBuilding : public CInterface
{
public:
	CBuilding(BuildingType build=BuildingType::NONE, BuildingState construction=BuildingState::CONSTRUCTION)
		: type(build), state(construction) {}
	BuildingType type = BuildingType::NONE;
	BuildingState state = BuildingState::CONSTRUCTION;
};

class CTown : public CInterface
{
public:
	CTown(unsigned int required=1)
		: influence(required) {}
	unsigned int influence = 1;
};

class CWorker : public CInterface
{
public:
	CWorker(WorkerState working=WorkerState::ROAM)
		: state(working) {}
	WorkerState state = WorkerState::ROAM;
};

class CMove : public CInterface
{
public:
	CMove(float magnitude=80.0f, Vector3 origin = (Vector3){0.0f, 0.0f, 0.0f}, Vector3 direction=(Vector3){0.0f, 0.0f, 0.0f}) 
		: home(origin), move(direction) {speed = magnitude * (1.0f / 60.0f);}
	float speed = 80.0f * (1.0f / 60.0f);
	Vector3 home = (Vector3){0.0f, 0.0f, 0.0f};
	Vector3 move = (Vector3){0.0f, 0.0f, 0.0f};
};

class CSpawner : public CInterface
{
public:
	CSpawner(SpawnType type=SpawnType::ENEMY, size_t rate=0, size_t maxSpawns=5, size_t currentAmount=0)
		: spawn(type), spawnRate(rate), max(maxSpawns), current(currentAmount) {}
	SpawnType spawn = SpawnType::ENEMY;
	size_t spawnRate = 0;
	size_t max = 5;
	size_t current = 0;
};
	
	typedef std::tuple< CTransform2D, CTransform3D, CModel, CHealth, CCost, CBuilding, CWorker, CMove, CSpawner, CTown > Components;
//	typedef std::tuple< std::vector < CTransform > > CompContainer;
//	typedef std::variant< CTransform > Component;
	
class CContainer // to be inherited from
{
protected:
	Components components;
public:
	CContainer() {}
	CContainer(Components c)
		: components(c) {}
	virtual ~CContainer() {}
	
	Components getComponents()
	{
		
		return components;
	}
	
	void setComponents(Components c)
	{
		components = c;
	}
	
	template <class C>
	C& getComponent()
	{
		
		return std::get< C >(components);
	}
	
	template <class C, typename... Args>
	C& addComponent(Args&&... CArgs)
	{
		auto& component = getComponent<C>();
		component = C(std::forward<Args>(CArgs)...);
		component.owned = true;
	
		return component;
	}
	
	template <class C>
	C& addComponent(C& newComponent)
	{
		auto& component = getComponent< C >();
		component = newComponent;
		component.owned = true;
	
		return component;
	}
	
	template <class C>
	bool hasComponent()
	{
	
		return getComponent< C >().owned;
	}
};