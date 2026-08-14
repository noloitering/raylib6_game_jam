#ifndef GAME_MODELS_H
#define GAME_MODELS_H

#include "../../libs/NoGUI/src/GUI.h"
#include "../../libs/NoMVC/src/Model.h"
#include "entity.h"

const float SQRT3 = sqrt(3.0f);
const unsigned int CELLSX = 15;
const unsigned int CELLSY = CELLSX * 1.155 + 1; // approximately for the range we'll be using
enum class GameModels {CLOCK = 0, RESOURCES = 1, GRID = 2, GUI = 3, ENTITIES = 4};

class Tile : public NoGUI::Button
{
public:
	Tile(const size_t& num, std::shared_ptr< NoGUI::nShape > style, const Vector2& pos={0.0f, 0.0f}, const Vector2& size={0.0f, 0.0f}, float rotation=0.0f, const NoGUI::Align& origin=NoGUI::Align(), const char* type="Default", const char* in="", std::shared_ptr< NoGUI::CContainer > c=nullptr)
		: NoGUI::Button(num, style, pos, size, rotation, origin, type, in, c) {}
	Tile(const size_t& num, std::shared_ptr< NoGUI::nShape > style, const Transform& dimensions, const char* type="Default", const char* in="", std::shared_ptr< NoGUI::CContainer > c=nullptr)
		: NoGUI::Button(num, style, dimensions, type, in, c) {}
	
	std::shared_ptr< Entity > building = nullptr;
};

class GameGrid : public NoGUI::Manager, public NoMVC::Model
{
public:
	enum pageEnums {GRID=0};
	// fills
	std::shared_ptr< NoGUI::Fill > cellFill = std::make_shared< NoGUI::Fill >((Color){254, 254, 254, 1}, (Color){100, 100, 100, 100});
	std::shared_ptr< NoGUI::Fill > swampFill = std::make_shared< NoGUI::Fill >(PURPLE, PINK);
	std::shared_ptr< NoGUI::Fill > convertingSwampFill = std::make_shared< NoGUI::Fill >(PINK);
	std::shared_ptr< NoGUI::Fill > mountainFill = std::make_shared< NoGUI::Fill >(BROWN);
	std::shared_ptr< NoGUI::Fill > townFill = std::make_shared< NoGUI::Fill >(GOLD, ORANGE);
	std::shared_ptr< NoGUI::Fill > portalFill = std::make_shared< NoGUI::Fill >(GREEN, LIME);
	std::shared_ptr< NoGUI::Fill > convertingPortalFill = std::make_shared< NoGUI::Fill >(GOLD);
	std::shared_ptr< NoGUI::Fill > monumentFill = std::make_shared< NoGUI::Fill >(DARKPURPLE);
	// outlines
	std::shared_ptr< NoGUI::Fill > nonWorkableOutlineFill = std::make_shared< NoGUI::Fill >(BLACK, RED);
	std::shared_ptr< NoGUI::Fill > workableOutlineFill = std::make_shared< NoGUI::Fill >(BLACK, DARKBLUE);
	std::shared_ptr< NoGUI::Outline > nonWorkableOutline = std::make_shared< NoGUI::Outline >(nonWorkableOutlineFill, 2);
	std::shared_ptr< NoGUI::Outline > workableOutline = std::make_shared< NoGUI::Outline >(workableOutlineFill, 2);
	// shapes
	std::shared_ptr< NoGUI::nShape > cellShape =  std::make_shared< NoGUI::nShape >(6, cellFill, nonWorkableOutline);
	std::shared_ptr< NoGUI::nShape > swampShape =  std::make_shared< NoGUI::nShape >(6, swampFill, workableOutline);
	std::shared_ptr< NoGUI::nShape > mountainShape = std::make_shared< NoGUI::nShape >(6, mountainFill, nonWorkableOutline);
	std::shared_ptr< NoGUI::nShape > townShape =  std::make_shared< NoGUI::nShape >(6, townFill, nonWorkableOutline);
	std::shared_ptr< NoGUI::nShape > monumentShape = std::make_shared< NoGUI::nShape >(6, monumentFill, nonWorkableOutline);
	std::shared_ptr< NoGUI::nShape > convertingSwampShape = std::make_shared< NoGUI::nShape >(6, convertingSwampFill, nonWorkableOutline);
	std::shared_ptr< NoGUI::nShape > portalShape = std::make_shared< NoGUI::nShape >(6, portalFill, nonWorkableOutline);
	std::shared_ptr< NoGUI::nShape > convertingPortalShape = std::make_shared< NoGUI::nShape >(6, convertingPortalFill, nonWorkableOutline);
	
