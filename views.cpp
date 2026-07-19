#include "views.h"

Vector3 convert2DPos3D(Vector2 position2D)
{
	
	return (Vector3){(position2D.x - 360.0f) * std::sin(CAMERAANGLE) * -1, (position2D.y * -1 + 360.0f), 0.0f};
}

void Scene::setVictory(bool win)
{
	if ( win )
	{
		std::shared_ptr< NoGUI::Manager > gui = dynamic_pointer_cast< NoGUI::Manager >(getModel((size_t)GameModels::GUI));
		gui->setEnabled(Overlay::VICTORY);
		state = GameState::VICTORY;
	}
	else
	{
		std::shared_ptr< NoGUI::Manager > gui = dynamic_pointer_cast< NoGUI::Manager >(getModel((size_t)GameModels::GUI));
		std::shared_ptr< GameGrid > grid = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
		std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
		std::shared_ptr< GameResources > resources = dynamic_pointer_cast< GameResources >(getModel((size_t)GameModels::RESOURCES));
		gui->getPage(Overlay::RESOURCES)->setEnabled(true);
		gui->getPage(Overlay::TABS)->setEnabled(true);
		gui->getPage(Overlay::BUILDINGS)->setEnabled(false);
		gui->getPage(Overlay::VICTORY)->setEnabled(false);
		grid->clear();
		for ( std::shared_ptr< Entity > entity : entities->entities.getEntities() )
		{
			entity->destroy();
		}
		resources->mana = 0;
		resources->maxMana = 100.0f;
		resources->manaGen = 0.05f;
		resources->workers = 1;
		resources->maxWorkers = 5;
		resources->units = 0;
		resources->maxUnits = 2;
		grid->initialize();
		grid->update();
		entities->update();
		entities->initialize();
		entities->update();
		initialize();
		entities->update();
		state = GameState::RUNNING;
	}
}

std::vector< std::shared_ptr< NoGUI::Element > > Scene::getSurrondingCells(std::shared_ptr< NoGUI::Element > centerElem, unsigned int radius)
{		
	std::vector< std::shared_ptr< NoGUI::Element > > surrondingElems;
	std::shared_ptr< NoGUI::Manager > gui = dynamic_pointer_cast< NoGUI::Manager >(getModel((size_t)GameModels::GRID));
	std::shared_ptr< NoGUI::Page > grid = gui->getPage((size_t)GameGrid::GRID);
	size_t center = centerElem->getId();
	int centerRow = center / (CELLSX + 1);
	bool centerShifted = (centerRow + 1) % 2 == 0;
	int minRow = (centerRow >= radius) ?  centerRow - radius : 0;
	int maxRow = (center + (CELLSX + 1) * radius < grid->getElements().size() - 1) ? centerRow + radius : (grid->getElements().size() - 1) / (CELLSX + 1);
	// get hexes around center
	int index = center - (CELLSX + 1) * centerRow;
	for (int i=1; i <= radius; i++)
	{
		if ( index - i >= 0 )
		{
			surrondingElems.push_back(grid->getElement(center - i));
		}
		if ( index + i < (CELLSX + 1) )
		{
			surrondingElems.push_back(grid->getElement(center + i));
		}
	}
	// do rows above center
	for (int rowInc=1; rowInc <= maxRow - centerRow; rowInc++)
	{
		int rowCenter = center + (CELLSX + 1) * rowInc;
		surrondingElems.push_back(grid->getElement(rowCenter));
		int rowWidth = radius * 2 + 1 - rowInc;
		int left = rowWidth / 2;
		int right = rowWidth / 2;
		if ( rowInc % 2 != 0 )
		{
			if ( centerShifted )
			{
				left -= 1;
			}
			else
			{
				right -= 1;
			}
		}
		for (int i=1; i <= left; i++)
		{
			if ( index - i >= 0 )
			{
				surrondingElems.push_back(grid->getElement(rowCenter - i));
			}
		}
		for (int i=1; i <= right; i++)
		{
			if ( index + i < (CELLSX + 1) )
			{
				surrondingElems.push_back(grid->getElement(rowCenter + i));
			}
		}
	}
	// do rows below center
	for (int rowInc=1; rowInc <= centerRow - minRow; rowInc++)
	{
		int rowCenter = center - (CELLSX + 1) * rowInc;
		surrondingElems.push_back(grid->getElement(rowCenter));
		int rowWidth = radius * 2 + 1 - rowInc;
		int left = rowWidth / 2;
		int right = rowWidth / 2;
		if ( rowInc % 2 != 0 )
		{
			if ( centerShifted )
			{
				left -= 1;
			}
			else
			{
				right -= 1;
			}
		}
		for (int i=1; i <= left; i++)
		{
			if ( index - i >= 0 )
			{
				surrondingElems.push_back(grid->getElement(rowCenter - i));
			}
		}
		for (int i=1; i <= right; i++)
		{
			if ( index + i < (CELLSX + 1) )
			{
				surrondingElems.push_back(grid->getElement(rowCenter + i));
			}
		}
	}
		
	return surrondingElems;
}

