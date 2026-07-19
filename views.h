#ifndef GAME_SCENES_H
#define GAME_SCENES_H
#include "../../libs/NoMVC/src/Controller.h"
#include "models.h"
#include <unordered_set>

const float CAMERAANGLE = 45.0f * (PI / 180.0f);
enum class GameState {LOSS = 0, RUNNING = 1, PAUSED = 2, VICTORY = 3};

Vector3 convert2DPos3D(Vector2 position2D);

class Scene : public NoMVC::View, public NoGUI::Listener, public Listener
{
private:
	GameState state = GameState::RUNNING;
protected:
	void setVictory(bool win);
public:
	Camera camera = { 0 };
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
	std::vector< std::shared_ptr< NoGUI::Element > > getSurrondingCells(std::shared_ptr< NoGUI::Element > centerElem, unsigned int radius=1);
	void animateElements();
	void spawnEntities();
	void placeMonument(std::shared_ptr< Tile > tile);
	void placePortal(std::shared_ptr< Tile > tile);
	std::vector< std::shared_ptr< NoGUI::Element > > buildMonument(std::shared_ptr< Tile > tile);
	void render();
	void run();
	void onNotify(std::shared_ptr< NoGUI::Element > elem, NoGUI::HoverEvent hevent, NoGUI::FocusEvent fevent);
	void onNotify(std::shared_ptr< Entity > entity, EntityEvent event);
	void initialize();
};

#endif