	GameGrid()
		: NoGUI::Manager(false) {}
	void update()
	{
		NoGUI::Manager::update();
	}
	void render()
	{
		NoGUI::Manager::render();
	}
	void addGridPage()
	{
		std::shared_ptr< NoGUI::Page > gridPage = addPage(true);
		Vector2 cellRadius = (Vector2){720.0f / (CELLSX * SQRT3), 720.0f / (CELLSX * SQRT3)};
//		for (unsigned int y=0; y < CELLSY * 3/2; y++)
		for (unsigned int y=0; y < CELLSY; y++)
		{
			for (unsigned int x=0; x < CELLSX + 1; x++)
			{
				Vector2 startPos;
				startPos.x = (y % 2 == 0) ? cellRadius.x * SQRT3 / 2 : cellRadius.x * SQRT3;
				startPos.x -= cellRadius.x * SQRT3 / 2;
				startPos.y = 720.0f - cellRadius.y * 3/8;
				NoGUI::Transform cellTransform = NoGUI::Transform((Vector2){startPos.x + cellRadius.x * x * SQRT3, startPos.y - cellRadius.y * y * 3/2}, cellRadius, NoGUI::Align());
				std::shared_ptr< NoGUI::Element > cell = gridPage->addElement< Tile >(cellShape, cellTransform, "Cell", "");
				if (x == 0 || x > CELLSX - 2)
				{
					cell->setInner("Mountain");
					cell->setShape(mountainShape);
				}
				else if ( y < 3 )
				{
					bool isShifted = (y + 1) % 2 == 0;
					if ( isShifted && x >  CELLSX - 1 - 4)
					{
						cell->setInner("Swamp");
						cell->setShape(swampShape);
					}
					else if ( !isShifted && x > CELLSX - 1 - 3)
					{
						cell->setInner("Swamp");
						cell->setShape(swampShape);
					}
					if ( y == 1 )
					{
						if ( x ==  CELLSX - 2 )
						{
							cell->setShape(monumentShape);
						}
					}
					// towns
					else if ( y == 2 )
					{
						if ( x ==  CELLSX - 1 - 4 )
						{
							cell->setInner("Town0");
							cell->setShape(townShape);
						}
					}
				}
				else if ( y == 3 )
				{
					if ( x >  CELLSX - 1 - 3 )
					{
						cell->setInner("Swamp");
						cell->setShape(swampShape);
					}
				}
				else if ( y == (CELLSY - 1) / 2 )
				{
					if ( x == 3 )
					{
						cell->setInner("Town1");
						cell->setShape(townShape);
					}
				}
				else if ( y == CELLSY * 3/4 )
				{
					if ( x ==  CELLSX - 1 - 2 )
					{
						cell->setInner("Town2");
						cell->setShape(townShape);
					}
				}
				if ( y == CELLSY - 5 )
				{
					if ( x == CELLSX - CELLSX / 2  )
					{
						cell->setInner("Town3");
						cell->setShape(townShape);
					}
				}
			}
		}
	}
	
	void initialize()
	{
		addGridPage();
	}
};