void Scene::spawnEntities()
{
	std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
	std::shared_ptr< GameClock > clock = dynamic_pointer_cast< GameClock >(getModel((size_t)GameModels::CLOCK));
	std::shared_ptr< GameResources > resources = dynamic_pointer_cast< GameResources >(getModel((size_t)GameModels::RESOURCES)); // TODO: maybe not use this, instead use the max value of the spawner
	for (std::shared_ptr< Entity > entity : entities->entities.getEntities())
	{
		CSpawner& spawner = entity->getComponent< CSpawner >();
		if ( spawner.owned )
		{
			if ( spawner.spawnRate > 0 && clock->getFrame() % spawner.spawnRate == 0 )
			{
				switch ( spawner.spawn )
				{
					case SpawnType::ENEMY:
					{
						if ( spawner.current < spawner.max )
						{
							std::shared_ptr< Entity > enemy = entities->entities.addEntity("Enemy", "Human");
							enemy->addComponent< CTransform3D >(entity->getComponent< CTransform3D >().pos, Vector3{25.0f, 25.0f, 25.0f}, Vector3{0.0f, 1.0f, 0.0f}, 180.0f);
							enemy->addComponent< CWorker >();
							enemy->addComponent< CHealth >(100.0f, 100.0f);
							enemy->addComponent< CMove >(80.0f, entity->getComponent< CTransform3D >().pos);
							CModel& modelComponent = enemy->addComponent< CModel >();
							modelComponent.model = game->assets->get< Model >("enemy");
							spawner.current++;
						}
					
						break;
					}
					
					case SpawnType::WORKER:
					{
						if ( resources->workers < resources->maxWorkers )
						{
							std::shared_ptr< Entity > worker = entities->entities.addEntity("Worker", "Goblin");
							worker->addComponent< CTransform3D >(entity->getComponent< CTransform3D >().pos, Vector3{25.0f, 25.0f, 25.0f});
							worker->addComponent< CWorker >();
							worker->addComponent< CHealth >(50.0f, 50.0f);
							worker->addComponent< CMove >(80.0f, entity->getComponent< CTransform3D >().pos);
							CModel& modelComponent = worker->addComponent< CModel >();
							modelComponent.model = game->assets->get< Model >("worker");
							resources->workers++;
						}
						
						break;
					}
				}
			}
		}
	}
}

