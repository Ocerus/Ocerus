#ifndef _COMPONENT_H_
#define _COMPONENT_H_

namespace EntitySystem
{
	class Component
	{
	public:
		virtual void Init(const ComponentDescription& desc) = 0;
		virtual void Deinit() = 0;

		virtual eEntityMessageResult HandleMessage(const EntityMessage& msg);

		static void LoadResources() = 0;
		static void ReleaseResources() = 0;

	private:
		Component();
		virtual ~Component();

}

#endif