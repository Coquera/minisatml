#ifndef Heap_h
#define Heap_h
#include "Vec.h"

//=================================================================================================
// A heap implementation with support for decrease/increase key.

template<class Comp>
class Heap {
    Comp     lt;
    vec<int> heap;     // heap of ints
    vec<int> indices;  // int -> index in heap

    // Index "traversal" functions
    static inline int left  (int i) { return i*2+1; }
    static inline int right (int i) { return (i+1)*2; }
    static inline int parent(int i) { return (i-1) >> 1; }


    inline void percolateUp(int i)
    {
        int x = heap[i];
        while (i != 0 && lt(x, heap[parent(i)])){
            heap[i]          = heap[parent(i)];
            indices[heap[i]] = i;
            i                = parent(i);
        }
        heap   [i] = x;
        indices[x] = i;
    }


    inline void percolateDown(int i)
    {
        int x = heap[i];
        while (left(i) < heap.size()){
            int child = right(i) < heap.size() && lt(heap[right(i)], heap[left(i)]) ? right(i) : left(i);
            if (!lt(heap[child], x)) break;
            heap[i]          = heap[child];
            indices[heap[i]] = i;
            i                = child;
        }
        heap   [i] = x;
        indices[x] = i;
    }


    bool heapProperty (int i) const {
        return i >= heap.size()
            || ((i == 0 || !lt(heap[i], heap[parent(i)])) && heapProperty(left(i)) && heapProperty(right(i))); }

  public:
    Heap(const Comp& c) : lt(c) { }

    int  size      ()          const { return heap.size(); }
    bool empty     ()          const { return heap.size() == 0; }
    bool inHeap    (int n)     const { return n < indices.size() && indices[n] >= 0; }
    int  operator[](int index) const { assert(index < heap.size()); return heap[index]; }

    void decrease  (int n) { assert(inHeap(n)); percolateUp(indices[n]); }

    // RENAME WHEN THE DEPRECATED INCREASE IS REMOVED.
    void increase_ (int n) { assert(inHeap(n)); percolateDown(indices[n]); }


    void insert(int n)
    {
        indices.growTo(n+1, -1);
        assert(!inHeap(n));

        indices[n] = heap.size();
        heap.push(n);
        percolateUp(indices[n]); 
    }


    int  removeMin()
    {
        int x            = heap[0];
        heap[0]          = heap.last();
        indices[heap[0]] = 0;
        indices[x]       = -1;
        heap.pop();
        if (heap.size() > 1) percolateDown(0);
        return x; 
    }

    void clear(bool dealloc = false) 
    { 
        for (int i = 0; i < heap.size(); i++)
            indices[heap[i]] = -1;
#ifdef NDEBUG
        for (int i = 0; i < indices.size(); i++)
            assert(indices[i] == -1);
#endif
        heap.clear(dealloc); 
    }

    // Fool proof variant of insert/decrease/increase
    void update (int n)
    {
        if (!inHeap(n))
            insert(n);
        else {
            percolateUp(indices[n]);
            percolateDown(indices[n]);
        }
    }

    // Delete elements from the heap using a given filter function (-object).
    // *** this could probaly be replaced with a more general "buildHeap(vec<int>&)" method ***
    template <class F>
    void filter(const F& filt) {
        int i,j;
        for (i = j = 0; i < heap.size(); i++)
            if (filt(heap[i])){
                heap[j]          = heap[i];
                indices[heap[i]] = j++;
            }else
                indices[heap[i]] = -1;

        heap.shrink(i - j);
        for (int i = heap.size() / 2 - 1; i >= 0; i--)
            percolateDown(i);

        assert(heapProperty());
    }

    // DEBUG: consistency checking
    bool heapProperty() const {
        return heapProperty(1); }

    // COMPAT: should be removed
    void setBounds (int n) { }
    void increase  (int n) { decrease(n); }
    int  getmin    ()      { return removeMin(); }

    void print () {
      printf ("Print heap\n");
      for (int i = 0 ; i < heap.size(); i++){
        printf ("  %d:%d\n",indices[i],heap[i]);
      }
      printf ("\n");
    }

};
//=================================================================================================
#endif