class Overlay : public NoGUI::Manager, public NoMVC::Model
{
public:
	enum pageNums {RESOURCES=0, TABS=1, ACTION=2, BUILDINGS=3, SPELLS=4, UNITS=5, TIPS=6, PAUSE=7, VICTORY=8};
	// fills
	std::shared_ptr< NoGUI::Fill > invis = std::make_shared< NoGUI::Fill >(BLANK);
	std::shared_ptr< NoGUI::Fill > tabFill = std::make_shared< NoGUI::Fill >(LIGHTGRAY, GRAY);
	std::shared_ptr< NoGUI::Fill > containerFill = std::make_shared< NoGUI::Fill >(GRAY);
	std::shared_ptr< NoGUI::Fill > manaBarFill = std::make_shared< NoGUI::Fill >(VIOLET);
	std::shared_ptr< NoGUI::Fill > noManaFill = std::make_shared< NoGUI::Fill >(MAROON);
	std::shared_ptr< NoGUI::Fill > blackTextFill = std::make_shared< NoGUI::Fill >(BLACK);
	std::shared_ptr< NoGUI::Fill > toolTipFill = std::make_shared< NoGUI::Fill >((Color){0, 0, 0, 200});
	// outlines
	std::shared_ptr< NoGUI::Fill > noManaOutlineFill = std::make_shared< NoGUI::Fill >(RED);
	std::shared_ptr< NoGUI::Fill > tabOutlineFill = std::make_shared< NoGUI::Fill >(DARKGRAY);
	std::shared_ptr< NoGUI::Outline > tabOutline = std::make_shared< NoGUI::Outline >(tabOutlineFill, 2);
	std::shared_ptr< NoGUI::Outline > noManaOutline = std::make_shared< NoGUI::Outline >(noManaOutlineFill, 2);
	// shapes
	std::shared_ptr< NoGUI::nShape > containerShape = std::make_shared< NoGUI::nShape >(4, containerFill, tabOutline);
	std::shared_ptr< NoGUI::nShape > tabShape = std::make_shared< NoGUI::nShape >(4, tabFill, tabOutline);
	std::shared_ptr< NoGUI::nShape > invisShape = std::make_shared< NoGUI::nShape >(4, invis);
	std::shared_ptr< NoGUI::nShape > unitShape = std::make_shared< NoGUI::nShape >(4, invis, tabOutline);
	std::shared_ptr< NoGUI::nShape > toolTipShape = std::make_shared< NoGUI::nShape >(4, toolTipFill, tabOutline);
	std::shared_ptr< NoGUI::nShape > manaBarShape = std::make_shared< NoGUI::nShape >(4, manaBarFill);
	std::shared_ptr< NoGUI::nShape > noManaShape = std::make_shared< NoGUI::nShape >(4, noManaFill, noManaOutline);
	// transforms
	NoGUI::Transform actionContainerTransform;
	// toolTips
	// TODO: make this case-insensitive
	size_t static constexpr toolTipHasher(const char* key)
	{
		size_t hash = 0;
		const size_t primeNumber = 31;
		while (*key != '\0')
		{
			hash = hash * primeNumber + static_cast< size_t >(*key);
			key++;
		}
		
		return hash;
	}
	std::unordered_map< const char*, const char*,  size_t(*)(const char*), bool(*)(const char*, const char*) > tips
	{
		{
			{"Lycanthropy", "A Fast Moving Melee Striker Unit"}, 
			{"Undeath", "A Slow Tanky Hard Hitting Melee Bruiser"}
		},
		2,
		toolTipHasher, 
		TextIsEqual, 
	};
	Overlay()
		: NoGUI::Manager(false) {}

