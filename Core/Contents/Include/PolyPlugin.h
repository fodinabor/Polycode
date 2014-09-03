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

#pragma once
#include "PolyGlobals.h"
#include "PolyString.h"
#include "PolyResource.h"

namespace Polycode {
	class ObjectEntry;
	
	class _PolyExport Prop : public PolyBase {
	public:
		Prop(const String& name, const int type = Prop::PROP_STRING);
		Prop(ObjectEntry *entry);
		~Prop();

		Prop *loadPropFromEntry(ObjectEntry* entry);

		int type;
		String name;

		int length;

		std::vector<Prop*> children;

		static const unsigned int PROP_NUMBER	= 0;
		static const unsigned int PROP_INT		= 1;
		static const unsigned int PROP_BOOL		= 2;
		static const unsigned int PROP_STRING	= 3;
		static const unsigned int PROP_ARRAY	= 4;
	};

	class _PolyExport Plugin : public Resource {
		public:
			Plugin(const String& name);
			Plugin(ObjectEntry *entry);
			~Plugin();
		
			Plugin* loadPluginFromEntry(ObjectEntry *entry);

			void addProp(Prop* prop);
			void setProp(Prop* prop);

			void removeProp(Prop* prop);

			std::vector<Prop*> getProps() const;
			unsigned int getNumProps() const;

			unsigned int pluginType;

			static const unsigned int PLUGIN_ENTITY = 0;

			String ext;
			ObjectEntry *sheetEntry;

		protected:
			std::vector<Prop*> props;
	};
}