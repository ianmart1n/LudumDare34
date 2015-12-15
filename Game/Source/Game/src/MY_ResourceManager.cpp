#pragma once

#include <MY_ResourceManager.h>

Scenario * MY_ResourceManager::scenario = nullptr;
Scenario * MY_ResourceManager::endScenario = nullptr;
std::vector<Scenario*> MY_ResourceManager::events;
std::vector<bool> MY_ResourceManager::eventUsed;

void MY_ResourceManager::init(){
	for (int i = 1; i < 3; i++) {
		std::stringstream ss;
		ss << "assets/scenarios/scenario" << i << ".json";

		Scenario * scen = new Scenario(ss.str());
		events.push_back(scen);
		eventUsed.push_back(false);
		resources.push_back(scen);
	}

	scenario = new Scenario("assets/scenario.json");

	endScenario = new Scenario("assets/scenarios/endScenario.json");
	resources.push_back(scenario);
}