	void update()
	{
		NoGUI::Manager::update();
	}
	void render()
	{
		NoGUI::Manager::render();
	}
	void addResourcePage()
	{
		std::shared_ptr< NoGUI::Page > resourcePage = addPage(true);
		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(DARKGRAY);
		std::shared_ptr< NoGUI::CContainer > labelComponents = resourcePage->addComponents("Label");
		labelComponents->addComponent< NoGUI::CText >(textFill, nullptr, 20.0f);
		std::shared_ptr< NoGUI::nShape > labelShape = std::make_shared< NoGUI::nShape >(4, invis);
		Vector2 manaLabelRadius = (Vector2){50, 10};
		Vector2 manaBarRadius = (Vector2){50, 20};
		Vector2 manaLabelPos = (Vector2){720 - manaLabelRadius.x, 700 - manaLabelRadius.y * 7};
		Vector2 manaBarPos = (Vector2){720 - manaLabelRadius.x, 700 - manaBarRadius.y * 2};
		NoGUI::Transform manaLabelTransform = NoGUI::Transform(manaLabelPos, manaLabelRadius, NoGUI::Align());
		NoGUI::Transform manaBarTransform = NoGUI::Transform(manaBarPos, manaBarRadius, NoGUI::Align());
		resourcePage->addElement< NoGUI::Element >(labelShape, manaLabelTransform, "Label", "0");
		std::shared_ptr< NoGUI::Slider > manaBar = resourcePage->addElement< NoGUI::Slider >(containerShape, manaBarTransform, "Mana");
		std::shared_ptr< NoGUI::Slider > noManaBar = resourcePage->addElement< NoGUI::Slider >(noManaShape, manaBarTransform, "Mana"); 
		manaBar->setSlide(manaBarShape, NoGUI::Align(-1, 0));
		manaBar->setActive(false);
		noManaBar->setSlide(manaBarShape, NoGUI::Align(-1, 0));
		noManaBar->setActive(false);
		noManaBar->getShape()->fill->col.a = 0;
		noManaBar->getShape()->outline->fill->col.a = 0;
	}
	void addActionTabsPage()
	{
		std::shared_ptr< NoGUI::Page > tabPage = addPage(true);
		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(DARKGRAY);
		std::shared_ptr< NoGUI::CContainer > tabComponents = tabPage->addComponents("Tab");
		tabComponents->addComponent< NoGUI::CText >(textFill, nullptr, 20.0f);
		Vector2 tabRadius = (Vector2){35.0f, 20.0f};
		Vector2 buildPos = (Vector2){720 - tabRadius.x * 3, tabRadius.y};
		Vector2 spellPos = (Vector2){720 - tabRadius.x, tabRadius.y};
		NoGUI::Transform buildTransform = NoGUI::Transform(buildPos, tabRadius);
		NoGUI::Transform spellTransform = NoGUI::Transform(spellPos, tabRadius);
		tabPage->addElement< NoGUI::Button >(tabShape, buildTransform, "Tab", "Build");
		tabPage->addElement< NoGUI::Button >(tabShape, spellTransform, "Tab", "Spells");
	}
	void addActionSelectionPage()
	{
		std::shared_ptr< NoGUI::Page > actionPage = addPage(false);
//		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(BLACK);
		std::shared_ptr< NoGUI::CContainer > closeComponents = actionPage->addComponents("Close");
		closeComponents->addComponent< NoGUI::CText >(blackTextFill, nullptr, 20.0f);
		Vector2 containerRadius = (Vector2){100.0f, 360.0f};
		Vector2 containerPos = (Vector2){720.0f - containerRadius.x, 360.0f};
		Vector2 closeRadius = (Vector2){65.0f, 25.0f};
		Vector2 closePos = (Vector2){containerPos.x - containerRadius.x + closeRadius.x + 10.0f, 720.0f - closeRadius.y - 10.0f};
		actionContainerTransform = NoGUI::Transform(containerPos, containerRadius);
		NoGUI::Transform closeTransform = NoGUI::Transform(closePos, closeRadius);
		actionPage->addElement< NoGUI::Element >(containerShape, actionContainerTransform, "Container");
		actionPage->addElement< NoGUI::Button >(tabShape, closeTransform, "Close", "Close");
	}
	void addBuildPage()
	{
		std::shared_ptr< NoGUI::Page > buildPage = addPage(false);
//		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(BLACK);
		std::shared_ptr< NoGUI::CContainer > buildingComponents = buildPage->addComponents("Building");
		buildingComponents->addComponent< NoGUI::CText >(blackTextFill, nullptr, 20.0f);
		Vector2 buildingRadius = (Vector2){80.0f, 60.0f};
		NoGUI::Transform buildingTransform = NoGUI::Transform((Vector2){actionContainerTransform.position.x, buildingRadius.y + 20.0f}, buildingRadius);
		buildPage->addElement< NoGUI::Button >(tabShape, buildingTransform, "Building", "Monument");
	}
	void addSpellPage()
	{
		std::shared_ptr< NoGUI::Page > spellsPage = addPage(false);
//		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(BLACK);
		std::shared_ptr< NoGUI::CContainer > spellComponents = spellsPage->addComponents("Spell");
		spellComponents->addComponent< NoGUI::CText >(blackTextFill, nullptr, 20.0f);
		Vector2 spellRadius = (Vector2){80.0f, 60.0f};
		NoGUI::Transform spell0Transform = NoGUI::Transform((Vector2){actionContainerTransform.position.x, spellRadius.y + 20.0f}, spellRadius);
		NoGUI::Transform spell1Transform = NoGUI::Transform((Vector2){actionContainerTransform.position.x, spellRadius.y * 3 + 20.0f * 2}, spellRadius);
		NoGUI::Transform spell2Transform = NoGUI::Transform((Vector2){actionContainerTransform.position.x, spellRadius.y * 5 + 20.0f * 3}, spellRadius);
		spellsPage->addElement< NoGUI::Button >(tabShape, spell0Transform, "Spell", "Command");
		spellsPage->addElement< NoGUI::Button >(tabShape, spell1Transform, "Spell", "Heal");
		spellsPage->addElement< NoGUI::Button >(tabShape, spell2Transform, "Spell", "Summon Goblin");
	}
	void addUnitPage()
	{
		std::shared_ptr< NoGUI::Page > unitPage = addPage(false);
//		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(BLACK);
		std::shared_ptr< NoGUI::CContainer > unitComponents = unitPage->addComponents("Unit");
		unitComponents->addComponent< NoGUI::CText >(blackTextFill, nullptr, 20.0f);
		std::shared_ptr< NoGUI::CContainer > labelComponents = unitPage->addComponents("Label");
		labelComponents->addComponent< NoGUI::CText >(blackTextFill, nullptr, 20.0f);
		Vector2 containerRadius = (Vector2){180, 250};
		Vector2 labelRadius = (Vector2){150, 30};
		Vector2 unitRadius = (Vector2){100, 35};
		NoGUI::Transform containerTransform = NoGUI::Transform((Vector2){360, 360}, containerRadius);
		NoGUI::Transform labelTransform = NoGUI::Transform((Vector2){360, 360 - containerRadius.y + labelRadius.y}, labelRadius);
		NoGUI::Transform lycanthropyTransform = NoGUI::Transform((Vector2){360, labelTransform.position.y + labelRadius.y + unitRadius.y}, unitRadius);
		NoGUI::Transform undeathTransform = NoGUI::Transform((Vector2){360, lycanthropyTransform.position.y + labelRadius.y + unitRadius.y * 2}, unitRadius);
		unitPage->addElement< NoGUI::Element >(containerShape, containerTransform, "Container");
		unitPage->addElement< NoGUI::Element >(invisShape, labelTransform, "Label", "Choose Your Hex:");
		std::shared_ptr< NoGUI::Slider > lycanthropySlider = unitPage->addElement< NoGUI::Slider >(unitShape, lycanthropyTransform, "Unit", "Lycanthropy");
		std::shared_ptr< NoGUI::Slider > undeathSlider = unitPage->addElement< NoGUI::Slider >(unitShape, undeathTransform, "Unit", "Undeath");
		lycanthropySlider->setSlide(manaBarShape, NoGUI::Align(-1, 0));
		undeathSlider->setSlide(manaBarShape, NoGUI::Align(-1, 0));
//		tips[lycanthropySlider->getId()] = "A Fast Moving Melee Striker Unit";
//		tips[undeathSlider->getId()] = "A Slow Tanky Hard Hitting Melee Bruiser";
	}
	void addToolTips()
	{
		std::shared_ptr< NoGUI::Page > toolTips = addPage(false);
		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(RAYWHITE);
		std::shared_ptr< NoGUI::CContainer > tipComponents = toolTips->addComponents("Tip");
		Vector2 toolTipRadius = (Vector2){100, 9.0f};
		tipComponents->addComponent< NoGUI::CTextBox >(textFill, nullptr, 18.0f, NoGUI::Align(-1, -1), true);
		NoGUI::Transform tipTransform = NoGUI::Transform((Vector2){0.0f, 0.0f}, toolTipRadius, NoGUI::Align(-1, -1));
		toolTips->addElement< NoGUI::Element >(toolTipShape, tipTransform, "Tip", "");
	}
	void addPausePage()
	{
		std::shared_ptr< NoGUI::Page > pausePage = addPage(false);
		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(GRAY);
		std::shared_ptr< NoGUI::CContainer > labelComponents = pausePage->addComponents("Label");
		labelComponents->addComponent< NoGUI::CText >(textFill, nullptr, 50.0f);
		Vector2 messageRadius = (Vector2){360.0f, 50.0f};
		Vector2 messagePos = (Vector2){360.f, 360.0f};
		NoGUI::Transform messageTransform = NoGUI::Transform(messagePos, messageRadius);
		pausePage->addElement< NoGUI::Element >(invisShape, messageTransform, "Label", "Paused");
	}
	void addVictoryPage()
	{
		std::shared_ptr< NoGUI::Page > victoryPage = addPage(false);
//		std::shared_ptr< NoGUI::Fill > textFill = std::make_shared< NoGUI::Fill >(BLACK);
		std::shared_ptr< NoGUI::Fill > messageFill = std::make_shared< NoGUI::Fill >((Color){255, 0, 110, 255});
		std::shared_ptr< NoGUI::CContainer > labelComponents = victoryPage->addComponents("Label");
		labelComponents->addComponent< NoGUI::CText >(messageFill, nullptr, 40.0f);
		std::shared_ptr< NoGUI::CContainer > restartComponents = victoryPage->addComponents("Restart");
		restartComponents->addComponent< NoGUI::CText >(blackTextFill, nullptr, 20.0f);
		Vector2 messageRadius = (Vector2){360.0f, 50.0f};
		Vector2 buttonRadius = (Vector2){150.0f, 30.0f};
		NoGUI::Transform messageTransform = NoGUI::Transform((Vector2){360.0f, 300.0f}, messageRadius);
		NoGUI::Transform buttonTransform = NoGUI::Transform((Vector2){360.0f, 360.0f}, buttonRadius);
		victoryPage->addElement< NoGUI::Element >(invisShape, messageTransform, "Label", "YOU WIN!");
		victoryPage->addElement< NoGUI::Button >(tabShape, buttonTransform, "Restart", "Restart");
	}
	void closeActions()
	{
		getPage(Overlay::ACTION)->setEnabled(false);
		getPage(Overlay::BUILDINGS)->setEnabled(false);
		getPage(Overlay::SPELLS)->setEnabled(false);
		getPage(Overlay::TABS)->setEnabled(true);
	}
	void resizeTextBox(std::shared_ptr< NoGUI::Element > textBox, float radiusX, float fontSize)
	{
		float fontHalfSize = fontSize / 2;
		textBox->radius = (Vector2){radiusX, fontHalfSize};
		std::vector< std::tuple< const char*, float, unsigned int > > wrappedContents = NoGUI::WrapText(textBox->getInner(), GetFontDefault(), fontSize, textBox->components->getComponent< NoGUI::CTextBox >().spacing.x, *(textBox));
		if ( wrappedContents.size() > 1 )
		{
//			float increment = gui->msgSize / 2;
			for (size_t i=1; i < wrappedContents.size(); i++)
			{
				textBox->radius.y += fontHalfSize;
				//gui->msgTransform.translate(0, gui->msgSize);
//				gui->msgTransform.translate(0, gui->msgHalfSize);
			}
		}
	}
	void initialize()
	{
		addResourcePage();
		addActionTabsPage();
		addActionSelectionPage();
		addBuildPage();
		addSpellPage();
		addUnitPage();
		addToolTips();
		addPausePage();
		addVictoryPage();
	}
};

