#include <yttrium/thread_buffer.h>

namespace Yttrium
{

ThreadBufferBase::ThreadBufferBase(size_t capacity)
	: _capacity(capacity)
	, _size(0)
	, _first(0)
{
}

ThreadBufferBase::~ThreadBufferBase()
{
}

void ThreadBufferBase::begin_read()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_read.wait(lock, [this]{ return _size > 0; });
	lock.release();
}

void ThreadBufferBase::begin_write()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_write.wait(lock, [this]{ return _size < _capacity; });
	lock.release();
}

void ThreadBufferBase::end_read()
{
	std::lock_guard<std::mutex> lock(_mutex, std::adopt_lock);
	_first = (_first + 1) % _capacity;
	--_size;
	if (_size == _capacity - 1)
		_write.notify_one();
}

void ThreadBufferBase::end_write()
{
	std::lock_guard<std::mutex> lock(_mutex, std::adopt_lock);
	++_size;
	if (_size == 1)
		_read.notify_one();
}

bool ThreadBufferBase::try_begin_read()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (!_size)
		return false;
	lock.release();
	return true;
}

bool ThreadBufferBase::try_begin_read(Clock milliseconds)
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (!_read.wait_for(lock, std::chrono::milliseconds(milliseconds), [this]{ return _size > 0; }))
		return false;
	lock.release();
	return true;
}

bool ThreadBufferBase::try_begin_write()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_size == _capacity)
		return false;
	lock.release();
	return true;
}

bool ThreadBufferBase::try_begin_write(Clock milliseconds)
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (!_write.wait_for(lock, std::chrono::milliseconds(milliseconds), [this]{ return _size < _capacity; }))
		return false;
	lock.release();
	return true;
}

} // namespace Yttrium
