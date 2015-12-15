#pragma once

#include <Scene.h>
#include <UILayer.h>
#include <UIEvent.h>

class PerspectiveCamera;
class MousePerspectiveCamera;

class MeshEntity;

class Shader;
class RenderSurface;
class StandardFrameBuffer;
class Material;
class Sprite;

class PointLight;
class ComponentShaderText;

class MY_Scene : public Scene{
public:
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	
	ComponentShaderBase * baseShader;
	ComponentShaderText * textShader;
	
	MousePerspectiveCamera * playerCam;

	UIEvent * uiEvent;

	float lastEventTime;
	float nextEventTime;
	Scenario * eventToUse;
	bool gameover;

	float fadeOutStart;
	float fadeOutLength;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	virtual void load() override;
	virtual void unload() override;

	void getNextEvent();

	UILayer uiLayer;

	MY_Scene(Game * _game);
	~MY_Scene();
};