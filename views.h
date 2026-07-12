#ifndef GAME_SCENES_H
#define GAME_SCENES_H
#include "../../libs/NoMVC/src/Controller.h"
#include "models.h"
//#include "buildings.h"
//#include "entity.h"

//const float PI = 3.14159f;
const float CAMERAANGLE = 45.0f * (PI / 180.0f);
enum class GameState {LOSS = 0, RUNNING = 1, PAUSED = 2, VICTORY = 3};

Vector3 convert2DPos3D(Vector2 position2D)
{
	
	return (Vector3){(position2D.x - 360.0f) * std::sin(CAMERAANGLE) * -1, (position2D.y * -1 + 360.0f), 0.0f};
}

class Scene : public NoMVC::View, public NoGUI::Listener
{
private:
	GameState state = GameState::RUNNING;
protected:
	void setVictory(bool win)
	{
		if ( win )
		{
			std::shared_ptr< NoGUI::Manager > gui = dynamic_pointer_cast< NoGUI::Manager >(getModel((size_t)GameModels::GRID));
		}
	}
public:
	Camera camera = { 0 };
//	std::shared_ptr< Entity > building = nullptr;
	BuildingType currentBuild = BuildingType::NONE;
	Vector3 portalSize;
	Scene(NoMVC::Controller* g, NoMVC::WindowConfig conf)
		: NoMVC::View(g, conf) 
	{
		camera.position = (Vector3){ 0.0f, 360.0f, -360.0f };
		camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
		camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
		camera.fovy = 720.0f * std::sin(CAMERAANGLE);
		camera.projection = CAMERA_ORTHOGRAPHIC;
		float scaleFactorX = 720.0f / (CELLSX * SQRT3) * 2 * camera.position.y / 1400.0f;
		float scaleFactorY = scaleFactorX - 2.0f;
		float scaleFactorZ = scaleFactorY / 2.0f;
		portalSize = (Vector3){scaleFactorX, scaleFactorY, scaleFactorZ};
	}
	std::vector< std::shared_ptr< NoGUI::Element > > getSurrondingCells(std::shared_ptr< NoGUI::Element > centerElem, unsigned int radius=1)
	{		
		std::vector< std::shared_ptr< NoGUI::Element > > surrondingElems;
		std::shared_ptr< NoGUI::Manager > gui = dynamic_pointer_cast< NoGUI::Manager >(getModel((size_t)GameModels::GRID));
		std::shared_ptr< NoGUI::Page > grid = gui->getPage((size_t)GameGrid::GRID);
		size_t center = centerElem->getId();
		int centerRow = center / (CELLSX + 1);
		bool centerShifted = (centerRow + 1) % 2 == 0;
		int minRow = (centerRow >= radius) ?  centerRow - radius : 0;
		int maxRow = (center + (CELLSX + 1) * radius < grid->getElements().size() - 1) ? centerRow + radius : (grid->getElements().size() - 1) / (CELLSX + 1);
		// for (int row=minRow; row <= maxRow; row++ )
		// {
			// bool isShiftedRow = (row + 1) % 2 == 0;	
			// int rowDiff = (row > centerRow) ? row - centerRow : centerRow - row;
			// int columnLeftShift = rowDiff / 2;
			// int columnRightShift = rowDiff / 2;
			// if ( rowDiff % 2 != 0 )
			// {
				// if ( centerShifted )
				// {
					// if ( !isShiftedRow )
					// {
						// columnLeftShift += 1;
					// }
					// else
					// {
						// columnRightShift += 1;
					// }
				// }
				// else
				// {
					// if ( isShiftedRow )
					// {
						// columnRightShift += 1;
					// }
					// else
					// {
						// columnLeftShift += 1;
					// }
				// }
			// }
			// int minColumn = (center % CELLSX) - radius + columnLeftShift;
			// int maxColumn = (center % CELLSX) + radius - columnRightShift;
			// if ( minColumn < 0 )
			// {
				// minColumn = 0;
			// }
			// if ( maxColumn > CELLSX - 1 ) 
			// {
				// maxColumn = CELLSX - 1;
			// }
			// for (int column=minColumn; column <= maxColumn; column++)
			// {
				// int targetIndex = column + (row * CELLSX);
				// if ( targetIndex != center && targetIndex < grid->getElements().size() - 1 )
				// {
					// surrondingElems.push_back(grid->getElement(targetIndex));
				// }
			// }
		// }
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
	
	void placeMonument(std::shared_ptr< NoGUI::Element > tile)
	{
		std::shared_ptr< GameGrid > gui = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
//		std::shared_ptr< GameClock > clock = dynamic_pointer_cast< GameClock >(getModel((size_t)GameModels::CLOCK));
		tile->setShape(gui->monumentShape);
		for ( std::shared_ptr< NoGUI::Element > cell : getSurrondingCells(tile, 2) )
		{
			if ( TextLength(cell->getInner()) == 0 )
			{
				cell->setShape(gui->convertingSwampShape);
//				gui->monumentInfluence.push(std::make_pair(cell, clock->getFrame()));
//				gui->monumentInfluence.push(std::make_pair(cell, tile->getId()));
			}
//			else if ( TextIsEqual("Town", cell->getInner()) )
//			{
//				cell->setShape(gui->convertingPortalShape);
//				gui->monumentInfluence.push(std::make_pair(cell, clock->getFrame()));
//			}
		}
	}
	
	void render() 
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
			float alphaPercent = (float)framesRemain / 75.0f;
			grid->convertingSwampFill->col = (Color){grid->swampFill->col.r, grid->swampFill->col.g, grid->swampFill->col.b, static_cast<unsigned char>(255 * alphaPercent)};
			grid->convertingSwampFill->hoverCol = (Color){grid->swampFill->col.r, grid->swampFill->col.g, grid->swampFill->col.b, static_cast<unsigned char>(255 * alphaPercent)};
			grid->convertingPortalFill->col = (Color){grid->portalFill->col.r, grid->portalFill->col.g, grid->portalFill->col.b, static_cast<unsigned char>(255 * alphaPercent)};
			grid->convertingPortalFill->hoverCol = (Color){grid->portalFill->col.r, grid->portalFill->col.g, grid->portalFill->col.b, static_cast<unsigned char>(255 * alphaPercent)};
		}
//		gui->getPage(GameGrid::GRID)->render();
		grid->render();
		std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
		BeginMode3D(camera);
			entities->render();
		EndMode3D();
		gui->render();		
	}
	
	void run()
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
			for ( std::shared_ptr< Entity > town : entities->entities.getEntities("Town") )
			{
				if ( town->getComponent< CSpawner >().spawnRate > 0 && clock->getFrame() % town->getComponent< CSpawner >().spawnRate == 0 )
				{
					if ( resources->workers < resources->maxWorkers )
					{
						std::shared_ptr< Entity > entity = entities->entities.addEntity("Worker", "Goblin");
						entity->addComponent< CTransform3D >(town->getComponent< CTransform3D >().pos, Vector3{25.0f, 25.0f, 25.0f});
						entity->addComponent< CWorker >();
						entity->addComponent< CHealth >(50.0f, 50.0f);
						entity->addComponent< CMove >();
						CModel& modelComponent = entity->addComponent< CModel >();
						modelComponent.model = LoadModel("./assets/character-orc.glb");		
						resources->workers += 1;
					}
				}
			}
			// TODO: use an event for this
			for (std::shared_ptr< Entity > building : entities->entities.getEntities("Building"))
			{
				CBuilding& buildingComp = building->getComponent< CBuilding >();
				switch ( buildingComp.type )
				{
					case BuildingType::MONUMENT:
					{
						if ( buildingComp.state == BuildingState::BUILT )
						{
//							std::shared_ptr< NoGUI::Page > gridPage = gui->getPage(GameGrid::GRID);
							Vector2 monumentPos = building->getComponent< CTransform2D >().pos;
							std::shared_ptr< NoGUI::Element > monumentTile = gridPage->getElement(monumentPos.x * CELLSX + monumentPos.y);
							for (std::shared_ptr< NoGUI::Element > cell : getSurrondingCells(monumentTile, 2))
							{
								if ( TextLength(cell->getInner()) == 0 )
								{ 
									cell->setInner("Swamp");
									cell->setShape(grid->swampShape);
								}
							}
							buildingComp.state = BuildingState::ACTIVE;
						}
					
						break;
					}
				
					default:
					{
					
						break;
					}
				}
			}
			for ( std::shared_ptr< Entity > town : entities->entities.getEntities("Town") )
			{
				if ( town->hasComponent< CTown >() )
				{
					std::vector< std::shared_ptr< NoGUI::Element > > monumentCells;
					Vector2 townPos = town->getComponent< CTransform2D >().pos;
					std::shared_ptr< NoGUI::Element > townTile = gridPage->getElement(townPos.x * CELLSX + townPos.y);
//					for (std::shared_ptr< NoGUI::Element > cell : getSurrondingCells(townTile, 2))
					std::vector< std::shared_ptr< NoGUI::Element > > surrondingCells = getSurrondingCells(townTile, 2);
					for (int i=0; i < surrondingCells.size(); i++)
					{
						std::shared_ptr< Tile > cell = dynamic_pointer_cast< Tile >(surrondingCells.at(i));
						if ( cell->building )
						{
							CBuilding& cellBuilding = cell->building->getComponent< CBuilding >();
							if ( cellBuilding.type == BuildingType::MONUMENT && cellBuilding.state != BuildingState::CONSTRUCTION )
							{
								monumentCells.push_back(cell);
							}
						}
					}
					if ( monumentCells.size() >= town->getComponent< CTown >().influence )
					{
						// convert Town cell into portal
						CTransform3D& townTransform = town->getComponent< CTransform3D >();
						UnloadModel(town->getComponent< CModel >().model);
						town->getComponent< CModel >().model = LoadModel("./assets/magic_portal.glb");
						town->getComponent< CTransform3D >().scale = portalSize;
						town->getComponent< CSpawner >().spawnRate = 5 * 60;
						town->getComponent< CTown >().owned = false;
						townTile->setShape(grid->portalShape);
						townTile->setInner("Portal");
						resources->maxWorkers += 5;
						for (int i=0; i < monumentCells.size(); i++)
						{
							std::shared_ptr< Tile > monumentCell = std::dynamic_pointer_cast< Tile >(monumentCells.at(i));
							monumentCell->setShape(grid->swampShape);
							// destroy monument entities
							UnloadModel(monumentCell->building->getComponent< CModel >().model);
							monumentCell->building->destroy();
							monumentCell->building = nullptr;
						}
					}
				}
			}
