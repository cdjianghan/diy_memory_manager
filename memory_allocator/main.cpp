#include <cstdio>
#include <zconf.h>
#include <utility>
#include <assert.h>
#include <stdint.h>
#include <list>
#include "sbrk.cpp"

using word_t = intptr_t;

struct Block {
  // Object header

  /*
   * Block size.
   */
  size_t size;

  /*
   * Whether this block is used.
   */
  bool used;

  /*
   * Next block in the list.
   */
  Block * next;

  // User data
  word_t data[1];

};

/*
 * function declaration
 */
Block * listAllocate(Block * block, size_t size);


/*
 * Heap start.
 */
static Block * heapStart = nullptr;

/*
 * Current top in the heap.
 */
static auto top = heapStart;

/*
 * Aligns the size by the machine word
 */
inline size_t align(size_t n) {
  return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

inline size_t allocSize(size_t size) {
  return size + sizeof(Block) - sizeof(std::declval<Block>().data);
}

/*
 * Request (maps) memory from OS
 */
Block * requestFromOS(size_t size) {
  auto block = (Block * )_sbrk(0);

  if (_sbrk(allocSize(size)) == (void * )-1) {
    return nullptr;
  }

  return block;
}

enum class SearchMode {
  FirstFit,
  NextFit,
  BestFit,
  FreeList,
  SegregatedList,
};

static Block * searchStart = heapStart;

static auto searchMode = SearchMode::FreeList;

void resetHeap() {
  if (heapStart == nullptr) {
    return;
  }

  brk(heapStart);

  heapStart = nullptr;
  top = nullptr;
  searchStart = nullptr;
}

void init(SearchMode mode) {
  searchMode = mode;
  resetHeap();
}


Block * firstFit(size_t size) {
  auto block = heapStart;

  while (block != nullptr) {
    if (block->used || block->size < size) {
      block = block->next;
      continue;
    }

    return block;
  }

  return nullptr;
}

Block * nextFit(size_t size) {
  auto block = searchStart;
  auto end = searchStart;

  while (block != nullptr) {
    if (block->used || block->size < size) {
      block = block->next;
    }

    // if end -> return the begin
    if (block == nullptr) {
      block = heapStart;
      continue;
    }
    // if scan a circle, don't find a target block, break
    if (block == end) break;
    searchStart = block->next;
    if (searchStart == nullptr) searchStart = heapStart;

    return block;
  }
  return nullptr;
}

Block * bestFit(size_t size) {
  auto block = heapStart;
  Block * fit = nullptr;

  while (block != nullptr) {
    if (block->used || block->size < size) {
      block = block->next;
    }

    if (fit == nullptr) fit = block;
    else if (fit->next > block->next) fit = block;

    block = block->next;
  }
  return fit;
}
/*
 * explict Freelist
 */
static std::list<Block *> free_list;

Block * freeList(size_t size) {
  for (const auto &block : free_list) {
    if (block->size < size) {
      continue;
    }
    free_list.remove(block);
    return listAllocate(block, size);
  }
  return nullptr;
}

/*
 * segregatedList
 */
Block * segregatedLists[] = {
    nullptr,   //   8
    nullptr,   //  16
    nullptr,   //  32
    nullptr,   //  64
    nullptr,   // 128
    nullptr,   // any > 128
};

inline int getBucket(size_t size) {
  return size / sizeof(word_t) - 1;
}

Block * segregatedFit(size_t size) {
  auto bucket = getBucket(size);
  auto originalHeapStart = heapStart;

  heapStart = segregatedLists[bucket];
  auto block = firstFit(size);

  heapStart = originalHeapStart;
  return block;
}


Block * findBlock(size_t size) {
  switch (searchMode) {
    case SearchMode::FirstFit:
      return firstFit(size);
    case SearchMode::NextFit:
      return nextFit(size);
    case SearchMode::BestFit:
      return bestFit(size);
    case SearchMode::FreeList:
      return freeList(size);
    case SearchMode::SegregatedList:
      return segregatedFit(size);
  }
}

// block split
/*
 * Split the block to two blocks
 */
Block * split(Block * block, size_t size) {
  Block * p = block + (size + sizeof(block) - sizeof(word_t));
  p->size = block->size - size - (sizeof(block) * 2 - 2 * sizeof(word_t));
  p->used = false;
  p->next = block->next;
  block->next = p;
  return block;
}

/*
 * Whether this block can be split
 */
inline bool canSplit(Block * block, size_t size) {
  if (block->size > size + 2 * sizeof(Block) - 2 * sizeof(word_t)) {
    return true;
  }
  return false;
}

/*
 * Allocates a block from the list, splitting if needed
 */

Block * listAllocate(Block * block, size_t size) {
  if (canSplit(block, size)) {
    block = split(block, size);
  }

  block->used = true;
  block->size = size;

  return block;
}

/*
 * Whether we should merge this block
 */
bool canCoalesce(Block * block) {
  return block->next && !block->next->used;
}

/*
 * Coalesces two adjacent block
 */
Block * coalesce(Block * block) {
  Block * p = block->next;
  block->next = block->next->next;
  block->size = block->size + p->size;
  printf("two block can merge, size is %d\n", block->size);
  return block;
}

/*
 * Allocates a block of memory
 */
word_t * alloc(size_t size) {
  size = align(size);

  // search for a free block in the list
  if (auto block = findBlock(size)) {
    return block->data;
  }

  auto block = requestFromOS(size);

  block->size = size;
  block->used = true;

  // if can't find a free block, we call memory from OS

  if (heapStart == nullptr) {
    heapStart = block;
  }

  if (top != nullptr) {
    top->next = block;
  }

  top = block;

  return block->data;
}

Block * getHeader(word_t * data) {
  return (Block * )((char * )data + sizeof(std::declval<Block>().data) - sizeof(Block));
}

void free(word_t * data) {
  auto block = getHeader(data);
  if (canCoalesce(block)) {
    block = coalesce(block);
  }
  block->used = false;
  free_list.push_back(block);
}

int main() {
  arena = mmap(0, arenaSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  // --------------------------------------
  // Test case : split and merge test
  //

  // test case 1 merge test
  auto p1 = alloc(12);
  auto p2 = alloc(12);
  free(p2);
  free(p1);
  printf("%p\n", p1);
  printf("%p\n", p2);
  auto p3 = alloc(24);
  printf("%p\n", p3);
  assert(p1 == p3);
  // test case2 split test
  free(p3);
  auto p4 = alloc(4);

  printf("%d\n", sizeof(word_t));
  assert(p1 == p3 && getHeader(p4)->size == 8 );


  puts("\nAll assertions passed!\n");

}
