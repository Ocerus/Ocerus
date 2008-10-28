#ifndef _IMAGESET_MGR_H
#define _IMAGESET_MGR_H

#define gImagesetMgr GUISystem::GUIMgr::ImagesetMgr::GetSingleton()

namespace GUIMgr {
	class ImagesetMgr : public Singleton<ImagesetMgr>
	{
	public:
		Imageset* CreateImageset(const String& Name, Texture* Texture);
		Imageset* CreateImageset(const String& FileName, const String& ResourceGroup = "");
		Imageset* CreateImagesetFromImageFile(const String& Name, const String& FileName, const String& ResourceGroup = "");

		void DestroyImageset(const String& Name);

		void DestroyImageset(Imageset* Imageset);

		void DestroyAllImagesets();

		Imageset* GetImageset(const String& Name) const;

		bool IsImagesetPresent(const String& Name) const;

		void NotifyScreenResolution(const Size& Size);

		void WriteImagesetToStream(const String& Imageset, OutStream& OStream) const;

		typedef	ConstBaseIterator<ImagesetRegistry>	ImagesetIterator;
		ImagesetIterator GetIterator() const;

		ImagesetMgr();
		virtual ~ImagesetMgr();
	};
}
#endif