/// @file
///
#ifndef __EDITOR_PROJECTMGR_H__
#define __EDITOR_PROJECTMGR_H__

#include "Base.h"
#include "ResourceSystem/ResourceTypes.h"

namespace Core
{
	/// The SceneInfo structure holds information about a scene.
	struct SceneInfo
	{
		string name;       ///< Name of the scene
		string filename;   ///< Name of the XML file with scene (name of resource)

		/// Operator for comparing to scene names.
		bool operator==(const string& sceneName) const { return name == sceneName; }
	};

	/// The list of scenes.
	typedef vector<SceneInfo> SceneInfoList;

	/// The ProjectInfo structure holds information about a project.
	struct ProjectInfo
	{
		string name;      ///< Name of the project
		string version;   ///< Version of the project
		string author;    ///< Author of the project
	};

	/// The Project class manages projects and its scenes.
	class Project
	{
	public:

		/// Project file name.
		static const char* PROJECT_FILE_NAME;

		/// Constructs a project manager.
		/// @param editorSupport Whether editor support is enabled.
		Project(bool editorSupport);

		/// Destroys the project.
		~Project();

		/// @name Project-related methods
		//@{

			/// Creates a new project that will be saved in the specified path and opens it.
			/// @return Returns true if new project was created.
			bool CreateProject(const string& name, const string& path);

			/// Opens a project in the specified path.
			bool OpenProject(const string& path);

			/// Saves project configuration file and closes the currently opened project.
			void CloseProject();

			/// Returns whether there is an opened project.
			bool IsProjectOpened() const;

			/// Returns the path of the opened project. Returns empty string if there is not an opened project.
			string GetOpenedProjectPath() const;
			
			/// Returns the absolute path of the opened project.
			string GetAbsoluteOpenedProjectPath() const;

			/// Stores the project info into specified variable.
			void GetOpenedProjectInfo(ProjectInfo& projectInfo) const;

			/// Sets the project info according to specified variable.
			void SetOpenedProjectInfo(const ProjectInfo& newProjectInfo);

			/// Returns the project config.
			inline Config* GetConfig() { return mProjectConfig; }
		
		//@}
		
		/// @name Scene-related methods
		//@{

			/// Makes sure the scene name is correct and returns it as a result.
			static string FixSceneName(const string& oldName);

			/// Makes sure the scene filename is correct and returns it as a result.
			static string FixSceneFilename(const string& oldName);

			/// Creates the scene with a given file name and scene name.
			/// @return False, if scene cannot be created, or another scene is already opened; otherwise returns True.
			bool CreateScene(const string& sceneFilename, const string& sceneName);

			/// Opens the scene at given index in SceneList.
			/// @return False, if scene is not found, or another scene is already opened; otherwise returns True.
			bool OpenSceneAtIndex(int32 index);
			
			/// Opens the scene with a given file name.
			/// @return False, if scene is not found, otherwise returns true.
			bool OpenScene(const string& scene) { return OpenSceneAtIndex(GetSceneIndex(scene)); }

			/// Opens the scene from a given resource.
			void OpenScene(const ResourceSystem::ResourcePtr resource);

			/// Opens the default scene of the project.
			/// @return False, if scene is not found, or another scene is already opened; otherwise returns True.
			bool OpenDefaultScene();

			/// Saves the opened scene.
			/// @return True, if scene was successfully saved.
			bool SaveOpenedScene();

			/// Closes the opened scene.
			void CloseOpenedScene();

			/// Renames the scene in the list.
			void RenameScene(const string& oldName, const string& newName, const string& newFilename);

			/// Saves the association between specified resource and resource type to the resource type map.
			void SetResourceType(ResourceSystem::ResourcePtr resource, ResourceSystem::eResourceType newType);
			
			/// Returns true if a scene is opened.
			bool IsSceneOpened() const;

			/// Returns true if the given resource contains a scene.
			bool IsResourceScene(const ResourceSystem::ResourcePtr resource) const;

			/// Returns the list of scenes.
			void GetSceneList(SceneInfoList& scenes);

			/// Makes sure all scene are present in the project.
			void RefreshSceneList();
			
			/// Gets the index of the scene with a specific name.
			/// @return Index of the scene or -1 if no such a scene exist.
			int32 GetSceneIndex(const string& scene) const;

			/// Returns the number of scenes.
			size_t GetSceneCount() const;

			/// Returns name of the opened scene, or empty string if no scene is opened.
			string GetOpenedSceneName() const;
			
			/// Forces the state of the project as the specified scene was opened.
			/// Use only when the scene is loaded from another source.
			void ForceOpenSceneName(const string& scene);
			
			/// Opens the scene at given index in SceneList in the next game loop.
			bool RequestOpenSceneAtIndex(int32 index);
			
			/// Opens the scene with a given file name in the next game loop.
			bool RequestOpenScene(const string& scene) { return RequestOpenSceneAtIndex(GetSceneIndex(scene)); }
			
			/// Returns the scene file name which will be openened in the next game loop.
			string GetRequestedSceneName() const;
			
			/// Updates project.
			void Update();

		//@}

	private:

		/// Sets the project info of the opened project to default values.
		void SetDefaultProjectInfo();

		/// Loads the configuration file of opened project.
		void LoadProjectConfig();
		
		/// Saves the configuration file of opened project.
		void SaveProjectConfig();

		/// Creates default directories and files for new project.
		void CreateDefaultProjectStructure();
		
		string mProjectPath;
		ProjectInfo mProjectInfo;
		Core::Config* mProjectConfig;
		ResourceSystem::ResourceTypeMap mResourceTypeMap;

		SceneInfoList mSceneList;
		int32 mSceneIndex;
		int32 mRequestSceneIndex;
		
		bool mEditorSupport;
	};
}

#endif // __EDITOR_PROJECTMGR_H__
