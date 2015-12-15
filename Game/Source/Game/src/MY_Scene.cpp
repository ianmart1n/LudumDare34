#pragma once

#include <MY_Scene.h>
#include <MY_ResourceManager.h>

#include <Game.h>
#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>
#include <Resource.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentText.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentMVP.h>

#include <MousePerspectiveCamera.h>

#include <System.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <NumberUtils.h>

#include <TextArea.h>
#include <Easing.h>

#include <UIEvent.h>

#include <RenderOptions.h>

MY_Scene::MY_Scene(Game * _game) :
	Scene(_game),
	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	baseShader(new ComponentShaderBase(true)),
	textShader(new ComponentShaderText(true)),
	uiLayer(0,0,0,0),
	fadeOutLength(5),
	fadeOutStart(-1)
{
	baseShader->addComponent(new ShaderComponentMVP(baseShader));
	baseShader->addComponent(new ShaderComponentTexture(baseShader));
	baseShader->compileShader();

	textShader->textComponent->setColor(glm::vec3(0.0f, 0.0f, 0.0f));

	// remove initial camera
	childTransform->removeChild(cameras.at(0)->parents.at(0));
	delete cameras.at(0)->parents.at(0);
	cameras.pop_back();

	//Set up debug camera
	playerCam = new MousePerspectiveCamera();
	cameras.push_back(playerCam);
	childTransform->addChild(playerCam);
	playerCam->farClip = 1000.f;
	playerCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	playerCam->parents.at(0)->translate(1.475f, 9.508f, 4.506f);
	playerCam->yaw = -90.0f;
	playerCam->pitch = -10.0f;
	playerCam->speed = 1;

	playerCam->interpolation = 1;

	activeCamera = playerCam;

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

	
	// reference counting for member variables
	++baseShader->referenceCount;
	++textShader->referenceCount;

	++screenSurface->referenceCount;
	++screenSurfaceShader->referenceCount;
	++screenFBO->referenceCount;
}

MY_Scene::~MY_Scene(){
	deleteChildTransform();
	// auto-delete member variables
	baseShader->decrementAndDelete();
	textShader->decrementAndDelete();

	screenSurface->decrementAndDelete();
	screenSurfaceShader->decrementAndDelete();
	screenFBO->decrementAndDelete();
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

	

	// event timer stuff
	if (_step->time > nextEventTime){
		if (uiEvent->readyForNewEvent){
			uiEvent->startEvent(eventToUse);
		}
		else{
			getNextEvent();
		}
	}

#ifdef _DEBUG
	if (keyboard->keyJustDown(GLFW_KEY_2)){
		Transform::drawTransforms = !Transform::drawTransforms;
	}
#endif

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