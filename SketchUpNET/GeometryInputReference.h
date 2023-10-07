#pragma once
#include <SketchUpAPI/slapi.h>
#include <SketchUpAPI/geometry.h>
#include <SketchUpAPI/initialize.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/layer.h>
#include <SketchUpAPI/model/vertex.h>
#include <SketchUpAPI/model/component_instance.h>
#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/drawing_element.h>
#include <iostream>

using namespace System;

#define CHECK(r) if (r != SU_ERROR_NONE) { throw gcnew System::InvalidOperationException(__FILE__ + " " + __LINE__ + " bad SU state."); }

namespace SketchUpNET
{
	class GeometryInputReference
	{
	public:
		GeometryInputReference() { SUGeometryInputCreate(&p); }
		~GeometryInputReference() { /*SUGeometryInputRelease(&p);*/ }

		GeometryInputReference(GeometryInputReference&& x)
		{
			p = x.p;
		}

		SUGeometryInputRef ref() {
			return p;
		}

		size_t add_point(const SUPoint3D& pt) {
			auto result = SUGeometryInputAddVertex(p, &pt);
			CHECK(result);
			if (result == SU_ERROR_NONE)
			{
				pt_count++;
				return pt_count - 1;
			}

			return -1;
		}
		
	private:
		size_t pt_count = 0;

		SUGeometryInputRef p;

		GeometryInputReference(GeometryInputReference const&) = delete;
		GeometryInputReference& operator= (GeometryInputReference const&) = delete;
	};

}