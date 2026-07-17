#include "raylib.h"
#include "views.h"
#include <iostream>
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>      // Emscripten library
#endif

NoMVC::WindowConfig window = (NoMVC::WindowConfig){720, 720, 60};
NoMVC::Controller game = NoMVC::Controller(window);
Model goblinModel = LoadModel("assets/character-orc.glb");

void main_loop()
{
	game.update();
	game.draw();
}

int main()
{
	std::cout << "initializing game" << std::endl;
	std::shared_ptr< Scene > scene = std::make_shared< Scene >(&game, window);
	game.changeScene(scene);
	std::shared_ptr< GameClock > clockModel = std::make_shared< GameClock >(false);
	std::shared_ptr< GameResources > resourceModel = std::make_shared< GameResources >();
	std::shared_ptr< GameGrid > gridModel = std::make_shared< GameGrid >();
	std::shared_ptr< Overlay > GUIModel = std::make_shared< Overlay >();
	std::shared_ptr< EntitySystem > entityModel = std::make_shared< EntitySystem >();
	gridModel->addListener(scene);
	GUIModel->addListener(scene);
	game.addModel(clockModel);
	game.addModel(resourceModel);
	game.addModel(gridModel);
	game.addModel(GUIModel);
	game.addModel(entityModel);
	game.assets->addSound("nomana.wav", "./assets/nomana.wav");
	game.assets->addModel("worker", "./assets/character-orc.glb");
	game.assets->addModel("enemy", "./assets/character-human.glb");
	game.assets->addModel("obelisk", "./assets/unit-tower.glb");
	game.assets->addModel("construction", "./assets/wood-structure.glb");
	game.assets->addModel("portal", "./assets/magic_portal.glb");
	game.assets->addModel("town0", "./assets/building_home_A_yellow.gltf");
	game.assets->addModel("town1", "./assets/building_home_B_yellow.gltf");
	game.assets->addModel("town2", "./assets/building_barracks_yellow.gltf");
	game.assets->addModel("town3", "./assets/building_castle_yellow.gltf");
	std::cout << "assets loaded" << std::endl;
	gridModel->initialize();
	std::cout << "grid initialized" << std::endl;
	GUIModel->initialize();
	std::cout << "gui initialized" << std::endl;
    entityModel->initialize();
	std::cout << "entities initialized" << std::endl;
	scene->initialize();
	std::cout << "scene initialized" << std::endl;
#if defined(PLATFORM_WEB)
	std::cout << "running for web" << std::endl;
    emscripten_set_main_loop(main_loop, 60, 1);
#else
	std::cout << "running for desktop" << std::endl;
    while (!WindowShouldClose())
    {
		main_loop();
    }
#endif
	CloseWindow();
	return 0;
}