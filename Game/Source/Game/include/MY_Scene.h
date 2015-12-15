#pragma once

#include <Scene.h>
#include <UILayer.h>
#include <UIEvent.h>
#include <BulletDebugDrawer.h>
#include <BulletWorld.h>

class PerspectiveCamera;
class MousePerspectiveCamera;

class Box2DWorld;
class Box2DDebugDrawer;
class Box2DMeshEntity;
class MeshEntity;

class ShaderComponentHsv;

class Shader;
class RenderSurface;
class StandardFrameBuffer;
class Material;
class Sprite;

class PointLight;

class BulletMeshEntity;
class ComponentShaderText;

class MY_Scene : public Scene{
public:
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	
	ComponentShaderBase * baseShader;
	ComponentShaderBase * characterShader;
	ComponentShaderText * textShader;

	BulletWorld * bulletWorld;
	BulletDebugDrawer * debugDrawer;
	
	MousePerspectiveCamera * playerCam;
	
	Sprite * mouseIndicator;
	Sprite * crosshair;
	MousePerspectiveCamera * debugCam;

	Box2DWorld * box2dWorld;
	Box2DDebugDrawer * box2dDebug;

	UIEvent * uiEvent;

	float lastEventTime;
	float nextEventTime;
	Scenario * eventToUse;
	bool gameover;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	virtual void load() override;
	virtual void unload() override;

	void getNextEvent();

	UILayer uiLayer;

	MY_Scene(Game * _game);
	~MY_Scene();
};