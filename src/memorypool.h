#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <stdint.h>
#include "semaphore.h"
#include <list>

namespace mt {

class MemoryPool
{
public:
	MemoryPool(size_t NumBlocks, size_t BlockSize) :
		_lock(1),
		_blockSize(BlockSize)
	{
		for(size_t i = 0; i < NumBlocks; ++i) {
			uint8_t * data = new uint8_t[_blockSize];
			_blocks.push_back(data);
			_free.push_back(data);
		}
	}

	MemoryPool::~MemoryPool()
	{
		for(std::list<uint8_t *>::iterator it = _blocks.begin();
			it != _blocks.end();
			it++)
		{
			delete [] *it;
		}
	}

	uint8_t * Alllocate()
	{
		uint8_t * result = 0;
		_lock.Wait();
		if (!_free.empty()) {
			result = _free.front();
			_free.pop_front();
		}
		_lock.Signal();
		return result;
	}

	void Deallocate(uint8_t * Buffer)
	{
		if (!Buffer) {
			return;
		}
		_lock.Wait();
		_free.push_back(Buffer);
		_lock.Signal();
	}

	size_t blocksize() const { return _blockSize; }

protected:
	std::list<uint8_t *>	_blocks;	//< all allocated blocks
	std::list<uint8_t *>	_free;		//< free blocks
	tasks::Semaphore		_lock;
	size_t					_blockSize;
};

} // namespace mt

#endif
