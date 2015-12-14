#pragma once

#include <MY_ResourceManager.h>

Scenario * MY_ResourceManager::scenario = nullptr;
std::vector<Scenario*> MY_ResourceManager::events;

void MY_ResourceManager::init(){
	for (int i = 1; i < 3; i++) {
		std::stringstream ss;
		ss << "assets/scenarios/scenario" << i << ".json";

		Scenario * scen = new Scenario(ss.str());
		events.push_back(scen);
		resources.push_back(scen);
	}

	scenario = new Scenario("assets/scenario.json");
	resources.push_back(scenario);
}