class EntitySystem : public Notifier, public NoMVC::Model
{
public:
	EntitySystem() {}
	virtual ~EntitySystem() {}
	EntityManager entities;
	void updateStateEnemy(const std::vector< std::shared_ptr< Entity > >& enemies, const std::vector< std::shared_ptr< Entity > >& buildings, const std::vector< std::shared_ptr< Entity > >& workers, const std::vector< std::shared_ptr< Entity > >& units)
	{
		for ( std::shared_ptr< Entity > enemy : enemies )
		{
			CTransform3D& enemyTransform = enemy->getComponent< CTransform3D >();
			CMove& enemyMove = enemy->getComponent< CMove >();
			CWorker& enemyAI = enemy->getComponent< CWorker >();
			CAttack& enemyAttack = enemy->getComponent< CAttack >();
			auto [closestEntity, closestEntityDistance] = getClosestEntity3D(enemy->getComponent< CTransform3D >().pos, buildings);
			auto [closestWorker, closestWorkerDistance] = getClosestEntity3D(enemy->getComponent< CTransform3D >().pos, workers);
			auto [closestUnit, closestUnitDistance] = getClosestEntity3D(enemy->getComponent< CTransform3D >().pos, units);
			if ( closestWorkerDistance <= enemyAI.awareness || closestUnitDistance <= enemyAI.awareness )
			{
				if ( closestUnitDistance <= closestWorkerDistance )
				{
					closestEntityDistance = closestUnitDistance;
					closestEntity = closestUnit;
				}
				else
				{
					closestEntityDistance = closestWorkerDistance;
					closestEntity = closestWorker;
				}
			}
			if ( closestEntityDistance <= enemyAttack.range )
			{
				enemyAI.state = WorkerState::HEAL;
				enemyMove.target = enemyTransform.pos;
				enemyAI.target = closestEntity;
				enemyAttack.timer += GetFrameTime();
			}
			else if ( closestEntityDistance <= enemyAI.awareness )
			{
				enemyAI.state = WorkerState::HEAL;
				enemyMove.target = closestEntity->getComponent< CTransform3D >().pos;
				enemyAI.target = closestEntity;
				enemyAttack.timer = 0.0f; // TODO? gradually decrease timer rather than set to 0 
			}
			else
			{
				enemyAI.state = WorkerState::ROAM;
				enemyAI.target = nullptr;
			}
		}
	}
	
