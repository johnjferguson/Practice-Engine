#pragma once
#include <memory>
#include "Engine.h"


class EngineState
{
public:
	virtual void Init() = 0;
	virtual void Cleanup() = 0;

	virtual void Pause() = 0;
	virtual void Resume() = 0;

	virtual void HandleEvents(Engine& engine) = 0;
	virtual void Update(Engine& engine) = 0;
	virtual void Render(Engine& engine) = 0;

	void ChangeState(Engine& engine, EngineState* pEngineState)
	{
		engine.ChangeState(pEngineState);
	}

	Window* GetWindow(Engine& engine)
	{
		return &engine.Wnd();
	}
	Engine* GetEngine(Engine& engine)
	{
		return &engine;
	}
protected:
	EngineState() {};
};
