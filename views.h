#ifndef GAME_SCENES_H
#define GAME_SCENES_H
#include "../../libs/NoMVC/src/Controller.h"
#include "models.h"
#include <unordered_set>

const float CAMERAANGLE = 45.0f * (PI / 180.0f);
enum class GameState {LOSS = 0, RUNNING = 1, PAUSED = 2, VICTORY = 3};

Vector3 convert2DPos3D(Vector2 position2D);
inline Camera camera = { 0 };

class Scene : public NoMVC::View, public NoGUI::Listener, public Listener
{
private:
	GameState state = GameState::RUNNING;
	float unitSelectionProgress = 0.0f;
	const unsigned int unitSelectionTime = 1200; // in milliseconds
protected:
	void setVictory(bool win);
	void setPaused(bool pause);
public:
	Action currentAction;
	Vector3 portalSize;
	Scene(NoMVC::Controller* g, NoMVC::WindowConfig conf)
		: NoMVC::View(g, conf) 
	{
		camera.position = (Vector3){ 0.0f, 360.0f, -360.0f };
		camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
		camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
		camera.fovy = 720.0f * std::sin(CAMERAANGLE);
		camera.projection = CAMERA_ORTHOGRAPHIC;
		float scaleFactorX = conf.height / (CELLSX * SQRT3) * 2 * 0.257f;
		float scaleFactorY = scaleFactorX - 2.0f;
		float scaleFactorZ = scaleFactorY / 2.0f;
		portalSize = (Vector3){scaleFactorX, scaleFactorY, scaleFactorZ};
	}
	std::vector< std::shared_ptr< NoGUI::Element > > getSurrondingCells(std::shared_ptr< NoGUI::Element > centerElem, unsigned int radius=1);
	void togglePause();
	void animateUnitSelection();
	void animateManaBar(unsigned int current, unsigned int start, unsigned int end);
	void animateSwampCells(unsigned int current, unsigned int frames=75);
	void animateElements();
	void animateSpellHeal(unsigned int current, unsigned int start, unsigned int end, Color circleColor=(Color){255, 203, 0, 80}, float radius=100.0f);
	void animateSpellCommand(unsigned int current, unsigned int start, unsigned int end, Color innerCircleColor=MAROON, Color outerCircleColor=BLACK);
	void animateSpells();
	void spawnEntities();
	void playNoMana();
	void playFullPortal();
	void placeMonument(std::shared_ptr< Tile > tile);
	void placePortal(std::shared_ptr< Tile > tile);
	std::vector< std::shared_ptr< NoGUI::Element > > buildMonument(std::shared_ptr< Tile > tile);
	void castCommand(const Vector3& point, unsigned int animFrames=48);
	void castHeal(const Vector3& point, float amount=20.0f, float radius=100.0f, unsigned int animFrames=60);
	void castSummon(const Vector3& point, SpawnType creature);
	void castSpell(const Vector3& point, SpellType spell);
	void render();
	void run();
	void onNotify(std::shared_ptr< NoGUI::Element > elem, NoGUI::HoverEvent hevent, NoGUI::FocusEvent fevent);
	void onNotify(std::shared_ptr< Entity > entity, EntityEvent event);
	void initialize();
};

#endif