	void updateStateWorker(const std::vector< std::shared_ptr< Entity > >& workers, const std::vector< std::shared_ptr< Entity > >& buildings)
	{
		std::vector< std::shared_ptr< Entity > > damagedBuildings;
		for ( std::shared_ptr< Entity > building : buildings )
		{
			CHealth& buildingHP = building->getComponent< CHealth >();
			if ( buildingHP.hp < buildingHP.max )
			{
				damagedBuildings.push_back(building);
			}
		}
		for (std::shared_ptr< Entity > worker : workers)
		{
			CTransform3D& workerTransform = worker->getComponent< CTransform3D >();
			CMove& workerMove = worker->getComponent< CMove >();
			CHealth& workerHealth = worker->getComponent< CHealth >();
			CWorker& workerAI = worker->getComponent< CWorker >();
			CAttack& workerAttack = worker->getComponent< CAttack >();
			bool isFullHealth = workerHealth.hp >= workerHealth.max;
			float maxProximity = worker->getComponent< CAttack >().range;
			workerAI.state = WorkerState::ROAM;
			workerAI.target = nullptr;
			if ( isFullHealth && damagedBuildings.size() )
			{
				auto [closestMonument, closestMonumentDistance] = getClosestEntity3D(workerTransform.pos, damagedBuildings);
				if ( closestMonumentDistance < maxProximity )
				{
					workerAI.state = WorkerState::HEAL;
					workerMove.target = workerTransform.pos;
					workerAI.target = closestMonument;
					workerAttack.timer += GetFrameTime();
				}
				else
				{
					workerMove.target = closestMonument->getComponent< CTransform3D >().pos;
					workerAI.target = closestMonument;
					workerAttack.timer = 0.0f; // TODO? gradually decrease timer rather than set to 0
				}
			}
			else
			{
				Vector3 direction = (Vector3) {workerMove.home.x - workerTransform.pos.x, workerMove.home.y - workerTransform.pos.y, 0.0f};
				float distance = direction.x * direction.x + direction.y * direction.y;
				if ( distance > maxProximity * maxProximity )
				{
					workerAI.state = WorkerState::WALK;
					workerMove.target = workerMove.home;
				}
			}
		}
	}
	