void Scene::animateElements()
{
	std::shared_ptr< GameGrid > grid = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
	std::shared_ptr< Overlay > gui = dynamic_pointer_cast< Overlay >(getModel((size_t)GameModels::GUI));
	std::shared_ptr< GameClock > clock = dynamic_pointer_cast< GameClock >(getModel((size_t)GameModels::CLOCK));
	unsigned char endFrame = 75;
	unsigned char framesRemain = clock->getFrame() % endFrame;
	if ( framesRemain == 0 )
	{
		grid->convertingSwampFill->col = grid->cellFill->col;
		grid->convertingSwampFill->hoverCol = grid->cellFill->col;
		grid->convertingPortalFill->col = grid->townFill->col;
		grid->convertingPortalFill->hoverCol = grid->townFill->col;
	}
	else
	{
		float alphaPercent = (float)framesRemain / endFrame;
		grid->convertingSwampFill->col = (Color){grid->swampFill->col.r, grid->swampFill->col.g, grid->swampFill->col.b, static_cast<unsigned char>(255 * alphaPercent)};
		grid->convertingSwampFill->hoverCol = (Color){grid->swampFill->col.r, grid->swampFill->col.g, grid->swampFill->col.b, static_cast<unsigned char>(255 * alphaPercent)};
		grid->convertingPortalFill->col = (Color){grid->portalFill->col.r, grid->portalFill->col.g, grid->portalFill->col.b, static_cast<unsigned char>(255 * alphaPercent)};
		grid->convertingPortalFill->hoverCol = (Color){grid->portalFill->col.r, grid->portalFill->col.g, grid->portalFill->col.b, static_cast<unsigned char>(255 * alphaPercent)};
	}
	std::shared_ptr< NoGUI::Slider > noManaBar = dynamic_pointer_cast< NoGUI::Slider >(gui->getPage(Overlay::RESOURCES)->getElements("Mana").back());
	if ( noManaBar->getShape()->fill->col.a > 0 )
	{
		float alphaPercent = (float)framesRemain / (endFrame - 1);
		noManaBar->getShape()->fill->col.a = static_cast<unsigned char>(255 * (1 - alphaPercent));
		noManaBar->getShape()->outline->fill->col.a = static_cast<unsigned char>(255 *  (1 - alphaPercent));
	}
}

void Scene::placeMonument(std::shared_ptr< Tile > tile)
{
	std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
	std::shared_ptr< GameGrid > gui = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
	// add building entity
	std::shared_ptr< Entity > building = entities->entities.addEntity("Building", "Monument");
	building->addComponent< CBuilding >(BuildingType::MONUMENT);
	building->addComponent< CCost >(BUILDINGCOSTS.at(static_cast<int>(BuildingType::MONUMENT)));
	building->addComponent< CHealth >();
	Vector3 elemPosition3D = convert2DPos3D(tile->position);
	// scale
	float scaleFactorX = tile->width() * camera.position.y / 1000.0f;
	float scaleFactorY = scaleFactorX - 3.0f;
	float scaleFactorZ = scaleFactorY - 4.0f;
	building->addComponent< CTransform3D >(elemPosition3D, Vector3{scaleFactorX, scaleFactorY, scaleFactorZ});
	CModel& buildingModel = building->addComponent< CModel >();
	buildingModel.model = game->assets->get< Model >("construction");
	// 2d position
	int row = tile->getId() / CELLSX;
	int column = tile->getId() % CELLSX;
	building->addComponent< CTransform2D >((Vector2){(float)column, (float)row});
	// modify tile
	tile->setShape(gui->monumentShape);
	tile->building = building;
	for ( std::shared_ptr< NoGUI::Element > cell : getSurrondingCells(tile, 2) )
	{
		if ( TextLength(cell->getInner()) == 0 )
		{
			cell->setShape(gui->convertingSwampShape);
		}
	}
}

