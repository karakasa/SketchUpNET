#pragma once

// #include "Surface.h"
// #include "Edge.h"
// #include "Curve.h"
// #include "GeometryInputReference.h"

namespace SketchUpNET
{
	using namespace System::Collections::Generic;
	GeometryInputReference CreateGeometryInput(List<Surface^>^ surfaces, List<Edge^>^ edges, List<Curve^>^ curves);
}