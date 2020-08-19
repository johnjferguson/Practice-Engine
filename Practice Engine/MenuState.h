#pragma once
#include "EngineState.h"

class MenuState : public EngineState
{
public:
	MenuState(const MenuState&) = delete;
	MenuState& operator=(const MenuState&) = delete;

	void Init();
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(Engine& engine);
	void Update(Engine& engine);
	void Render(Engine& engine);

	static MenuState* Instance()
	{
		return &menuState;
	}
protected:
	MenuState() {};
private:
	static MenuState menuState;
};