void Scene::placePortal(std::shared_ptr< Tile > tile)
{
	std::shared_ptr< GameResources > resources = dynamic_pointer_cast< GameResources >(getModel((size_t)GameModels::RESOURCES));
	std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
	std::shared_ptr< GameGrid > grid = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
	std::shared_ptr< NoGUI::Page > gridPage = grid->getPage(GameGrid::GRID);
	tile->setShape(grid->portalShape);
	tile->setInner("Portal");
	if ( tile->building == nullptr )
	{
		tile->building = entities->entities.addEntity("Town");
		tile->building->addComponent< CModel >();
		tile->building->addComponent< CTransform3D >(convert2DPos3D(tile->position));
		int row = tile->getId() / CELLSX;
		int column = tile->getId() % CELLSX;
		tile->building->addComponent< CTransform2D >((Vector2){(float)column, (float)row});
		tile->building->addComponent< CSpawner >();
	}
	tile->building->getComponent< CModel >().model = game->assets->get< Model >("portal");
	tile->building->getComponent< CTransform3D >().scale = portalSize;
	tile->building->getComponent< CTransform3D >().angle = 180.0f;
	tile->building->getComponent< CSpawner >().spawnRate = 5 * 60;
	tile->building->getComponent< CSpawner >().spawn = SpawnType::WORKER;
	tile->building->getComponent< CTown >().owned = false;
	resources->maxWorkers += 5;
}

std::vector< std::shared_ptr< NoGUI::Element > > Scene::buildMonument(std::shared_ptr< Tile > tile)
{
	// convert land
	std::shared_ptr< GameGrid > grid = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
	std::shared_ptr< NoGUI::Page > gridPage = grid->getPage(static_cast<int>(GameGrid::GRID));
	if ( tile->building == nullptr )
	{
		placeMonument(tile);
	}
	tile->building->getComponent< CModel >().model = game->assets->get< Model >("obelisk");
	CTransform3D& buildingTransform = tile->building->getComponent< CTransform3D >();
	buildingTransform.scale.x *=  3;
	buildingTransform.scale.y *=  3;
	buildingTransform.scale.z *=  3;
	std::vector< std::shared_ptr< NoGUI::Element > > nearbyTowns;
	for (std::shared_ptr< NoGUI::Element > cell : getSurrondingCells(tile, 2))
	{
		if ( TextLength(cell->getInner()) == 0 )
		{ 
			cell->setInner("Swamp");
			cell->setShape(grid->swampShape);
		}
		else if ( cell->getShape() == grid->townShape )
		{
			nearbyTowns.push_back(cell);
		}
	}
	tile->building->getComponent< CBuilding >().state = BuildingState::ACTIVE;
	
	return nearbyTowns; // TODO: handle the vector rather than just retruning
}

void Scene::render() 
{
	std::shared_ptr< GameGrid > grid = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
	std::shared_ptr< Overlay > gui = dynamic_pointer_cast< Overlay >(getModel((size_t)GameModels::GUI));
	std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
	animateElements();
	grid->render();
	BeginMode3D(camera);
		entities->render();
	EndMode3D();
	gui->render();
}

