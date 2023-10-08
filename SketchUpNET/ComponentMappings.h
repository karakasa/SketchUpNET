#pragma once
#include <map>
#include <string>
#include <SketchUpAPI/slapi.h>
#include <SketchUpAPI/geometry.h>
#include <SketchUpAPI/initialize.h>
#include <SketchUpAPI/unicodestring.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/vertex.h>

namespace component_mapping
{
	extern std::map<std::string, SUComponentDefinitionRef>* mappings 
		= new std::map<std::string, SUComponentDefinitionRef>();
}