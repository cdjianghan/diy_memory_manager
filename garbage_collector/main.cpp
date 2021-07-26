#include <iostream>
#include <map>
#include <vector>
#include <csetjmp>
#define __READ_RBP() __asm__ volatile("movq %%rbp, %0" : "=r"(__rbp))
#define __READ_RSP() __asm__ volatile("movq %%rsp, %0" : "=r"(__rsp))

class Traceable;

struct ObjectHeader {
  bool marked;
  size_t size;
};

std::map<Traceable *, ObjectHeader *> traceInfo;
void dump(const char *label);
struct Traceable {
  ObjectHeader * getHeader() {return traceInfo.at(this);}

  static void * operator new(size_t size) {
    void * object = ::operator new(size);

    auto header = new ObjectHeader;
    header->size = size;
    header->marked = false;

    traceInfo.insert(std::make_pair((Traceable *)object, header));
    return object;
  }
};

intptr_t * __rbp;
intptr_t * __rsp;
intptr_t * __stackBegin;

void gcInit() {
  __READ_RBP();
  __stackBegin = (intptr_t *)__rbp;
}
std::vector<Traceable *> getRoots() {
  std::vector<Traceable *> roots;

  jmp_buf jb;
  setjmp(jb);

  __READ_RSP();
  auto rsp = (uint8_t *)__rsp;
  auto top = (uint8_t *)__stackBegin;

  while (rsp < top) {
    auto address = (Traceable *)*(uintptr_t *)rsp;
    if (traceInfo.count(address) != 0) {
      roots.emplace_back(address);
    }
    rsp ++;
  }
  return roots;

}

std::vector<Traceable *> getPointers(Traceable * object) {
  auto p = (uint8_t *)object;
  auto end = (p + object->getHeader()->size);
  std::vector<Traceable *> result;
  while (p < end) {
    auto address = (Traceable *)*(uintptr_t *)p;
    if (traceInfo.count(address) != 0) {
      result.push_back(address);
    }
    p ++;
  }
  return result;
}

void mark() {
  auto worklist = getRoots();
  std::cout << "root size is " << worklist.size() << std::endl;
  while (worklist.size()) {
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

void sweep() {
  auto it = traceInfo.cbegin();
  while (it != traceInfo.cend()) {
    if (it->second->marked) {
      it->second->marked = false;
      it ++;
    } else {
      auto last = it;
      it = traceInfo.erase(it);
      delete last->first;
      std::cout << "find garbage" << std::endl;
    }

  }
}


void gc() {
  mark();
  dump("After mark:");
  sweep();
  dump("After sweep:");
}
struct Node : public Traceable {
  char name;

  Node * left;
  Node * right;
};

template <typename... T>
void print(const T &... t) {
  (void)std::initializer_list<int>{(std::cout << t << "", 0)...};
  std::cout << "\n";
}
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


Node * createGraph() {
  auto H = new Node;
  H->name = 'H';
  auto G = new Node;
  G->name = 'G';
  G->left = H;
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
int main() {
  gcInit();
  std::cout << __stackBegin << std::endl;
  auto node = createGraph();
  std::cout << (char *)node << std::endl;
  dump("Allocated graph:");
  node->right = nullptr;
  //node->left = nullptr;
  gc();
  return 0;
}

