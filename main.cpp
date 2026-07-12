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
	gridModel->initialize();
	std::cout << "grid initialized" << std::endl;
	GUIModel->initialize();
	std::cout << "gui initialized" << std::endl;
    entityModel->initialize();
	std::cout << "entities initialized" << std::endl;
	scene->initialize();
	game.assets->addSound("nomana.wav", "./assets/nomana.wav");
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