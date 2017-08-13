/*
fixed-block size memory pool, doubly linking with index in an array of object.
array (contigious in memory) is declared before pool initiliazation,
hence array is store out side the Pool.
no BLOCK object needed, but drawback shown when returning pointers to array of block.
no reset Pool, outPool allocation for larger item omitted,
no derivative considered,
all codes  are within this single file, brief test at the end.
*/
#include<iostream>
using namespace std;

class FixMemPool {
 public:
  // initial pool size (MB), and the size of each memory uint in the memory pool (KB)
  FixMemPool(size_t init_size_mb = 256, size_t uint_size_kb = 1);
  //when Malloc, only size is needed, a pointer is returned.
  void* Malloc(const size_t size);
  void Free(void* ptr);

  ~FixMemPool(){free(pMemPool);}//descturctor, shall check if no more allocated block before execution.

 private:
  // each structure refers to a block in the memory pool,but nore stored in pool.
  // the structure consists of 2 index for book keeping for prev and next uint
  // in an array of object, structure named listNode.
  struct _Uint {
   int prev, next; // members pPrev and pNext are int.
   void* pBlk; //member pBlk are pointer to a block of the memory pool.
  };
  //below declares
  // pointer to the memory pool
  void* pMemPool;

  // head pointer to allocated memory uint
  struct _Uint* pAllocatedMemUint;
  // head pointer to free memory uint
  struct _Uint* pFreeMemUint;

  // the size of each memory uint with/out the meta data of the uint
  size_t memUintSize, memUintSizeNoMeta;
  // the number of memory uints in the pool
  size_t numUints;
  // the number of allocated uints which are resided in the memory pool
  size_t numAllocatedUintsInPool;
  // the number of allocated uints including the ones resided outside the memory pool
  size_t numAllocatedUints;
  //convert btw block point pBlk and array index.
  char* AddrFromIndex(int i) const;
  int IndexFromAddr(const char* p) const;
  //convert btw array idx and pFree/pAllocated head.
  char* AddrFromIndexArray(int i) const;
  int IndexFromAddrArray(const char* p) const;
}

FixMemPool::FixMemPool(size_t init_size_mb = 256, size_t uint_size_kb = 1)
{
  //init
  pMemPool = NULL ;
  pAllocatedMemUint = pFreeMemUint = NULL;
  memUintSize = memUintSizeNoMeta = 0;
  numUints = numAllocatedUintsInPool = numAllocatedUints = 0;

  size_t perMB=1u<<20;
  size_t perKB=1u<<10;
  size_t poolSize=init_size_mb*perMB;
  pMemPool=malloc(poolSize);

  bool memAligned = poolSize % memUintSize == 0; //check if mem aligned
  numUints = memAligned ? (poolSize / memUintSize) : (poolSize / memUintSize + 1);
  poolSize = memUintSize * numUints;//reset poolSize

  int idx;
  struct _Uint listNode[numUints]; //declare an array of objects, standalone memory.
  //contrast to below method with new, which gives a _Uint* rather than _Uint.
  // ptr= new struct _Uint[numUints];

  //initiliazation,
  for (idx=0;idx<numUints;idx++){
    //simply linking adjacent uints during init.
    listNode[idx].prev=idx-1; //listNode[0].prev=-1,-1 in index equavalent to NULL in pointer.
    listNode[idx].next=idx+1;
    listNode[idx].pBlk=NULL;
    }
  listNode[numUints-1].next=-1; //rewrite end of the array
  pFreeMemUint=(_Uint*)AddrFromIndexArray[numUints-1]; //define free head as largest index, same for allocated head.
}

//temporarily use all pointer as char* TODO to counter verify the conversion pointer type in all 4 the function body.
char* AddrFromIndex(int i) const
{
  return (char*)pMemPool + ( i * memUintSize);
}

int IndexFromAddr(const char* p) const
{
  return (((size_t)(p - (char*)pMemPool)) / memUintSize);
}

char* AddrFromIndexArray(int i) const
{
  return (char*)listNode + (i * sizeof(struct _Uint));
}

int IndexFromAddrArray(const char* p) const
{
  return (((size_t)(p - listNode)) / sizeof(struct _Uint));
}

void* FixMemPool::Malloc(const size_t size){
  //allocate out of Pool if larger omitted.
  numAllocatedUintsInPool++;
  int idx=IndexFromAddrArray((char*)pFreeMemUint); //here idx refers to current Uint.
  pFreeMemUint=(_Uint*)AddrFromIndexArray(listNode[idx].prev);
  listNode[listNode[idx].prev].next=-1;
  if (pAllocatedMemUint==NULL){
   listNode[idx].prev=-1; //tail
  }else{
   listNode[idx].prev=IndexFromAddrArray((char*)pAllocatedMemUint);
  }
  //listNode[idx].next had been -1;
  listNode[IndexFromAddrArray((char*)pAllocatedMemUint)].next=idx;
  pAllocatedMemUint=(_Uint*)AddrFromIndexArray(idx);
  listNode[idx].pBlk=(void*)AddrFromIndex(idx);//map from array to block of memory pool.
  return listNode[idx].pBlk;
}

void FixMemPool::Free(void *ptr){
  numAllocatedUintsInPool--;
  int idx = IndexFromAddr((char*)ptr);

  if(listNode[idx].next==-1){ // if allocated head, shift head.
   pAllocatedMemUint=(_Uint*)AddrFromIndexArray(listNode[idx].prev);
  }else if(listNode[idx].prev==-1){ //if allocated tail
   listNode[listNode[idx].next].prev=-1;
  }else{ //if in middle
   listNode[listNode[idx].prev].next=listNode[listNode[idx].next].prev;
   listNode[listNode[idx].next].prev=listNode[listNode[idx].prev].next;
  }
  listNode[IndexFromAddrArray((char*)pFreeMemUint)].next=idx;
  listNode[idx].prev=IndexFromAddrArray((char*)pFreeMemUint);
  listNode[idx].next=-1;
  pFreeMemUint=(_Uint*)AddrFromIndexArray(listNode[idx]);

  listNode[idx].pBlk = NULL; //to counter verify what else need to be cleaned up.
}

int main(){
  //simple test if functional.
  FixMemPool pool(1,1);
  const size_t dataSize = 1000;//any size <= 1024.
  for(int i = 0; i < 1024; i++) {
   void* ptr = pool.Malloc(dataSize); //TODO to come up with ptr storing, and returning pointer type.
   if (i%2){
    pool.Free(ptr);
   }
  }
  //TODO to include usage counter functions.
  return 0;
}
