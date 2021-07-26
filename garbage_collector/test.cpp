/**
 * Writing a Mark-Sweep Garbage Collector in C++.
 *
 * This is a source code for the Lecture 9 from the
 * Essentials of Garbage Collectors course:
 *
 * http://dmitrysoshnikov.com/courses/essentials-of-garbage-collectors/
 *
 * See full details in:
 *
 * http://dmitrysoshnikov.com/compilers/writing-a-mark-sweep-garbage-collector/
 *
 * by Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 *
 * MIT License (C) 2020
 */

#include <iostream>
#include <memory>

#include <map>
#include <vector>

#include <setjmp.h>

template <typename... T>
void print(const T &... t) {
  (void)std::initializer_list<int>{(std::cout << t << "", 0)...};
  std::cout << "\n";
}

struct Node;
struct Traceable;
struct ObjectHeader;

/**
 * Objects tracing information: allocation pointer to header.
 */
static std::map<Traceable *, ObjectHeader *> traceInfo;

/**
 * Object header.
 */
struct ObjectHeader {
  bool marked;
  size_t size;
};

template <typename T>
void printVector(std::vector<T> const &input) {
  print("\n{");
  for (int i = 0; i < input.size(); i++) {
    print("  ", input.at(i), ", ");
  }
  print("}\n");
}

/**
 * The `Traceable` struct is used as a base class
 * for any object which should be managed by GC.
 */
struct Traceable {
  ObjectHeader *getHeader() { return traceInfo.at(this); }

  static void *operator new(size_t size) {
    // Allocate a block:
    void *object = ::operator new(size);

    // Create an object header for it:
    auto header = new ObjectHeader{.marked = false, .size = size};
    traceInfo.insert(std::make_pair((Traceable *)object, header));

    // Result is the actual block:
    return object;
  }
};

/**
 * Traceable Node structure.
 *
 * Contains the name of the node, and the
 * pointers to left and right sub-nodes,
 * forming a tree.
 */
struct Node : public Traceable {
  char name;

  Node *left;
  Node *right;
};

void dump(const char *label) {
  print("\n------------------------------------------------");
  print(label);

  print("\n{");

  for (const auto &it : traceInfo) {
    auto node = reinterpret_cast<Node *>(it.first);

    print("  [", node->name, "] ", it.first, ": {.marked = ", it.second->marked,
          ", .size = ", it.second->size, "}, ");
  }

  print("}\n");
}

/**
 * Go through object fields, and see if we have any
 * which are recorded in the `traceInfo`.
 */
std::vector<Traceable *> getPointers(Traceable *object) {
  auto p = (uint8_t *)object;
  auto end = (p + object->getHeader()->size);
  std::vector<Traceable *> result;
  while (p < end) {
    auto address = (Traceable *)*(uintptr_t *)p;
    if (traceInfo.count(address) != 0) {
      result.emplace_back(address);
    }
    p++;
  }
  return result;
}

/**
 * Frame pointer.
 */
intptr_t *__rbp;

/**
 * Stack pointer.
 */
intptr_t *__rsp;

/**
 * Main frame stack begin.
 */
intptr_t *__stackBegin;

#define __READ_RBP() __asm__ volatile("movq %%rbp, %0" : "=r"(__rbp))
#define __READ_RSP() __asm__ volatile("movq %%rsp, %0" : "=r"(__rsp))

/**
 * Initializes address of the main frame.
 */
void gcInit() {
  // `main` frame pointer:
  __READ_RBP();
  __stackBegin = (intptr_t *)*__rbp;
}

/**
 * Traverses the stacks to obtain the roots.
 */
std::vector<Traceable *> getRoots() {
  std::vector<Traceable *> result;

  // Some local variables (roots) can be stored in registers.
  // Use `setjmp` to push them all onto the stack.
  jmp_buf jb;
  setjmp(jb);

  __READ_RSP();
  auto rsp = (uint8_t *)__rsp;
  auto top = (uint8_t *)__stackBegin;

  while (rsp < top) {
    auto address = (Traceable *)*(uintptr_t *)rsp;
    if (traceInfo.count(address) != 0) {
      result.emplace_back(address);
    }
    rsp++;
  }

  return result;
}

/**
 * Mark phase.
 */
void mark() {
  auto worklist = getRoots();
  std::cout << worklist.size() << std::endl;
  while (!worklist.empty()) {
    auto o = worklist.back();
    worklist.pop_back();
    auto header = o->getHeader();

    if (!header->marked) {
      header->marked = true;
      for (const auto &p : getPointers(o)) {
        worklist.push_back(p);
      }
    }
  }
}

/**
 * Sweep phase.
 */
void sweep() {
  auto it = traceInfo.cbegin();
  while (it != traceInfo.cend()) {
    if (it->second->marked) {
      it->second->marked = false;
      ++it;
    } else {
      it = traceInfo.erase(it);
      delete it->first;
    }
  }
}

/**
 * Mark-Sweep GC.
 */
void gc() {
  mark();
  dump("After mark:");

  sweep();
  dump("After sweep:");
}

/*
   Graph:
     A        -- Root
    / \
   B   C
      / \
     D   E
        / \
       F   G
            \
             H
*/

Node * createGraph() {
  auto H = new Node;
  H->name = 'H';
  auto G = new Node;
  G->name = 'G';
  auto F = new Node;
  F->name = 'F';
  auto E = new Node;
  E->name = 'E';
  E->left = F;
  E->right = G;
  auto D = new Node;
  D->name = 'D';
  auto C = new Node;
  C->name = 'C';
  C->left = D;
  C->right = E;
  auto B = new Node;
  B->name = 'B';
  auto A = new Node;
  A->name = 'A';
  A->left = B;
  A->right = C;

  return A;
}

int main(int argc, char const *argv[]) {
  gcInit();

  auto A = createGraph();

  // Full alive graph:
  dump("Allocated graph:");

  // Detach the whole right sub-tree:
  A->right = nullptr;

  // Run GC:
  gc();

  return 0;
}

/*
------------------------------------------------
Allocated graph:
{
  [H] 0x7fed26401770: {.marked = 0, .size = 24},
  [G] 0x7fed264017d0: {.marked = 0, .size = 24},
  [F] 0x7fed26401830: {.marked = 0, .size = 24},
  [E] 0x7fed26401890: {.marked = 0, .size = 24},
  [D] 0x7fed264018f0: {.marked = 0, .size = 24},
  [C] 0x7fed26401950: {.marked = 0, .size = 24},
  [B] 0x7fed264019b0: {.marked = 0, .size = 24},
  [A] 0x7fed26401a10: {.marked = 0, .size = 24},
}
------------------------------------------------
After mark:
{
  [H] 0x7fed26401770: {.marked = 0, .size = 24},
  [G] 0x7fed264017d0: {.marked = 0, .size = 24},
  [F] 0x7fed26401830: {.marked = 0, .size = 24},
  [E] 0x7fed26401890: {.marked = 0, .size = 24},
  [D] 0x7fed264018f0: {.marked = 0, .size = 24},
  [C] 0x7fed26401950: {.marked = 0, .size = 24},
  [B] 0x7fed264019b0: {.marked = 1, .size = 24},
  [A] 0x7fed26401a10: {.marked = 1, .size = 24},
}
------------------------------------------------
After sweep:
{
  [B] 0x7fed264019b0: {.marked = 0, .size = 24},
  [A] 0x7fed26401a10: {.marked = 0, .size = 24},
}
*/
