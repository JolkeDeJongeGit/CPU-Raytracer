#pragma once
#include <vector>
#include <mutex>
namespace Util
{
	template<typename T>
	class SafeVector
	{
	public:
		SafeVector() {}
		SafeVector(T value) { push_back(std::move(value)); }
		T& operator [] (int i) { return get(i); }
		inline T& get(int i) { std::lock_guard<std::mutex> guard(mutex); return data[i]; }
		void pop() { std::lock_guard<std::mutex> guard(mutex); data.pop_back(); }
		void pop(int i) { std::lock_guard<std::mutex> guard(mutex); data.erase(data.begin() + i); }
		void push_back(T value) { std::lock_guard<std::mutex> guard(mutex); data.push_back(std::move(value)); }
		void clear() { std::lock_guard<std::mutex> guard(mutex); data.clear(); }
		size_t size() { std::lock_guard<std::mutex> guard(mutex); return data.size(); }
	private:
		std::mutex mutex;
		std::vector<T> data;
	};
}