	void updateStateUnit(const std::vector< std::shared_ptr< Entity > >& units, const std::vector< std::shared_ptr< Entity > >& enemies)
	{
		for (std::shared_ptr< Entity > unit : units)
		{
			CTransform3D& unitTransform = unit->getComponent< CTransform3D >();
			CMove& unitMove = unit->getComponent< CMove >();
			CWorker& unitAI = unit->getComponent< CWorker >();
			CAttack& unitAttack = unit->getComponent< CAttack >();
			auto [closestEnemy, closestEnemyDistance] = getClosestEntity3D(unit->getComponent< CTransform3D >().pos, enemies);
			if ( closestEnemyDistance <= unitAI.awareness && unitAI.state != WorkerState::WALK )
			{
				unitAI.target = closestEnemy;
				if ( closestEnemyDistance <= unitAttack.range )
				{
					unitAI.state = WorkerState::HEAL;
					unitMove.target = unitTransform.pos;
					unitAttack.timer += GetFrameTime();
				}
				else
				{
					unitAI.state = WorkerState::HEAL;
					unitMove.target = closestEnemy->getComponent< CTransform3D >().pos;
					unitAttack.timer = 0.0f; // TODO? gradually decrease timer rather than set to 0
				}
			}
			else if ( unitTransform.pos.x == unitMove.target.x && unitTransform.pos.y == unitMove.target.y )
			{
				unitAI.state = WorkerState::ROAM;
				unitAI.target = nullptr;
			}
		}
	}
	
	void updateState()
	{
		std::vector< std::shared_ptr< Entity > > buildings = entities.getEntities("Building");
		std::vector< std::shared_ptr< Entity > > workers = entities.getEntities("Worker");
		std::vector< std::shared_ptr< Entity > > units = entities.getEntities("Unit");
		std::vector< std::shared_ptr< Entity > > enemies = entities.getEntities("Enemy");
		updateStateEnemy(enemies, buildings, workers, units);
		updateStateWorker(workers, buildings);
		updateStateUnit(units, enemies);
	}
	