void Scene::run()
{
	render();
	std::shared_ptr< GameClock > clock = dynamic_pointer_cast< GameClock >(getModel((size_t)GameModels::CLOCK));
	std::shared_ptr< GameResources > resources = dynamic_pointer_cast< GameResources >(getModel((size_t)GameModels::RESOURCES));
	std::shared_ptr< GameGrid > grid = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
	std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
	std::shared_ptr< NoGUI::Page > gridPage = grid->getPage(GameGrid::GRID);
	if ( state == GameState::RUNNING )
	{
		// spawning
		spawnEntities();
		// movement
		// TODO: probably could be handled in the entity system
		for (std::shared_ptr< Entity > worker : entities->entities.getEntities("Worker"))
		{
			if ( worker->getComponent< CWorker >().state == WorkerState::ROAM )
			{
				CTransform3D& workerTransform = worker->getComponent< CTransform3D >();
				CMove& workerMove = worker->getComponent< CMove >();
				Vector3 toHome = (Vector3){workerMove.home.x - workerTransform.pos.x, workerMove.home.y - workerTransform.pos.y, workerMove.home.z - workerTransform.pos.z};
				float distanceToHome = std::sqrt(toHome.x * toHome.x + toHome.y * toHome.y);
				if ( distanceToHome > 150.0f )
				{
//					distanceToHome = std::sqrt(distanceToHome);
					workerMove.move.x = (toHome.x / distanceToHome) * workerMove.speed;
					workerMove.move.y = (toHome.y / distanceToHome) * workerMove.speed;
				}
				else if ( clock->getFrame() % 60 == 0 )
				{
					int randomAngle = GetRandomValue(0, 360);
					Vector2 randomDir = (Vector2){std::cos(randomAngle * PI / 180.0f), std::sin(randomAngle * PI / 180.0f)};
					workerMove.move.x = workerMove.speed * randomDir.x;
					workerMove.move.y = workerMove.speed * randomDir.y;
				}
			}
		}
		for (std::shared_ptr< Entity > worker : entities->entities.getEntities("Enemy"))
		{
			if ( worker->getComponent< CWorker >().state == WorkerState::ROAM )
			{
				CTransform3D& workerTransform = worker->getComponent< CTransform3D >();
				CMove& workerMove = worker->getComponent< CMove >();
				Vector3 toHome = (Vector3){workerMove.home.x - workerTransform.pos.x, workerMove.home.y - workerTransform.pos.y, workerMove.home.z - workerTransform.pos.z};
				float distanceToHome = std::sqrt(toHome.x * toHome.x + toHome.y * toHome.y);
				if ( distanceToHome > 50.0f )
				{
					workerMove.move.x = (toHome.x / distanceToHome) * workerMove.speed;
					workerMove.move.y = (toHome.y / distanceToHome) * workerMove.speed;
				}
				else if ( clock->getFrame() % 60 == 0 )
				{
					int randomAngle = GetRandomValue(0, 360);
					Vector2 randomDir = (Vector2){std::cos(randomAngle * PI / 180.0f), std::sin(randomAngle * PI / 180.0f)};
					workerMove.move.x = workerMove.speed * randomDir.x;
					workerMove.move.y = workerMove.speed * randomDir.y;
				}
			}
		}
//	if ( clock->getFrame() % 60 == 0 )
//	{
		// resources
		resources->mana += resources->manaGen;
		if ( resources->mana > resources->maxMana )
		{
			resources->mana = resources->maxMana;
		}
		if ( IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) )
		{
			currentBuild = BuildingType::NONE;
		}
		if ( IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) )
		{
			currentBuild = BuildingType::NONE;
		}
	}
//	}
	std::shared_ptr< Overlay > gui = dynamic_pointer_cast< Overlay >(getModel((size_t)GameModels::GUI));
	gui->getPage(Overlay::RESOURCES)->getElements().front()->setInner(TextFormat("%.0f", resources->mana));
	std::shared_ptr< NoGUI::Slider > manaBar = dynamic_pointer_cast< NoGUI::Slider >(gui->getPage(Overlay::RESOURCES)->getElements("Mana").front());
	std::shared_ptr< NoGUI::Slider > noManaBar = dynamic_pointer_cast< NoGUI::Slider >(gui->getPage(Overlay::RESOURCES)->getElements("Mana").back());
	manaBar->slideTo(resources->mana);
	noManaBar->slideTo(resources->mana);
	// controls
	if ( IsKeyPressed(KEY_P) )
	{
		switch (state)
		{
			case GameState::RUNNING:
			{
				clock->setPaused(true);
				state = GameState::PAUSED;
				
				break;
			}
			
			case GameState::PAUSED:
			{
				clock->setPaused(false);
				state = GameState::RUNNING;
				
				break;
			}
				
			default:
			{
				
				break;
			}
		}
	}
//	render();
}

