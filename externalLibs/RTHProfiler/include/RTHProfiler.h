/***************************************************************************************************
**
** profile.h
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

#ifndef RTHProfiler_h__
#define RTHProfiler_h__

#include "Setup/BasicTypes.h"

namespace RTHProfiler
{
	/*
	** A node in the Profile Hierarchy Tree
	*/
	class	CProfileNode {

	public:
		CProfileNode( const char * name, CProfileNode * parent );
		~CProfileNode( void );

		CProfileNode * Get_Sub_Node( const char * name );

		CProfileNode * Get_Parent( void )		{ return Parent; }
		CProfileNode * Get_Sibling( void )		{ return Sibling; }
		CProfileNode * Get_Child( void )			{ return Child; }

		void				Reset( void );
		void				Call( void );
		bool				Return( void );

		const char *	Get_Name( void )				{ return Name; }
		int				Get_Total_Calls( void )		{ return TotalCalls; }
		float				Get_Total_Time( void )		{ return TotalTime; }

	protected:

		const char *	Name;
		int				TotalCalls;
		float				TotalTime;
		int64			StartTime;
		int				RecursionCounter;

		CProfileNode *	Parent;
		CProfileNode *	Child;
		CProfileNode *	Sibling;
	};

	/*
	** An iterator to navigate through the tree
	*/
	class CProfileIterator
	{
	public:
		// Access all the children of the current parent
		void				First(void);
		void				Next(void);
		bool				Is_Done(void);

		void				Enter_Child( int index );		// Make the given child the new parent
		void				Enter_Current_Child( void );	// Make the current child the new parent
		void				Enter_Largest_Child( void );	// Make the largest child the new parent
		void				Enter_Parent( void );			// Make the current parent's parent the new parent

		// Access the current child
		const char *	Get_Current_Name( void )			{ return CurrentChild->Get_Name(); }
		int				Get_Current_Total_Calls( void )	{ return CurrentChild->Get_Total_Calls(); }
		float				Get_Current_Total_Time( void )	{ return CurrentChild->Get_Total_Time(); }

		// Access the current parent
		const char *	Get_Current_Parent_Name( void )			{ return CurrentParent->Get_Name(); }
		int				Get_Current_Parent_Total_Calls( void )	{ return CurrentParent->Get_Total_Calls(); }
		float				Get_Current_Parent_Total_Time( void )	{ return CurrentParent->Get_Total_Time(); }

	protected:

		CProfileNode *	CurrentParent;
		CProfileNode *	CurrentChild;

		CProfileIterator( CProfileNode * start );
		friend	class		CProfileManager;
	};


	/*
	** The Manager for the Profile system
	*/
	class CProfileManager {
	public:
		static void Start_Profile( const char * name );
		static void Stop_Profile( void );

		static void Reset( void );
		static void Update( void );
		static void Pause( void );
		static void Resume( void );
		inline static bool Is_Enabled(void) { return Enabled; }
		static int Get_Frame_Count_Since_Reset( void ) { return FrameCounter; }
		static float Get_Time_Since_Reset( void );
		static CProfileIterator* Get_Iterator( void ) { return new CProfileIterator( &Root ); }
		static void Release_Iterator( CProfileIterator * iterator ) { delete iterator; }

	private:
		static CProfileNode Root;
		static CProfileNode* CurrentNode;
		static int FrameCounter;
		static int64 ResetTime;
		static int64 TimeAccumulator;
		static bool Enabled;

		static void Update_Time_Accumulator( void );
	};


	/*
	** ProfileSampleClass is a simple way to profile a function's scope
	** Use the PROFILE macro at the start of scope to time
	*/
	class	CProfileSample {
	public:
		inline CProfileSample( const char * name )
		{ 
			CProfileManager::Start_Profile( name ); 
		}

		inline ~CProfileSample( void )					
		{ 
			CProfileManager::Stop_Profile(); 
		}
	};

}

#endif // RTHProfiler_h__
