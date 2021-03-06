/// @file
/// An application state when certain resources are loading.

#ifndef LoadingScreen_h__
#define LoadingScreen_h__

#include "StateMachine.h"
#include "../ResourceSystem/IResourceLoadingListener.h"

namespace Core
{
	/// Loads resource groups into the memory and displays info about the loading progress.
	/// The purpose of this application state is to make the user know that there are some data being loaded and it might
	/// take a while. The user is informed about the loading progress.
	class LoadingScreen : public ResourceSystem::IResourceLoadingListener
	{
	public:

		/// Constructor.
		LoadingScreen();

		/// Destructor.
		virtual ~LoadingScreen();

		/// Defines resource groups to be loaded.
		enum eType 
		{ 
			/// Basic resources needed for displaying the default application state.
			TYPE_BASIC_RESOURCES=0, 
			/// Resources needed in most of the states of the application.
			TYPE_GENERAL_RESOURCES,
			/// Resources needed in a scene.
			TYPE_SCENE,
			/// Resources for the editor.
			TYPE_EDITOR
		};

		/// Starts the resource loading.
		void DoLoading(eType type, const string& scene = "");

		/// Renders the current loading state.
		void Draw();

		/// @name Callbacks from ResourceSystem::IResourceLoadingListener
		//@{
		virtual void ResourceGroupLoadStarted(const string& groupName, uint32 resourceCount);
		virtual void ResourceLoadStarted(const ResourceSystem::Resource* resource);
		virtual void ResourceLoadEnded(void);
		virtual void ResourceGroupLoadEnded(void);
		//@}

	private:

		eType mType;
		GfxSystem::RenderTargetID mRenderTarget;
		Utils::Timer mAnimationTimer;
		Utils::Timer mAnimationEndTimer;
		int32 mAnimationFrame;

		void Init();
		void Clean();
	};
}

#endif // LoadingScreen_h__