void Scene::onNotify(std::shared_ptr< NoGUI::Element > elem, NoGUI::HoverEvent hevent, NoGUI::FocusEvent fevent)
{
	switch (hevent)
	{
		case NoGUI::HoverEvent::ONHOVER:
		{
			if ( !TextIsEqual("Cell", elem->getTag()) && !TextIsEqual("Label", elem->getTag()) )
			{
				std::shared_ptr< NoGUI::Manager > grid = dynamic_pointer_cast< NoGUI::Manager >(getModel((size_t)GameModels::GRID));
				grid->getPage(GameGrid::GRID)->setActive(false);
			}
			
			break;
		}
		
		case NoGUI::HoverEvent::HOVERING:
		{
			
			break;
		}
		
		case NoGUI::HoverEvent::OFFHOVER:
		{
			if ( !TextIsEqual("Cell", elem->getTag()) && !TextIsEqual("Label", elem->getTag()) )
			{
				std::shared_ptr< NoGUI::Manager > grid = dynamic_pointer_cast< NoGUI::Manager >(getModel((size_t)GameModels::GRID));
				grid->getPage(GameGrid::GRID)->setActive(true);
			}
			
			break;
		}
	}
	switch (fevent)
	{	
		case NoGUI::FocusEvent::ONFOCUS:
		{
			if ( TextIsEqual("Swamp", elem->getInner()) )
			{
				std::shared_ptr< GameResources > resources = dynamic_pointer_cast< GameResources >(getModel((size_t)GameModels::RESOURCES));
				float buildingCost = BUILDINGCOSTS[static_cast< int >(currentBuild)];
				if ( resources->mana >= buildingCost )
				{
					resources->mana -= buildingCost;
					std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
					switch (currentBuild)
					{
						case BuildingType::NONE:
						{
						
							break;
						}
						
						case BuildingType::MONUMENT:
						{									
							std::shared_ptr< Tile > tile = dynamic_pointer_cast< Tile >(elem);
							placeMonument(tile);
						
							break;
						}
					}
				}
				else
				{
					// TODO: seperate into own function
					std::shared_ptr< Overlay > gui = dynamic_pointer_cast< Overlay >(getModel((size_t)GameModels::GUI));
					std::shared_ptr< NoGUI::Element > noManaBar = gui->getPage(Overlay::RESOURCES)->getElements("Mana").back();
					noManaBar->getShape()->fill->col.a = 255;
					noManaBar->getShape()->outline->fill->col.a = 255;
					game->sfx->play(game->assets->get< Sound >("nomana.wav"));
				}
			}
				
			break;
		}
			
		case NoGUI::FocusEvent::FOCUSING:
		{
			
			break;
		}
			
		case NoGUI::FocusEvent::OFFFOCUS:
		{
			if ( TextIsEqual("Build", elem->getInner()) )
			{
				std::shared_ptr< NoGUI::Manager > gui = dynamic_pointer_cast< NoGUI::Manager >(getModel((size_t)GameModels::GUI));
				gui->getPage(Overlay::TABS)->setActive(false);
				gui->getPage(Overlay::BUILDINGS)->setEnabled(true);
			}
			else if ( TextIsEqual("Building", elem->getTag()) )
			{
				std::shared_ptr< Overlay > gui = dynamic_pointer_cast< Overlay >(getModel((size_t)GameModels::GUI));
				gui->getPage(Overlay::BUILDINGS)->setEnabled(false);
				if ( TextIsEqual("Monument", elem->getInner()) )
				{
					std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
					currentBuild = BuildingType::MONUMENT;
					gui->getPage(Overlay::TABS)->setActive(true);
				}
			}
			else if ( TextIsEqual("Restart", elem->getTag()) )
			{
				setVictory(false);
			}
			
			break;
		}
	}
}

