/// @file
/// Real-time interactive ingame profiling.

#ifndef Profiler_h__
#define Profiler_h__

#include "Base.h"
#include "Singleton.h"
#include <RTHProfiler.h>

/// Macro for easier use.
#define gProfiler LogSystem::Profiler::GetSingleton()

namespace LogSystem
{
	/// Real-time interactive ingame profiling. This class controls the behaviour of the profiler and provides measured
	/// data access.
	class Profiler: public Singleton<Profiler>
	{
	public:

		/// Initializes the profiler. The profiling is not automatically started.
		Profiler(void);

		/// Default destructor.
		~Profiler(void) {}
	
		/// Starts or resumes profiling of code performance.
		void Start(void);

		/// Stops the profiling of code so that no data are aggregated until started again.
		void Stop(void);

		/// Updates the profiler. To be called once per frame.
		void Update(void);

		/// Returns true if the profiler is measuring data.
		bool IsRunning(void);

		/// Writes the profiling results into the console.
		void DumpIntoConsole(void);

	};


	#ifdef USE_PROFILER
		/// Macro for profiling a block of code provided a custom string identifier to the profiling entry.
		#define PROFILE(sampleName) RTHProfiler::CProfileSample __profileSample(sampleName)
	#else
		#define PROFILE(sampleName) 
	#endif

	/// Macro for profiling a function.
	#define PROFILE_FNC() PROFILE(__FUNCTION__)
}


#endif // Profiler_h__