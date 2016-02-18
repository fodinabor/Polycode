CC=g++
INCDIR=../include/
SRCDIR=../src/lua/
PYTHON_EXECUTABLE=python

CFLAGS=-I$(INCDIR) -I$(INCDIR)lua5.1

default:
	$(PYTHON_EXECUTABLE) create_lua_library/create_lua_library.py $(INCDIR)polycode/core/ Polycode Polycode.h polycore Polycore lua/core/API lua/core/API/Polycode $(INCDIR)polycode/lua/core $(SRCDIR)core
	@echo "\033[1;32mBuilding \033[0m[\033[1;33m$(SRCDIR)core/PolycodeLUA.cpp\033[0m]"
	@$(CC) $(CFLAGS) -I$(INCDIR)polycode/core -I$(INCDIR)polycode/lua/core -c $(SRCDIR)core/PolycodeLUA.cpp -o $(SRCDIR)core/PolycodeLUA.o
	
	
all: default modules

modules:
	$(PYTHON_EXECUTABLE) create_lua_library/create_lua_library.py $(INCDIR)polycode/modules/physics2D/ Physics2D Polycode2DPhysics.h physics2d Physics2D lua/physics2D/API lua/physics2D/API/Physics2D $(INCDIR)polycode/lua/physics2D $(SRCDIR)physics2D
	$(PYTHON_EXECUTABLE) create_lua_library/create_lua_library.py $(INCDIR)polycode/modules/physics3D/ Physics3D Polycode3DPhysics.h physics3d Physics3D lua/physics3D/API lua/physics3D/API/Physics3D $(INCDIR)polycode/lua/physics3D $(SRCDIR)physics3D
	$(PYTHON_EXECUTABLE) create_lua_library/create_lua_library.py $(INCDIR)polycode/modules/ui/ UI PolycodeUI.h ui UI lua/ui/API lua/ui/API/UI $(INCDIR)polycode/lua/ui $(SRCDIR)ui