void Scene::onNotify(std::shared_ptr< Entity > entity, EntityEvent event)
{
	switch (event)
	{
		case EntityEvent::DESTROY:
		{
			if ( TextIsEqual("Worker", entity->getTag()) )
			{
				std::shared_ptr< GameResources > resources = dynamic_pointer_cast< GameResources >(getModel((size_t)GameModels::RESOURCES));
				resources->workers--;
				entity->destroy();
			}
			else if ( TextIsEqual("Building", entity->getTag()) )
			{
				std::shared_ptr< GameGrid > grid = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
				std::shared_ptr< NoGUI::Page > gridPage = grid->getPage(static_cast<int>(GameGrid::GRID));
				// disable tile
				std::shared_ptr< Tile > tile = dynamic_pointer_cast< Tile >(gridPage->getElements("Cell").at(entity->getComponent< CTransform2D >().pos.y * CELLSX + entity->getComponent< CTransform2D >().pos.x));
				entity->destroy(); // building is no longer alive
				tile->building = nullptr;
				tile->setShape(grid->swampShape);
				if ( entity->getComponent< CBuilding >().type == BuildingType::MONUMENT )
				{
					// disable surronding tiles
					std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
					std::vector< std::shared_ptr< NoGUI::Element > > cellsToDisable = getSurrondingCells(tile, 2);
					cellsToDisable.push_back(tile);
					// find intersections
					std::unordered_set< int > intersectingIndexes;
					for (std::shared_ptr< Entity > building : entities->entities.getEntities("Building"))
					{
						if ( building->isAlive() && building->getComponent< CBuilding >().type == BuildingType::MONUMENT )
						{
							std::shared_ptr< Tile > monumentCell = dynamic_pointer_cast< Tile >(gridPage->getElements("Cell").at(building->getComponent< CTransform2D >().pos.y * CELLSX + building->getComponent< CTransform2D >().pos.x));
							std::vector< std::shared_ptr< NoGUI::Element > > surrondingCells = getSurrondingCells(monumentCell, 2);
							surrondingCells.push_back(monumentCell);
							for (int i=0; i < cellsToDisable.size(); i++)
							{
								std::shared_ptr< NoGUI::Element > cellToDisable = cellsToDisable.at(i);
								for (std::shared_ptr< NoGUI::Element > surrondingCell : surrondingCells)
								{
									if ( surrondingCell == cellToDisable )
									{
										intersectingIndexes.insert(i);
									}
								}
							}
						}
					}
					// disable cells that don't intersect
					for (int i=0; i < cellsToDisable.size(); i++)
					{
						if ( intersectingIndexes.count(i) == 0 )
						{
							std::shared_ptr< NoGUI::Element > cellToDisable = cellsToDisable.at(i);
							if ( TextIsEqual("Swamp", cellToDisable->getInner()) || TextLength(cellToDisable->getInner()) == 0 )
							{
								cellToDisable->setShape(grid->cellShape);
								cellToDisable->setInner("");
							}
						}
					}
				}
				
			}
			
			break;
		}
			
		case EntityEvent::BUILD:
		{
			if ( entity->hasComponent< CBuilding >() )
			{
				CBuilding& entityBuild = entity->getComponent< CBuilding >();
				switch ( entityBuild.type )
				{
					case BuildingType::MONUMENT:
					{
						std::shared_ptr< GameGrid > grid = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
						std::shared_ptr< NoGUI::Page > gridPage = grid->getPage(static_cast<int>(GameGrid::GRID));
						Vector2 monumentPos = entity->getComponent< CTransform2D >().pos;
						std::shared_ptr< Tile > monumentTile = dynamic_pointer_cast< Tile >(gridPage->getElement(monumentPos.y * CELLSX + monumentPos.x));
						std::vector< std::shared_ptr< NoGUI::Element > > nearbyTowns = buildMonument(monumentTile);
						// convert towns
						for (int townIndex=0; townIndex < nearbyTowns.size(); townIndex++)
						{
							std::shared_ptr< Tile > townTile = dynamic_pointer_cast< Tile >(nearbyTowns.at(townIndex));
							std::shared_ptr< Entity > town = townTile->building;
							std::vector< std::shared_ptr< NoGUI::Element > > monumentCells;
							std::vector< std::shared_ptr< NoGUI::Element > > surrondingCells = getSurrondingCells(townTile, 2);
							for (int cellIndex=0; cellIndex < surrondingCells.size(); cellIndex++)
							{
								std::shared_ptr< Tile > cell = dynamic_pointer_cast< Tile >(surrondingCells.at(cellIndex));
								if ( cell->building )
								{
									CBuilding& cellBuilding = cell->building->getComponent< CBuilding >();
									if ( cellBuilding.type == BuildingType::MONUMENT && cellBuilding.state != BuildingState::CONSTRUCTION )
									{
										monumentCells.push_back(cell);
									}
								}
							}
							if ( town )
							{
								CTown& townComponent = town->getComponent< CTown >();
								if ( townComponent.owned && monumentCells.size() >= townComponent.influence )
								{
									placePortal(townTile);
									for (int i=0; i < monumentCells.size(); i++)
									{
										std::shared_ptr< Tile > monumentCell = std::dynamic_pointer_cast< Tile >(monumentCells.at(i));
										monumentCell->setShape(grid->swampShape);
										// destroy monument entities
										monumentCell->building->destroy();
										monumentCell->building = nullptr;
									}
									if ( town->getComponent< CTown >().influence > 5 )
									{
										setVictory(true);
									}
								}
							}
						}
					
						break;
					}
				
					default:
					{
			
						break;
					}
				}
			}
				
			break;
		}
	}
}

