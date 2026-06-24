/*
 * Simple per-thread work-queue thread pool used by the multithreading sample.
 */

#ifndef FRAMEWORK_THREAD_POOL_HPP
#define FRAMEWORK_THREAD_POOL_HPP

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace VersalGPUSDK
{

/// @brief Implements a simple thread pool which can be used to submit rendering
/// work to multiple
/// threads. It does not aim to distribute chunks of work dynamically to
/// threads, but API users
/// must submit work to particular worker threads.
class ThreadPool
{
public:
	/// @brief Sets the number of worker threads to spawn.
	///
	/// This call is heavyweight and should not be called more than once during
	/// initialization.
	/// @param workerThreadCount The number of worker threads.
	void setWorkerThreadCount(unsigned workerThreadCount);

	/// @brief Gets the current number of worker threads.
	unsigned getWorkerThreadCount() const
	{
		return workerThreads.size();
	}

	/// @brief Pushes a bundle of work to a thread.
	/// @param threadIndex The worker thread to push work to.
	/// @param func A generic function object which will be executed by the
	/// thread.
	/// Using C++11 lambdas is the intended way to create these objects.
	void pushWorkToThread(unsigned threadIndex, std::function<void()> func);

	/// @brief Waits for all worker threads to complete all work they have been
	/// assigned.
	void waitIdle();

private:
	class Worker
	{
	public:
		Worker();
		~Worker();

		void pushWork(std::function<void()> func);
		void waitIdle();

	private:
		std::thread workerThread;
		std::mutex lock;
		std::condition_variable cond;

		std::queue<std::function<void()>> workQueue;
		bool threadIsAlive = true;

		void threadEntry();
	};

	std::vector<std::unique_ptr<Worker>> workerThreads;
};
}

#endif