//		if ( clock->getFrame() % 60 == 0 )
//		{
			resources->mana += resources->manaGen;
			if ( resources->mana > resources->maxMana )
			{
				resources->mana = resources->maxMana;
			}
			if ( IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) )
			{
				currentBuild = BuildingType::NONE;
			}
		}
//		}
		std::shared_ptr< Overlay > gui = dynamic_pointer_cast< Overlay >(getModel((size_t)GameModels::GUI));
		gui->getPage(Overlay::RESOURCES)->getElements().front()->setInner(TextFormat("%.0f", resources->mana));
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
//		std::cout << "scene updated frame " << clock->getFrame() << std::endl;
//		render();
	}
	
	void onNotify(std::shared_ptr< NoGUI::Element > elem, NoGUI::HoverEvent hevent, NoGUI::FocusEvent fevent)
	{
		switch (fevent)
		{
			case NoGUI::FocusEvent::ONFOCUS:
			{
//				if ( TextIsEqual("Swamp", elem->getInner()) && building)
				if ( TextIsEqual("Swamp", elem->getInner()) )
				{
					std::shared_ptr< GameResources > resources = dynamic_pointer_cast< GameResources >(getModel((size_t)GameModels::RESOURCES));
//					CCost& buildingCost = building->getComponent< CCost >();
					float buildingCost = BUILDINGCOSTS[static_cast< int >(currentBuild)];
					if ( resources->mana >= buildingCost )
					{
						resources->mana -= buildingCost;
//						switch ( building->getComponent< CBuilding >().type )
						std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
						switch (currentBuild)
						{
							case BuildingType::NONE:
							{
							
								break;
							}
						
							case BuildingType::MONUMENT:
							{	
								std::cout << "building monument" << std::endl;
								std::shared_ptr< Entity > building = entities->entities.addEntity("Building", "Monument");
								building->addComponent< CBuilding >(BuildingType::MONUMENT);
								building->addComponent< CCost >(buildingCost);
								building->addComponent< CHealth >();
								Vector3 elemPosition3D = convert2DPos3D(elem->position);
								std::cout << "cells 2D size: " << elem->width() << ", " << elem->height() << std::endl;
								float scaleFactorX = elem->width() * camera.position.y / 1000.0f;
								float scaleFactorY = scaleFactorX - 3.0f;
								float scaleFactorZ = scaleFactorY - 4.0f;
								building->addComponent< CTransform3D >(elemPosition3D, Vector3{scaleFactorX, scaleFactorY, scaleFactorZ});
								CModel& buildingModel = building->addComponent< CModel >();
								buildingModel.model = LoadModel("./assets/wood-structure.glb");
								int row = elem->getId() / CELLSX;
								int column = elem->getId() % CELLSX;
								building->addComponent< CTransform2D >((Vector2){(float)row, (float)column});
								
								placeMonument(elem);
								std::shared_ptr< Tile > tile = dynamic_pointer_cast< Tile >(elem);
								tile->building = building;
//								building = nullptr;
							
								break;
							}
						}
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
//						building = entities->entities.addEntity("Building", "Monument");
						// TODO: magic numbers
//						building->addComponent< CBuilding >(BuildingType::MONUMENT);
//						building->addComponent< CCost >(20);
//						building->addComponent< CHealth >();
						currentBuild = BuildingType::MONUMENT;
						gui->getPage(Overlay::TABS)->setActive(true);
					}
				}
				
				break;
			}
		}
	}
	
	void initialize()
	{
		std::shared_ptr< GameGrid > gui = dynamic_pointer_cast< GameGrid >(getModel((size_t)GameModels::GRID));
		gui->update();
		std::shared_ptr< EntitySystem > entities = dynamic_pointer_cast< EntitySystem >(getModel((size_t)GameModels::ENTITIES));
//		for (std::shared_ptr< NoGUI::Element > cell : gui->getPage(GameGrid::GRID)->getElements())
		for(int i=0; i < gui->getPage(GameGrid::GRID)->getElements().size(); i++)
		{
			std::shared_ptr< Tile > cell = dynamic_pointer_cast< Tile >(gui->getPage(GameGrid::GRID)->getElements()[i]);
			if ( cell->getShape() == gui->townShape )
			{
				std::shared_ptr< Entity > town = entities->entities.addEntity("Town");
				int row = cell->getId() / CELLSX;
				int column = cell->getId() % CELLSX;
				town->addComponent< CTransform2D >((Vector2){(float)row, (float)column});
				CModel& buildingModel = town->addComponent< CModel >();
				float scaleFactorX;
				float angle = 0;
				char townTier = cell->getInner()[4];
				if ( townTier == '0')
				{
					buildingModel.model = LoadModel("./assets/building_home_A_yellow.gltf");
					scaleFactorX = cell->width() * camera.position.y / 500.0f;
					angle = 180;
					town->addComponent< CTown >(1);
				}
				else if ( townTier == '1' )
				{
					buildingModel.model = LoadModel("./assets/building_home_B_yellow.gltf");
					scaleFactorX = cell->width() * camera.position.y / 600.0f;
					angle = 180;
					town->addComponent< CTown >(2);
				}
				else if ( townTier == '2' )
				{
					buildingModel.model = LoadModel("./assets/building_barracks_yellow.gltf");
					scaleFactorX = cell->width() * camera.position.y / 1000.0f;
					town->addComponent< CTown >(4);
				}
				else if ( townTier == '3' )
				{
					buildingModel.model = LoadModel("./assets/building_castle_yellow.gltf");
					scaleFactorX = cell->width() * camera.position.y / 1400.0f;
					town->addComponent< CTown >(6);
				}
				Vector3 elemPosition3D = convert2DPos3D(cell->position);
				float scaleFactorY = scaleFactorX - 2.0f;
				float scaleFactorZ = scaleFactorY / 2.0f;
				town->addComponent< CTransform3D >(elemPosition3D, Vector3{scaleFactorX, scaleFactorY, scaleFactorZ}, Vector3{0.0f, 1.0f, 0.0f}, angle);
				cell->building = town;
			}
		}
	}
};

#endif