void Scene::initialize()
{
	std::shared_ptr< GameGrid > gui = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
	gui->update();
	std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
	std::shared_ptr< Entity > entity = entities->entities.addEntity("Worker", "Goblin");
	entity->addComponent< CTransform3D >((Vector3){0.0f, -360.0f, 0.0f}, Vector3{25.0f, 25.0f, 25.0f});
	entity->addComponent< CWorker >();
	entity->addComponent< CHealth >(50.0f, 50.0f);
	entity->addComponent< CMove >(80.0f, (Vector3){0.0f, -360.0f, 0.0f});
	CModel& modelComponent = entity->addComponent< CModel >();
	modelComponent.model = game->assets->get< Model >("worker");
	for(int i=0; i < gui->getPage(GameGrid::GRID)->getElements().size(); i++)
	{
		std::shared_ptr< Tile > cell = dynamic_pointer_cast< Tile >(gui->getPage(GameGrid::GRID)->getElements()[i]);
		if ( cell->getShape() == gui->townShape )
		{
			std::shared_ptr< Entity > town = entities->entities.addEntity("Town");
			int row = cell->getId() / CELLSX;
			int column = cell->getId() % CELLSX;
			town->addComponent< CTransform2D >((Vector2){(float)column, (float)row});
			CModel& buildingModel = town->addComponent< CModel >();
			float scaleFactorX;
			float angle = 0;
			char townTier = cell->getInner()[4];
			if ( townTier == '0')
			{
				buildingModel.model = game->assets->get< Model >("town0");
				scaleFactorX = cell->width() * camera.position.y / 500.0f;
				angle = 180;
				town->addComponent< CTown >(1);
				town->addComponent< CSpawner >(SpawnType::ENEMY, 0, 0, 0);
			}
			else if ( townTier == '1' )
			{
				buildingModel.model = game->assets->get< Model >("town1");
				scaleFactorX = cell->width() * camera.position.y / 600.0f;
				angle = 180;
				town->addComponent< CTown >(2);
				town->addComponent< CSpawner >(SpawnType::ENEMY, 15 * 60, 2, 0);
			}
			else if ( townTier == '2' )
			{
				buildingModel.model = game->assets->get< Model >("town2");
				scaleFactorX = cell->width() * camera.position.y / 1000.0f;
				town->addComponent< CTown >(4);
				town->addComponent< CSpawner >(SpawnType::ENEMY, 15 * 60, 5, 0);
			}
			else if ( townTier == '3' )
			{
				buildingModel.model = game->assets->get< Model >("town3");
				scaleFactorX = cell->width() * camera.position.y / 1100.0f;
				town->addComponent< CTown >(6);
				town->addComponent< CSpawner >(SpawnType::ENEMY, 10 * 60, 6, 0);
			}
			Vector3 elemPosition3D = convert2DPos3D(cell->position);
			float scaleFactorY = scaleFactorX - 2.0f;
			float scaleFactorZ = scaleFactorY / 2.0f;
			town->addComponent< CTransform3D >(elemPosition3D, Vector3{scaleFactorX, scaleFactorY, scaleFactorZ}, Vector3{0.0f, 1.0f, 0.0f}, angle);
			cell->building = town;
		}
	}
}