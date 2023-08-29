
#include "StaticBuffer.h"
// the declarations for this class can be found at "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];

StaticBuffer::StaticBuffer() {

  // initialise all blocks as free
 for (int blockIndex = 0, blockAllocMapSlot = 0; blockIndex < 4; blockIndex++) {
		unsigned char buffer [BLOCK_SIZE];
		Disk::readBlock(buffer, blockIndex);

		for (int slot = 0; slot < BLOCK_SIZE; slot++, blockAllocMapSlot++)
			StaticBuffer::blockAllocMap[blockAllocMapSlot] = buffer[slot];
	}
  for (/*bufferIndex = 0 to BUFFER_CAPACITY-1*/int bufferIndex = 0;bufferIndex<BUFFER_CAPACITY;bufferIndex++) {
    metainfo[bufferIndex].free = true;
    metainfo[bufferIndex].dirty = false;
    metainfo[bufferIndex].timeStamp = -1;
    metainfo[bufferIndex].blockNum = -1;
  }
}

/*
At this stage, we are not writing back from the buffer to the disk since we are
not modifying the buffer. So, we will define an empty destructor for now. In
subsequent stages, we will implement the write-back functionality here.
*/

/* Get the buffer index where a particular block is stored
   or E_BLOCKNOTINBUFFER otherwise
*/
int StaticBuffer::getBufferNum(int blockNum) {
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  // and return E_OUTOFBOUND if not valid.
  if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }
  // find and return the bufferIndex which corresponds to blockNum (check metainfo)
  for(int i = 0;i<BUFFER_CAPACITY;i++)
  {
    if(metainfo[i].free == false && metainfo[i].blockNum == blockNum)return i;
  }

  // if block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}

int StaticBuffer::getFreeBuffer(int blockNum){
    // // Check if blockNum is valid (non zero and less than DISK_BLOCKS)
    // // and return E_OUTOFBOUND if not valid.
    // if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    //   return E_OUTOFBOUND;
    // }
    // // increase the timeStamp in metaInfo of all occupied buffers.
    // for(int i = 0;i<BUFFER_CAPACITY;i++)
    // {
    //   metainfo[i].timeStamp++;
    // }
    // // let bufferNum be used to store the buffer number of the free/freed buffer.
    // int bufferNum = -1;

    // // iterate through metainfo and check if there is any buffer free
    // int largetime = -1;
    // int time = -1e9;
    // for(int i = 0;i<BUFFER_CAPACITY;i++)
    // {
    //   if(metainfo[i].free)
    //   {
    //     bufferNum = i;
    //     break;
    //   }
    //   if(metainfo[i].timeStamp>time)
    //   {
    //     time = metainfo[i].timeStamp;
    //     largetime = i;
    //   }
    // }
    // if(bufferNum == -1)
    // {
    //   if(metainfo[largetime].dirty)Disk::writeBlock(blocks[largetime],blockNum);
    //   bufferNum = largetime;
    // }
    // metainfo[bufferNum].free = false;
    // metainfo[bufferNum].blockNum = blockNum;
    // metainfo[bufferNum].dirty = false;
    // metainfo[bufferNum].timeStamp = 0;
    // return bufferNum;
    if (blockNum < 0 || blockNum >= DISK_BLOCKS) return E_OUTOFBOUND;
	for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++)
		metainfo[bufferIndex].timeStamp++;

	int allocatedBuffer = 0;

	// TODO: iterate through all the blocks in the StaticBuffer
	// TODO: find the first free block in the buffer (check metainfo)
	// TODO: assign allocatedBuffer = index of the free block
	for (; allocatedBuffer < BUFFER_CAPACITY; allocatedBuffer++)
		if (metainfo[allocatedBuffer].free) break;

	if (allocatedBuffer == BUFFER_CAPACITY) {
		int lastTimestamp = -1, bufferNum = -1;
		for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++) {
			if (metainfo[bufferIndex].timeStamp > lastTimestamp) {
				lastTimestamp = metainfo[bufferIndex].timeStamp;
				bufferNum = bufferIndex;
			}
		}

		allocatedBuffer = bufferNum;
		if (metainfo[allocatedBuffer].dirty == true) {
			Disk::writeBlock(StaticBuffer::blocks[allocatedBuffer], 
								metainfo[allocatedBuffer].blockNum);
		}

		// return FAILURE;
	}

	metainfo[allocatedBuffer].free = false, 
	metainfo[allocatedBuffer].dirty = false,
	metainfo[allocatedBuffer].timeStamp = 0, 
	metainfo[allocatedBuffer].blockNum = blockNum;

	return allocatedBuffer;
    // if a free buffer is available, set bufferNum = index of that free buffer.

    // if a free buffer is not available,
    //     find the buffer with the largest timestamp
    //     IF IT IS DIRTY, write back to the disk using Disk::writeBlock()
    //     set bufferNum = index of this buffer

    // update the metaInfo entry corresponding to bufferNum with
    // free:false, dirty:false, blockNum:the input block number, timeStamp:0.

    // return the bufferNum.
}

int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().
    int bufferNum = getBufferNum(blockNum);
    if(bufferNum == E_BLOCKNOTINBUFFER)return E_BLOCKNOTINBUFFER;
    if(bufferNum == E_OUTOFBOUND)return E_OUTOFBOUND;
    else
    {
      metainfo[bufferNum].dirty = true;
    }
    return SUCCESS;

    // if block is not present in the buffer (bufferNum = E_BLOCKNOTINBUFFER)
    //     return E_BLOCKNOTINBUFFER

    // if blockNum is out of bound (bufferNum = E_OUTOFBOUND)
    //     return E_OUTOFBOUND

    // else
    //     (the bufferNum is valid)
    //     set the dirty bit of that buffer to true in metainfo

    // return SUCCESS
}

// write back all modified blocks on system exit
StaticBuffer::~StaticBuffer() {
  /*iterate through all the buffer blocks,
    write back blocks with metainfo as free=false,dirty=true
    using Disk::writeBlock()
    */
  for (int blockIndex = 0, blockAllocMapSlot = 0; blockIndex < 4; blockIndex++) {
		unsigned char buffer [BLOCK_SIZE];

		for (int slot = 0; slot < BLOCK_SIZE; slot++, blockAllocMapSlot++) 
			buffer[slot] = blockAllocMap[blockAllocMapSlot];

		Disk::writeBlock(buffer, blockIndex);
	}
  for(int i = 0;i<BUFFER_CAPACITY;i++)
  {
    if((metainfo[i].free == false) && (metainfo[i].dirty == true))
    { 
      Disk::writeBlock(blocks[i],metainfo[i].blockNum);
    }
  }
}
