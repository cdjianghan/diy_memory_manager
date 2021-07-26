//
// Created by 马子杰 on 2021/2/25.
//

#include <stdint.h>
#include <sys/mman.h>

static void * arena = nullptr;

static char * _brk = nullptr;

// brk mmap

static size_t arenaSize = 4 * 1024 * 1024;
void * _sbrk(intptr_t increment) {
  _brk = (char *)mmap(0, arenaSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  arena = _brk + arenaSize;

  if (increment == 0) return _brk;
  if (increment + _brk > arena ) {
    return (void *)-1;
  } else {
    _brk = _brk + increment;
  }
}



