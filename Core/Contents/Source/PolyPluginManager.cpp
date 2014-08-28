/*
Copyright (C) 2014 by Joachim Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "PolyPluginManager.h"
#include "PolyPlugin.h"
#include "PolyObject.h"
#include "PolyLogger.h"

using namespace Polycode;

PluginManager::PluginManager() {
	version = 1.0;
}

PluginManager::~PluginManager(){}

std::vector<Plugin*> PluginManager::loadPluginsFromFile(const String &fileName){
	std::vector<Plugin*> retPlugins;
	Object pluginFile;
	if (!pluginFile.loadFromBinary(fileName))
		if (!pluginFile.loadFromXML(fileName))
			return retPlugins;
	
	//Logger::log("Loading plugins from %s", fileName.c_str());

	version = 1.0;

	//ObjectEntry* versionEntry = pluginFile.root["version"];
	//if (versionEntry->NumberVal > version)
	//	return retPlugins;
	
	//if (!pluginsEntry) {
	//	Logger::log("No plugins in %s\n", fileName.c_str());
	//	return retPlugins;
	//}
	
	for (int i = 0; i < pluginFile.root.children.size(); i++){
		ObjectEntry* pluginEntry = pluginFile.root.children[i];
		if (!pluginEntry || pluginEntry->name != "plugin")
			continue;

		Plugin *plugin = new Plugin(pluginEntry);
		if (plugin)
			retPlugins.push_back(plugin);
	}
	return retPlugins;
}