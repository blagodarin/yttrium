#include "thread_buffer.h"

namespace Yttrium
{

ThreadBufferBase::ThreadBufferBase(size_t capacity, Allocator *allocator)
	: _capacity(capacity)
	, _size(0)
	, _first(0)
	, _private(Y_NEW(allocator, Private)(allocator))
{
}

ThreadBufferBase::~ThreadBufferBase()
{
	Y_DELETE(_private->allocator, _private);
}

void ThreadBufferBase::begin_read()
{
	_private->mutex.lock();
	if (!_size)
	{
		_private->read.wait(&_private->mutex);
	}
}

void ThreadBufferBase::begin_write()
{
	_private->mutex.lock();
	if (_size == _capacity)
	{
		_private->write.wait(&_private->mutex);
	}
}

void ThreadBufferBase::end_read()
{
	_first = (_first + 1) % _capacity;
	--_size;
	if (_size == _capacity - 1)
	{
		_private->write.notify_one();
	}
	_private->mutex.unlock();
}

void ThreadBufferBase::end_write()
{
	++_size;
	if (_size == 1)
	{
		_private->read.notify_one();
	}
	_private->mutex.unlock();
}

bool ThreadBufferBase::try_begin_read()
{
	_private->mutex.lock();
	if (!_size)
	{
		_private->mutex.unlock();
		return false;
	}
	return true;
}

bool ThreadBufferBase::try_begin_read(Clock milliseconds)
{
	_private->mutex.lock();
	if (!_size && !_private->read.try_wait(&_private->mutex, milliseconds))
	{
		_private->mutex.unlock();
		return false;
	}
	return true;
}

bool ThreadBufferBase::try_begin_write()
{
	_private->mutex.lock();
	if (_size == _capacity)
	{
		_private->mutex.unlock();
		return false;
	}
	return true;
}

bool ThreadBufferBase::try_begin_write(Clock milliseconds)
{
	_private->mutex.lock();
	if (_size == _capacity && !_private->write.try_wait(&_private->mutex, milliseconds))
	{
		_private->mutex.unlock();
		return false;
	}
	return true;
}

} // namespace Yttrium
