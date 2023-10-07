/*

SketchUpNET - a C++ Wrapper for the Trimble(R) SketchUp(R) C API
Copyright(C) 2015, Autor: Maximilian Thumfart

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#include <SketchUpAPI/slapi.h>
#include <SketchUpAPI/geometry.h>
#include <SketchUpAPI/initialize.h>
#include <SketchUpAPI/unicodestring.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/vertex.h>
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>
#include <vector>
#include <map>
#include <string>
#include "Utilities.h"
#include "Surface.h"
#include "Edge.h"
#include "Curve.h"
#include "Layer.h"
#include "Group.h"
#include "Instance.h"
#include "Component.h"
#include "OptionsManager.h"
#include "GeometryInput.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

namespace SketchUpNET
{
	public enum class SKPVersion
	{
		V2013,
		V2014,
		V2015,
		V2016,
		V2017,
		V2018,
		V2019,
		V2020,
		V2021,
		V3 = 100003,
		V4 = 100004,
		V5 = 100005,
		V6 = 100006,
		V7 = 100007,
		V8 = 100008
	};

	/// <summary>
	/// SketchUp Base Class
	/// </summary>
	public ref class SketchUp
	{


	public:
		/// <summary>
		/// Containing Model Surfaces
		/// </summary>
		System::Collections::Generic::List<Surface^>^ Surfaces;

		/// <summary>
		/// Containing Model Layers
		/// </summary>
		System::Collections::Generic::List<Layer^>^ Layers;

		/// <summary>
		/// Containing Model Groups
		/// </summary>
		System::Collections::Generic::List<Group^>^ Groups;

		/// <summary>
		/// Containing Model Component Definitions
		/// </summary>
		System::Collections::Generic::Dictionary<String^, Component^>^ Components;

		/// <summary>
		/// Containing Model Material Definitions
		/// </summary>
		System::Collections::Generic::Dictionary<String^, Material^>^ Materials;

		/// <summary>
		/// Containing Model Component Instances
		/// </summary>
		System::Collections::Generic::List<Instance^>^ Instances;

		/// <summary>
		/// Containing Model Curves (Arcs)
		/// </summary>
		System::Collections::Generic::List<Curve^>^ Curves; 

		/// <summary>
		/// Containing Model Edges (Lines)
		/// </summary>
		System::Collections::Generic::List<Edge^>^ Edges;

		/// <summary>
		/// Version of the loaded file is more recent than the SketchUp API
		/// </summary>
		bool MoreRecentFileVersion;

		OptionsManager^ Options;

		/// <summary>
		/// Loads a SketchUp Model from filepath without loading Meshes.
		/// Use this if you don't need meshed geometries.
		/// </summary>
		/// <param name="filename">Path to .skp file</param>
		bool LoadModel(System::String^ filename)
		{
			return LoadModel(filename, false);
		}

		/// <summary>
		/// Loads a SketchUp Model from filepath. Optionally load meshed geometries.
		/// </summary>
		/// <param name="filename">Path to .skp file</param>
		/// <param name="includeMeshes">Load model including meshed geometries</param>
		bool LoadModel(System::String^ filename, bool includeMeshes)
		{
			const char* path = Utilities::ToString(filename).get();

			SUInitialize();


			SUModelRef model = SU_INVALID;
			SUModelLoadStatus status;
			SUModelCreateFromFileWithStatus(&model, path, &status);	

			if (status == SUModelLoadStatus_Success_MoreRecent)
				MoreRecentFileVersion = true;
			else
				MoreRecentFileVersion = false;


			Layers = gcnew System::Collections::Generic::List<Layer^>();
			Groups = gcnew System::Collections::Generic::List<Group^>();
			Components = gcnew System::Collections::Generic::Dictionary<String^,Component^>();
			Materials = gcnew System::Collections::Generic::Dictionary<String^, Material^>();
			Options = SketchUpNET::OptionsManager::FromModel(model);

			SUEntitiesRef entities = SU_INVALID;
			SUModelGetEntities(model, &entities);

			//Get All Materials
			size_t matCount = 0;
			SUModelGetNumMaterials(model, &matCount);

			if (matCount > 0) {
				std::vector<SUMaterialRef> materials(matCount);
				SUModelGetMaterials(model, matCount, &materials[0], &matCount);

				for (size_t i = 0; i < matCount; i++) {
					Material^ mat = Material::FromSU(materials[i]);
					if (!Materials->ContainsKey(mat->Name))
						Materials->Add(mat->Name, mat);
				}
			}
			
			//Get All Layers
			size_t layerCount = 0;
			SUModelGetNumLayers(model, &layerCount);

			if (layerCount > 0) {
				std::vector<SULayerRef> layers(layerCount);
				SUModelGetLayers(model, layerCount, &layers[0], &layerCount);

				for (size_t i = 0; i < layerCount; i++) {
					Layer^ layer = Layer::FromSU(layers[i]);
					Layers->Add(layer);
				}
			}

			//Get All Groups	
			size_t groupCount = 0;
			SUEntitiesGetNumGroups(entities, &groupCount);

			if (groupCount > 0) {
				std::vector<SUGroupRef> groups(groupCount);
				SUEntitiesGetGroups(entities, groupCount, &groups[0], &groupCount);

				for (size_t i = 0; i < groupCount; i++) {
					Group^ group = Group::FromSU(groups[i], includeMeshes, Materials);
					Groups->Add(group);
				}

			}


			// Get all Components
			size_t compCount = 0;
			SUModelGetNumComponentDefinitions(model, &compCount);

			if (compCount > 0) {
				std::vector<SUComponentDefinitionRef> comps(compCount);
				SUModelGetComponentDefinitions(model, compCount, &comps[0], &compCount);

				for (size_t i = 0; i < compCount; i++) {
					Component^ component = Component::FromSU(comps[i], includeMeshes, Materials);
					Components->Add(component->Guid, component);
				}
			}

			Surfaces = Surface::GetEntitySurfaces(entities, includeMeshes, Materials);
			Curves = Curve::GetEntityCurves(entities);
			Edges = Edge::GetEntityEdges(entities);
			Instances = Instance::GetEntityInstances(entities, Materials);

			for each (Instance^ var in Instances)
			{
				if (Components->ContainsKey(var->ParentID))
				{
					System::Object^ o = Components[var->ParentID];
					var->Parent = o;
				}
			}

			for each (KeyValuePair<String^, Component^>^ cmp in Components)
			{
				FixRefs(cmp->Value);
			}

			for each (Group^ var in Groups)
			{
				FixRefs(var);
			}


			SUModelRelease(&model);
			SUTerminate();
			return true;

		};

		/// <summary>
		/// Saves a SketchUp Model from filepath to a new file.
		/// Use this if you want to convert a SketchUp file to a different format.
		/// </summary>
		/// <param name="filename">Path to original .skp file</param>
		/// <param name="version">SketchUp Version to save the new file in</param>
		/// <param name="newFilename">Path to new .skp file</param>
		bool SaveAs(System::String^ filename, SKPVersion version, System::String^ newFilename)
		{
			const char* path = Utilities::ToString(filename).get();
			SUInitialize();

			SUModelRef model = SU_INVALID;
			SUModelLoadStatus status;
			SUModelCreateFromFileWithStatus(&model, path, &status);

			if (status == SUModelLoadStatus_Success_MoreRecent)
				MoreRecentFileVersion = true;
			else
				MoreRecentFileVersion = false;

			SUModelVersion saveversion = ToSUVersion(version);

			SUModelSaveToFileWithVersion(model, Utilities::ToString(newFilename).get(), saveversion);

			SUModelRelease(&model);
			SUTerminate();
			return true;
		}

		/// <summary>
		/// Append current SketchUp Model Data to an existing SketchUp file.
		/// </summary>
		/// <param name="filename">Path to .skp file</param>
		bool AppendToModel(System::String^ filename)
		{
			const char* path = Utilities::ToString(filename).get();

			SUInitialize();


			SUModelRef model = SU_INVALID;

			SUModelLoadStatus status;
			SUModelCreateFromFileWithStatus(&model, path, &status);

			if (status == SUModelLoadStatus_Success_MoreRecent)
				MoreRecentFileVersion = true;
			else
				MoreRecentFileVersion = false;


			SUEntitiesRef entities = SU_INVALID;
			SUModelGetEntities(model, &entities);

			SUEntitiesAddFaces(entities, Surfaces->Count, Surface::ListToSU(Surfaces));
			SUEntitiesAddEdges(entities, Edges->Count, Edge::ListToSU(Edges));
			SUEntitiesAddCurves(entities, Curves->Count, Curve::ListToSU(Curves));

			SUModelSaveToFile(model, Utilities::ToString(filename).get());
			
			SUModelRelease(&model);
			SUTerminate();
			return true;

		};

		/// <summary>
		/// Write current SketchUp Model to a new SketchUp file using the latest version.
		/// </summary>
		/// <param name="filename">Path to .skp file</param>
		/// <returns></returns>
		bool WriteNewModel(System::String^ filename)
		{
			return WriteNewModel(filename, SketchUpNET::SKPVersion::V2021);
		}

		/// <summary>
		/// Write current SketchUp Model to a new SketchUp file using a specific version.
		/// </summary>
		/// <param name="filename">Path to .skp file</param>
		/// <param name="version">SketchUp version</param>
		/// <returns></returns>
		bool WriteNewModel(System::String^ filename, SketchUpNET::SKPVersion version)
		{
			SUInitialize();
			SUModelRef model = SU_INVALID;
			SUResult res = SUModelCreate(&model);

			if (res != SU_ERROR_NONE) return false;


			SUEntitiesRef entities = SU_INVALID;
			SUModelGetEntities(model, &entities);

			auto input = CreateGeometryInput(Surfaces, Edges, Curves);
			SUEntitiesFill(entities, input.ref(), true);

			WriteComponentsAndInstances(model, entities);

			SUModelVersion v = ToSUVersion(version);
			auto path = Utilities::ToString(filename);
			SUModelSaveToFileWithVersion(model, path.get(), v);
			SUModelRelease(&model);
			SUTerminate();

			return true;
		}

		private:

			void WriteComponentsAndInstances(const SUModelRef& model, const SUEntitiesRef& entities)
			{
				std::vector<SUComponentDefinitionRef> components(Components->Count);
				std::map<std::string, SUComponentDefinitionRef> mappings;

				auto enumerator = Components->GetEnumerator();
				int i = 0;

				while (enumerator.MoveNext()) {
					auto kv = enumerator.Current;
					auto comp = kv.Value->CreateEmptyObject();
					auto guid = (kv.Value)->Guid;
					auto component_name = msclr::interop::marshal_as<std::string>(guid);

					components[i] = comp;
					mappings[component_name] = comp;

					++i;
				}

				SUModelAddComponentDefinitions(model, components.size(), components.data());

				enumerator = Components->GetEnumerator();
				i = 0;

				while (enumerator.MoveNext()) {
					auto comp = enumerator.Current.Value;
					comp->FillContents(components[i]);

					++i;
				}

				for (auto i = 0; i < Instances->Count; i++) {
					auto instance = Instances[i];
					auto pid = instance->ParentID;

					auto component_name = msclr::interop::marshal_as<std::string>(pid);

					auto suInstance = instance->ToSU(mappings[component_name]);

					SUEntitiesAddInstance(entities, suInstance, nullptr);
				}
			}

			SUModelVersion ToSUVersion(SketchUpNET::SKPVersion version) {
				switch (version) {
				case SketchUpNET::SKPVersion::V2013:
					return SUModelVersion::SUModelVersion_SU2013;
				case SketchUpNET::SKPVersion::V2014:
					return SUModelVersion::SUModelVersion_SU2014;
				case SketchUpNET::SKPVersion::V2015:
					return SUModelVersion::SUModelVersion_SU2015;
				case SketchUpNET::SKPVersion::V2016:
					return SUModelVersion::SUModelVersion_SU2016;
				case SketchUpNET::SKPVersion::V2017:
					return SUModelVersion::SUModelVersion_SU2017;
				case SketchUpNET::SKPVersion::V2018:
					return SUModelVersion::SUModelVersion_SU2018;
				case SketchUpNET::SKPVersion::V2019:
					return SUModelVersion::SUModelVersion_SU2019;
				case SketchUpNET::SKPVersion::V2020:
					return SUModelVersion::SUModelVersion_SU2020;
				case SketchUpNET::SKPVersion::V2021:
					return SUModelVersion::SUModelVersion_SU2021;
				case SketchUpNET::SKPVersion::V3:
					return SUModelVersion::SUModelVersion_SU3;
				case SketchUpNET::SKPVersion::V4:
					return SUModelVersion::SUModelVersion_SU4;
				case SketchUpNET::SKPVersion::V5:
					return SUModelVersion::SUModelVersion_SU5;
				case SketchUpNET::SKPVersion::V6:
					return SUModelVersion::SUModelVersion_SU6;
				case SketchUpNET::SKPVersion::V7:
					return SUModelVersion::SUModelVersion_SU7;
				case SketchUpNET::SKPVersion::V8:
					return SUModelVersion::SUModelVersion_SU8;
				default:
					return SUModelVersion::SUModelVersion_SU2021;
				}
			}

			void FixRefs(Component^ comp)
			{
				for each (Instance^ var in comp->Instances)
				{
					if (Components->ContainsKey(var->ParentID))
					{
						System::Object^ o = Components[var->ParentID];
						var->Parent = o;

						FixRefs(Components[var->ParentID]);
					}
				}
			}

			void FixRefs(Group^ comp)
			{
				for each (Instance^ var in comp->Instances)
				{
					if (Components->ContainsKey(var->ParentID))
					{
						System::Object^ o = Components[var->ParentID];
						var->Parent = o;

						FixRefs(Components[var->ParentID]);
					}
				}
			}


	};


}