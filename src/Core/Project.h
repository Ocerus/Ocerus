/// @file
///
#ifndef __EDITOR_PROJECTMGR_H__
#define __EDITOR_PROJECTMGR_H__

#include "Base.h"
#include "ResourceSystem/ResourceTypes.h"

namespace Core
{

	class Project
	{
	public:

		/// The list of scenes.
		typedef vector<pair<string, string> > Scenes;

		struct ProjectInfo
		{
			string name;
			string version;
			string author;
		};

		/// Constructs a project manager.
		Project();

		/// Destroys the project.
		~Project();

		/// Creates a new project that will be saved in the specified path and opens it.
		/// Returns true if new project was created.
		bool CreateProject(const string& path);

		/// Opens a project in the specified path.
		bool OpenProject(const string& path, bool editorSupport);

		/// Closes the currently opened project.
		void CloseProject();

		/// Returns whether there is an opened project.
		inline bool IsProjectOpened() { return mIsOpened; }

		/// Returns the path of the opened project. Returns empty string if there is not an opened project.
		inline string GetOpenedProjectPath() { return mPath; }

		/// Stores the project info into specified variable.
		inline void GetProjectInfo(ProjectInfo& projectInfo) { projectInfo = mInfo; }

		/// Sets the project info according to specified variable.
		inline void SetProjectInfo(const ProjectInfo& newProjectInfo) { mInfo = newProjectInfo; }

		void GetScenes(Scenes& scenes) const;

		inline string GetCurrentScene() const { return mCurrentScene; }

		void OpenScene(const string& scene);

		void OpenSceneAtIndex(uint32 index);

	private:

		/// Sets the project info of the opened project to default values.
		void SetDefaultProjectInfo();

		/// Saves the configuration file of opened project.
		void SaveProject();

		bool mIsOpened;
		string mPath;
		ProjectInfo mInfo;
		Core::Config* mConfig;
		ResourceSystem::ResourceTypeMap mResourceTypeMap;
		Scenes mScenes;
		string mCurrentScene;
	};

}

#endif // __EDITOR_PROJECTMGR_H__
