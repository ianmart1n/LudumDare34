#pragma once

#include <MY_Scene.h>
#include <MY_ResourceManager.h>

#include <Game.h>
#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>
#include <Resource.h>

#include <DirectionalLight.h>
#include <PointLight.h>
#include <SpotLight.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentText.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentHsv.h>
#include <shader\ShaderComponentMVP.h>

#include <shader\ShaderComponentIndexedTexture.h>
#include <TextureColourTable.h>

#include <Box2DWorld.h>
#include <Box2DMeshEntity.h>
#include <Box2DDebugDrawer.h>

#include <MousePerspectiveCamera.h>
#include <FollowCamera.h>

#include <System.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <NumberUtils.h>

#include <NodeBulletBody.h>
#include <BulletMeshEntity.h>
#include <TextArea.h>
#include <Box2DWorld.h>
#include <Box2DDebugDrawer.h>
#include <Easing.h>

#include <UIEvent.h>

#include <RenderOptions.h>

MY_Scene::MY_Scene(Game * _game) :
	Scene(_game),
	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	baseShader(new ComponentShaderBase(true)),
	characterShader(new ComponentShaderBase(true)),
	textShader(new ComponentShaderText(true)),
	debugDrawer(nullptr),
	uiLayer(0,0,0,0),
	box2dWorld(new Box2DWorld(b2Vec2(0.f, -10.0f))),
	box2dDebug(new Box2DDebugDrawer(box2dWorld)),
	fadeOutLength(5),
	fadeOutStart(-1)
{
	hsv = new ShaderComponentHsv(baseShader, 0, 1, 1);
	baseShader->addComponent(new ShaderComponentMVP(baseShader));
	//baseShader->addComponent(new ShaderComponentDiffuse(baseShader));
	baseShader->addComponent(new ShaderComponentTexture(baseShader));
	baseShader->addComponent(hsv);
	baseShader->compileShader();

	textShader->textComponent->setColor(glm::vec3(0.0f, 0.0f, 0.0f));
	//screenSurface->unload();
	//screenSurface->load();

	// remove initial camera
	//childTransform->removeChild(cameras.at(0)->parents.at(0));
	//delete cameras.at(0)->parents.at(0);
	//cameras.pop_back();

	//Set up debug camera
	debugCam = new MousePerspectiveCamera();
	cameras.push_back(debugCam);
	childTransform->addChild(debugCam);
	debugCam->farClip = 1000.f;
	debugCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parents.at(0)->translate(1.475f, 9.508f, 4.506f);
	debugCam->yaw = -90.0f;
	debugCam->pitch = -10.0f;
	debugCam->speed = 1;

	debugCam->interpolation = 1;

	activeCamera = debugCam;

	childTransform->addChild(box2dDebug, false);
	box2dDebug->drawing = true;
	box2dWorld->b2world->SetDebugDraw(box2dDebug);
	box2dDebug->AppendFlags(b2Draw::e_shapeBit);
	box2dDebug->AppendFlags(b2Draw::e_centerOfMassBit);
	box2dDebug->AppendFlags(b2Draw::e_jointBit);

	//uiLayer.addMouseIndicator();

	MeshEntity* bus = new MeshEntity(Resource::loadMeshFromObj("assets/bus.obj").at(0), baseShader);
	childTransform->addChild(bus);
	bus->mesh->pushTexture2D(MY_ResourceManager::scenario->getTexture("BUS")->texture);

	MeshEntity* passenger = new MeshEntity(MeshFactory::getPlaneMesh(8), baseShader);
	childTransform->addChild(passenger);
	passenger->mesh->pushTexture2D(MY_ResourceManager::scenario->getTexture("PASSENGER")->texture);
	passenger->childTransform->translate(-9.5,4.4f,7.1f);
	passenger->childTransform->rotate(90, 0, 1, 0, kWORLD);

	uiEvent = new UIEvent(uiLayer.world, textShader);
	uiLayer.addChild(uiEvent);


	lastEventTime = 0;
	nextEventTime = 10;
	gameover = false;
	getNextEvent();

	MY_ResourceManager::endScenario->eventManager.addEventListener("gameover", [this](sweet::Event * _event){
		gameover = true;
	});

	MY_ResourceManager::scenario->getAudio("BGM")->sound->play(true);
}

void MY_Scene::getNextEvent(){
	if (!gameover){
		//check if there are events left
		bool done = true;
		for (bool b : MY_ResourceManager::eventUsed){
			if (!b){
				done = false;
				break;
			}
		}

		lastEventTime = glfwGetTime();
		nextEventTime = lastEventTime + sweet::NumberUtils::randomInt(12, 22);
	
		if (done){
			// there aren't any more scenarios, so just load the ending
			eventToUse = MY_ResourceManager::endScenario;
		} else{
			// find an unused event
			unsigned long int i = 0;
			do{
				i = sweet::NumberUtils::randomInt(0, MY_ResourceManager::events.size()-1);
			} while (MY_ResourceManager::eventUsed.at(i));
			eventToUse = MY_ResourceManager::events.at(i);
			MY_ResourceManager::eventUsed.at(i) = true;
		}
	}
}

MY_Scene::~MY_Scene(){
	deleteChildTransform();
	baseShader->safeDelete();
	characterShader->safeDelete();
	textShader->safeDelete();

	screenSurface->safeDelete();
	//screenSurfaceShader->safeDelete();
	screenFBO->safeDelete();
}


void MY_Scene::update(Step * _step){
	if (keyboard->keyJustDown(GLFW_KEY_ESCAPE)){
		game->exit();
	}


	if (!gameover){
		MY_ResourceManager::endScenario->eventManager.update(_step);
	}
	else{
		if (fadeOutStart < -1){
			fadeOutStart = _step->time;
		}
		else if(_step->time > fadeOutStart + fadeOutLength){
			game->exit();
		}
	}

	box2dWorld->update(_step);

	

	// event timer stuff
	if (_step->time > nextEventTime){
		if (uiEvent->readyForNewEvent){
			uiEvent->startEvent(eventToUse);
		}
		else{
			getNextEvent();
		}
	}


	if(keyboard->keyJustDown(GLFW_KEY_F12)){
		game->toggleFullScreen();
	}

	if (keyboard->keyJustDown(GLFW_KEY_1)){
		cycleCamera();
	}

	if (keyboard->keyJustDown(GLFW_KEY_2)){
		Transform::drawTransforms = !Transform::drawTransforms;
	}

	glm::uvec2 sd = sweet::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);
	Scene::update(_step);
	uiLayer.update(_step);
}

void MY_Scene::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
	//Bind frameBuffer
	screenFBO->bindFrameBuffer();
	//render the scene to the buffer
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);

	//Render the buffer to the render surface
	screenSurface->render(screenFBO->getTextureId());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	uiLayer.render(_matrixStack, _renderOptions);
}

void MY_Scene::load(){
	Scene::load();	

	screenSurface->load();
	screenFBO->load();
	uiLayer.load();
}

void MY_Scene::unload(){
	uiLayer.unload();
	screenFBO->unload();
	screenSurface->unload();

	Scene::unload();	
}