	void updateMovement()
	{
		for (std::shared_ptr< Entity > entity : entities.getEntities())
		{
			// movement
			CMove& movement = entity->getComponent< CMove >();
			CTransform3D& transform = entity->getComponent< CTransform3D >();
			if ( movement.owned )
			{
				if ( movement.target.x >= 509 )
				{
					movement.target.x = 508;
				}
				if ( movement.target.x <= -509 )
				{
					movement.target.x = -508;
				}
				if ( movement.target.y >= 360 )
				{
					movement.target.y = 359;
				}
				if ( movement.target.y <= -360 )
				{
					movement.target.y = -359;
				}
				Vector3 direction = (Vector3){movement.target.x - transform.pos.x, movement.target.y - transform.pos.y, 0.0f};
				float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
				if ( distance > 0.0f )
				{
					if ( movement.speed >= distance )
					{
						transform.pos.x = movement.target.x;
						transform.pos.y = movement.target.y;
					}
					else
					{
						transform.pos.x += direction.x / distance * movement.speed;
						transform.pos.y += direction.y / distance * movement.speed;
					}
				}
			}
		}
	}
	
	void updateDamage()
	{
		for ( std::shared_ptr< Entity > entity : entities.getEntities() )
		{
			CAttack& attack = entity->getComponent< CAttack >();
			if ( entity->hasComponent< CWorker >() && entity->getComponent< CWorker >().state == WorkerState::HEAL && attack.timer >= attack.rate )
			{
				const Vector3& entityPos = entity->getComponent< CTransform3D >().pos;
				const Vector3& targetPos = entity->getComponent< CMove >().target;
				Vector3 direction = (Vector3){targetPos.x - entityPos.x, targetPos.y - entityPos.y, 0.0f};
				float distance = direction.x * direction.x + direction.y * direction.y;
				if ( distance <= attack.range )
				{
					std::shared_ptr< Entity > target = entity->getComponent< CWorker >().target;
					CHealth& targetHealth = target->getComponent< CHealth >();
					targetHealth.hp -= attack.damage;
					attack.timer = 0.0f;
				}
			}
			CHealth& health = entity->getComponent< CHealth >();
			if ( health.owned )
			{
				CBuilding& building = entity->getComponent< CBuilding >();
				if ( health.hp < 0 )
				{
					notify(entity, EntityEvent::DESTROY);
				}
				else if ( building.owned && building.state == BuildingState::CONSTRUCTION && health.hp >= health.max )
				{
					health.hp = health.max;
					notify(entity, EntityEvent::BUILD);
				}
			}
		}
	}
	
	virtual void update()
	{
		entities.update();
	}
	
	virtual void render()
	{
		for (std::shared_ptr< Entity > entity : entities.getEntities() )
		{
			CTransform3D& transform = entity->getComponent< CTransform3D >();
			CModel& model = entity->getComponent< CModel >();
			CHealth& health = entity->getComponent< CHealth >();
			if ( transform.owned && model.owned )
			{
				Color modelColor = WHITE;
				if ( health.owned )
				{
					float healthRatio = health.hp / health.max;
					modelColor.g *= healthRatio;
					modelColor.b *= healthRatio;
				}
				DrawModelEx(*(entity->getComponent< CModel >().model.get()), transform.pos, transform.axis, transform.angle, transform.scale, modelColor);
			}
		}
	}
	void initialize()
	{
		// std::shared_ptr< Entity > entity = entities.addEntity("Worker", "Goblin");
		// entity->addComponent< CTransform3D >((Vector3){0.0f, -360.0f, 0.0f}, Vector3{25.0f, 25.0f, 25.0f});
		// entity->addComponent< CWorker >();
		// entity->addComponent< CHealth >(50.0f, 50.0f);
		// entity->addComponent< CMove >(80.0f, (Vector3){0.0f, -360.0f, 0.0f});
		// CModel& modelComponent = entity->addComponent< CModel >();
		// modelComponent.model = LoadModel("./assets/character-orc.glb");
	}
};

class GameClock : public NoMVC::Model
{
private:
	bool paused = false;
	size_t frame = 0;
public:
	GameClock(bool pause=false)
		: paused(pause) {std::cout << GetFPS() << std::endl;}
	void update()
	{
		if (!paused)
		{
			frame++;
		}
	}
	void render() {}
	void setPaused(bool pause)
	{
		paused = pause;
	}
	bool getPaused()
	{
		
		return paused;
	}
	size_t getFrame()
	{
		
		return frame;
	}
	float getTime()
	{
		
		return GetFrameTime() * frame;
	}
};

class GameResources : public NoMVC::Model
{
public:
	float mana = 0.0f;
	float maxMana = 100.0f;
	float manaGen = 0.05f;
	size_t workers = 1;
	size_t maxWorkers = 1;
	size_t units = 0;
	size_t maxUnits = 2;
	void update() {}
	void render() {}
};

#endif