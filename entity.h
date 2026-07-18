#ifndef GAME_ENTITIES_H
#define GAME_ENTITIES_H

#include <unordered_map>
#include <vector>
#include <iostream>
#include "component.h"
#include "../../libs/NoGUI/src/Core.h"
//#include "../../libs/NoMVC/src/Model.h"

class Entity : public CContainer, public NoMAD::GameObj
{
protected:
	bool active = true;
public:
	Entity(const size_t & i, const char* t, const char* in="", bool a=true)
		: NoMAD::GameObj(i, t, in), active(a) {}
	void destroy()
	{
		active = false;
	}
	
	bool isAlive()
	{
		
		return active;
	}
};
	
typedef std::vector<std::shared_ptr< Entity > > EntityVec;

class EntityManager
{
private:
	EntityVec toAdd;
	std::unordered_map<std::string, EntityVec> entities;
	size_t total = 0;
public:
	EntityManager() {}
	void clear();
	void update();
	void removeEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> addEntity(const char* tag, const char* in="");
	size_t getTotal();
	EntityVec getEntities(); // in order of ID
	EntityVec& getEntities(const std::string& tag); // in map order
	std::unordered_map<std::string, EntityVec>& getEntityMap();
	void setEntities(const EntityVec& vec);
	void setEntities(const std::unordered_map<std::string, EntityVec>& map);
};

// EVENTS
enum class EntityEvent {DESTROY=0, BUILD=1};
class Listener
{
public:
	Listener () {}
	virtual ~Listener() {}
	virtual void onNotify(std::shared_ptr< Entity > entity, EntityEvent event) = 0;
};

class Notifier
{
private:
	std::vector< std::shared_ptr< Listener > > listeners;
	size_t total = 0;
protected:
	void notify(std::shared_ptr< Entity > entity, EntityEvent event)
	{
		for (std::shared_ptr< Listener > listener : listeners)
		{
			listener->onNotify(entity, event);
		}
	}
public:
	size_t addListener(std::shared_ptr< Listener > listener)
	{
		listeners.push_back(listener);
		
		return total++;
	}
	
	void removeListener(size_t pos)
	{
		listeners.erase(listeners.begin() + pos - 1);
		total--;
	}
	
	std::vector< std::shared_ptr< Listener > > getListeners()
	{
		
		return listeners;
	}
};

#endif