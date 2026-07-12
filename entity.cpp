#include "entity.h"

void EntityManager::update()
{
	std::unordered_map<std::string, EntityVec> new_map;
	for (auto me : entities)
	{
		for (size_t elemIndex=0; elemIndex < me.second.size(); elemIndex++)
		{
			auto e = me.second[elemIndex];
			if ( e->isAlive() )
			{
				new_map[me.first].push_back(e);
			}
		}
	}
	if (!toAdd.empty())
	{
		for (auto a : toAdd)
		{
			new_map[a->getTag()].push_back(a);
		}
		toAdd.clear();
	}
	entities = new_map;
}

void EntityManager::clear()
{
	entities.clear();
	total = 0;
}

void EntityManager::removeEntity(std::shared_ptr<Entity> entity)
{
	entity->destroy();
}

std::shared_ptr<Entity> EntityManager::addEntity(const char* tag, const char* in)
{
	auto e = std::shared_ptr<Entity>(new Entity(total++, tag, in));
	toAdd.push_back(e);
	return e;
}

EntityVec EntityManager::getEntities()
{
	EntityVec result(total);
	for (auto it=entities.begin(); it != entities.end(); it++) // order by id
	{
		for (std::shared_ptr< Entity > e : it->second)
		{
			result.at(e->getId()) = e;
		}
	}
	EntityVec newVec;
	for (std::shared_ptr< Entity > e : result) // eliminate deleted entities
	{
		if ( e )
		{
			newVec.push_back(e);
		}
	}

	return newVec;
}

std::unordered_map<std::string, EntityVec>& EntityManager::getEntityMap()
{
	
	return this->entities;
}

EntityVec & EntityManager::getEntities(const std::string & tag)
{
	return entities[tag];
}

size_t EntityManager::getTotal()
{
	
	return total;
}

void EntityManager::setEntities(const EntityVec& vec)
{
	clear();
	for (auto elem : vec)
	{
		entities[elem->getTag()].push_back(elem);
		total++;
	}
}

void EntityManager::setEntities(const std::unordered_map<std::string, EntityVec>& map)
{
	clear();
	entities = map;
	for (auto elemTag : entities)
	{
		total += elemTag.second.size();
	}
}

// void EntitySystem::update()
// {
	// entities.update();
	// std::vector< std::shared_ptr< Entity > > damagedMonuments;
	// for ( std::shared_ptr< Entity > building : entities.getEntities("Building") )
	// {
		// if ( building->getComponent< CBuilding >().type == BuildingType::MONUMENT )
		// {
			// CHealth& buildingHP = building->getComponent< CHealth >();
			// if ( buildingHP.hp < buildingHP.max )
			// {
				// damagedMonuments.push_back(building);
			// }
		// }
	// }
	// for ( std::shared_ptr< Entity > worker : entities.getEntities("Worker") )
	// {
		// if ( damagedMonuments.size() )
		// {
			// CTransform3D& workerTransform = worker->getComponent< CTransform3D >();
			// Vector3 monumentPos = damagedMonuments.front()->getComponent< CTransform3D >().pos;
			// Vector3 direction = (Vector3){monumentPos.x - workerTransform.pos.x, monumentPos.y - workerTransform.pos.y, 0.0f};
			// float closestDistance = (direction.x) * (direction.x) + (direction.y) * (direction.y);
			// int closest = 0;
			// for (int i=1; i < damagedMonuments.size();i++)
			// {
				// std::shared_ptr< Entity > damagedMonument = damagedMonuments.at(i);
				// Vector3 damagedMonumentPos = damagedMonument->getComponent< CTransform3D >().pos;
				// direction.x = damagedMonumentPos.x - workerTransform.pos.x;
				// direction.y = damagedMonumentPos.y - workerTransform.pos.y;
				// float distance = (direction.x) * (direction.x) + (direction.y) * (direction.y);
				// if ( distance < closestDistance )
				// {
					// closestDistance = distance;
					// closest = i;
				// }
			// }
			// std::shared_ptr< Entity > closestMonument = damagedMonuments.at(closest);
			// Vector3 closestPos = closestMonument->getComponent< CTransform3D >().pos;
			// closestDistance = std::sqrt(closestDistance);
// //			workerTransform.pos = closestPos;
			// if ( closestDistance <= 30.0f )
			// {
				// CHealth& monumentHealth = closestMonument->getComponent< CHealth >();
				// monumentHealth.hp += 0.25f;
				// if ( monumentHealth.hp >= monumentHealth.max )
				// {
					// monumentHealth.hp = monumentHealth.max;
					// CBuilding& monumentBuilding = closestMonument->getComponent< CBuilding >();
					// std::cout << static_cast<int>(monumentBuilding.state) << std::endl;
					// if ( monumentBuilding.state == BuildingState::CONSTRUCTION )
					// {
						// std::cout << "changing building state" << std::endl;
						// monumentBuilding.state = BuildingState::BUILT;
					// }
					// std::cout << static_cast<int>(monumentBuilding.state) << std::endl;
					// closestMonument->getComponent< CModel >().model = LoadModel("./assets/unit-tower.glb");
					// // TODO: load texture
					// CTransform3D& closestMonumentTransform = closestMonument->getComponent< CTransform3D >();
					// closestMonumentTransform.scale.x *=  3;
					// closestMonumentTransform.scale.y *=  3;
					// closestMonumentTransform.scale.z *=  3;
				// }
			// }
			// else
			// {
				// float moveSpeed = 80.0f * (1.0f / 60.0f);
				// direction = (Vector3){closestPos.x - workerTransform.pos.x, closestPos.y - workerTransform.pos.y, 0.0f};
				// workerTransform.pos.x += (direction.x / closestDistance) * moveSpeed;
				// workerTransform.pos.y += (direction.y / closestDistance) * moveSpeed;
			// }
		// }
	// }
// }

// void EntitySystem::render()
// {
	// for (std::shared_ptr< Entity > entity : entities.getEntities() )
	// {
		// CTransform3D& transform = entity->getComponent< CTransform3D >();
		// CModel& model = entity->getComponent< CModel >();
		// if ( transform.owned && model.owned )
		// {
// //			DrawModel(*(entity->getComponent< CModel >().model.get()), entity->getComponent< CTransform3D >().pos, 1.0f, WHITE);
			// DrawModelEx(model.model, transform.pos, transform.axis, transform.angle, transform.scale, WHITE);
		// }
	// }
// }

// void EntitySystem::initialize()
// {
	// std::shared_ptr< Entity > entity = entities.addEntity("Worker", "Goblin");
// //	std::shared_ptr< Model > model3D = std::make_shared< Model >(LoadModel("./assets/character-orc.glb"));
	// entity->addComponent< CTransform3D >((Vector3){0.0f, -360.0f, 0.0f}, Vector3{25.0f, 25.0f, 25.0f});
	// CModel& modelComponent = entity->addComponent< CModel >();
// //	modelComponent.model = std::make_shared< Model >(LoadModel("./assets/character-orc.glb"));
	// modelComponent.model = LoadModel("./assets/character-orc.glb");
// }