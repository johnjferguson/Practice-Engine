#pragma once
#include "EngineState.h"

class FreeState : public EngineState
{
public:
	FreeState(const FreeState&) = delete;
	FreeState& operator=(const FreeState&) = delete;

	void Init();
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(Engine& engine);
	void Update(Engine& engine);
	void Render(Engine& engine);

	static FreeState* Instance()
	{
		return &freeState;
	}
protected:
	FreeState() {};
private:
	static FreeState freeState;
};