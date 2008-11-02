#ifndef LoadingScreen_h__
#define LoadingScreen_h__

#include "../Utility/StateMachine.h"
#include "../ResourceSystem/IResourceLoadingListener.h"

namespace Core
{
	/** Loads resource gruops into memory and displays info about the loading progress.
	*/
	class LoadingScreen : public ResourceSystem::IResourceLoadingListener
	{
	public:
		LoadingScreen() {}
		virtual ~LoadingScreen() {}

		enum eType { TYPE_BASIC_RESOURCES=0, TYPE_GENERAL_RESOURCES };

		/// Main entry point.
		void DoLoading(eType type);

		/// Update visuals.
		void Draw();

		/// Callbacks from the resource manager.
		//@{
		virtual void ResourceGroupLoadStarted(const string& groupName, uint32 resourceCount);
		virtual void ResourceLoadStarted(const ResourceSystem::ResourcePtr& resource);
		virtual void ResourceLoadEnded(void);
		virtual void ResourceGroupLoadEnded(void);
		//@}

	private:
		eType mType;

		void Init();
		void Deinit();
	};
}

#endif // LoadingScreen_h__