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

		/// Constructs a project manager.
		/// @param editorSupport Whether editor support is enabled.
		Project(bool editorSupport);

		/// Destroys the project.
		~Project();

		/// @name Project-related methods
		//@{

			/// Creates a new project that will be saved in the specified path and opens it.
			/// @return Returns true if new project was created.
			bool CreateProject(const string& path);

			/// Opens a project in the specified path.
			bool OpenProject(const string& path);

			/// Saves project configuration file and closes the currently opened project.
			void CloseProject();

			/// Returns whether there is an opened project.
			bool IsProjectOpened() const;

			/// Returns the path of the opened project. Returns empty string if there is not an opened project.
			string GetOpenedProjectPath() const;

			/// Stores the project info into specified variable.
			void GetOpenedProjectInfo(ProjectInfo& projectInfo) const;

			/// Sets the project info according to specified variable.
			void SetOpenedProjectInfo(const ProjectInfo& newProjectInfo);
		
		//@}
		
		/// @name Scene-related methods
		//@{

			/// Creates the scene with a given file name and scene name.
			/// @return False, if scene cannot be created, or another scene is already opened; otherwise returns True.
			bool CreateScene(const string& sceneFilename, const string& sceneName);

			/// Opens the scene with given file name.
			/// @return False, if scene is not found, or another scene is already opened; otherwise returns True.
			bool OpenScene(const string& scene);

			/// Opens the scene at given index in SceneList.
			/// @return False, if scene is not found, or another scene is already opened; otherwise returns True.
			bool OpenSceneAtIndex(int index);

			/// Opens the default scene of the project.
			/// @return False, if scene is not found, or another scene is already opened; otherwise returns True.
			bool OpenDefaultScene();

			/// Saves the opened scene.
			/// @return True, if scene was successfully saved.
			bool SaveOpenedScene();

			/// Closes the opened scene.
			void CloseOpenedScene();

			/// Returns true if a scene is opened.
			bool IsSceneOpened() const;

			/// Returns the list of scenes.
			void GetSceneList(SceneInfoList& scenes) const;

			/// Returns the number of scenes.
			size_t GetSceneCount() const;

			/// Returns name of the opened scene, or empty string if no scene is opened.
			string GetOpenedSceneName() const;

		//@}

	private:

		/// Sets the project info of the opened project to default values.
		void SetDefaultProjectInfo();

		/// Loads the configuration file of opened project.
		void LoadProjectConfig();
		
		/// Saves the configuration file of opened project.
		void SaveProjectConfig();

		string mProjectPath;
		ProjectInfo mProjectInfo;
		Core::Config* mProjectConfig;
		ResourceSystem::ResourceTypeMap mResourceTypeMap;

		SceneInfoList mSceneList;
		int mSceneIndex;
		
		bool mEditorSupport;
	};
}

#endif // __EDITOR_PROJECTMGR_H__
