/**
 * @file priorityQueue.cpp
 * @author MParolari
 *
 * @section DESCRIPTION
 * Priority queue implemented with a binary heap.
 *
 * @section LICENSE
 * GNU GPLv2 - see LICENSE file
 */

typedef unsigned int            py_t;  // priority type
typedef unsigned short int      pos_t; // position type, must be integer

template <class T>
struct PriorityItem {
  py_t priority;
  T item;
  pos_t pos; // position in heap
};

// non-stable priority queue with static dimension
template <class T>
class BinHeapPQ {
  pos_t maxSize;
  pos_t size;
  PriorityItem<T>** heap; // array of pointer
  void swap(PriorityItem<T>* &, PriorityItem<T>* &);
  void upRestore(pos_t);
  void downRestore(pos_t);
public:
  BinHeapPQ(pos_t);
  ~BinHeapPQ();
  bool isEmpty();
  bool isFull();
  T min(); // throw an exception if heap is empty
  //TODO is the item was deleted, the pointer is unsafe
  const PriorityItem<T>* emplace(py_t, T);
  void decrease(py_t, const PriorityItem<T>*);
  void increase(py_t, const PriorityItem<T>*); 
  void deleteMin();
};

/**
 * Init the priority queue.
 *
 * @param maxSize The maximum size (number of items) you want to.
 */
template <class T>
BinHeapPQ<T>::BinHeapPQ(pos_t maxSize) {
  this->maxSize = maxSize;
  size = 0;
  heap = new PriorityItem<T>*[maxSize] {nullptr}; //TODO legal/usefull init?
}

template <class T>
BinHeapPQ<T>::~BinHeapPQ() {
  // de-allocate the dinamic heap
  for (pos_t i=0; i < size; i++)
    delete heap[i];
  delete[] heap;
}

/**
 * Check if the priority queue is empty.
 *
 * @return True only if the current size is zero.
 */
template <class T>
bool BinHeapPQ<T>::isEmpty() {
  return (size == 0);
}

/**
 * Check if the priority queue id full.
 *
 * @return True only if the current size is the maximum size.
 */
template <class T>
bool BinHeapPQ<T>::isFull() {
  return (size == maxSize);
}

/**
 * Function for get the minimum PriorityItem, aka the item associated with
 * the minimum priority.
 *
 * Note that this won't delete the minimum item; if the queue is empty,
 * it will raise an exception.
 *
 * @return The (copy) value associated.
 */
template <class T>
T BinHeapPQ<T>::min() {
  if (size > 0)
    return heap[0]->item;
  throw("Empty priority queue!");
}

/** 
 * Swap two PriorityItems in heap, updating their position.
 *
 * @param a The first.
 * @param b The second.
 */
template <class T>
void BinHeapPQ<T>::swap(PriorityItem<T>* &a, PriorityItem<T>* &b) {
  pos_t a_pos = a->pos, b_pos = b->pos; // positions must be preserved (and updated)
  PriorityItem<T>* tmp = a;
  a = b;
  b = tmp;
  a->pos = a_pos;
  b->pos = b_pos;
}

/**
 * Bottom-up restore of the heap.
 *
 * If the PriorityItem in position (i) has a priority lesser than his ancestors,
 * due to different reasons (ie, his priority decreased, or it's a new item),
 * this function take the item in his correct position, fixing the heap.
 * Working only if the rest of heap is a valid/legal heap.
 * Because this function restore the heap from the element in (i) position
 * to the root (bottom-up), is complexity is O(h), where (h) is the height of heap;
 * beacuse we work with a binary tree, this function is O(log(n)).
 *
 * @param i The position of item to check/restore.
 */
template <class T>
void BinHeapPQ<T>::upRestore(pos_t i) {
  while (i > 0 && heap[i]->priority < heap[i/2]->priority) {
    swap(heap[i], heap[i/2]);
    i /= 2;
  }
}

/**
 * Top-down restore of the heap.
 *
 * If the PriorityItem in position (i) has a priority greater than his descendants,
 * due to different reasons (ie, his priority increased, or it was deleted),
 * this function look at his direct children and if one of them is lesser than it,
 * we swap them, and check, "recursively", the new position.
 * Working only if the rest of heap (aka the two sub-heap of children) is a legal heap.
 * Because this function restore the heap from the element in (i) position
 * to a "leaf" (top-down), is complexity is O(h), where (h) is the height of heap;
 * beacuse we work with a binary tree, this function is O(log(n)).
 *
 * @param i The position of item to check/restore.
 */
template <class T>
void BinHeapPQ<T>::downRestore(pos_t i) {
  while (i >= 0) { // condition for entering the loop
    pos_t min = i; // the position of the minimum item between (i) and his children
    if (2*i    < size)
      if (heap[2*i]->priority    < heap[min]->priority) min = 2*i;
    if (2*i +1 < size)
      if (heap[2*i +1]->priority < heap[min]->priority) min = 2*i +1;
    
    if (min != i) { // if (i) is not the minimum item
      swap(heap[i], heap[min]);
      i = min; // update the position to check
    }
    else // nothing to do, heap is restored, exit
      break;
  }
}

/**
 * Function for emplacing a new item.
 *
 * @param priority The priority of the new item.
 * @param item The value of the new item.
 * @return A read-only pointer, for monitoring the item created.
 */
template <class T>
const PriorityItem<T>* BinHeapPQ<T>::emplace(py_t priority, T item) {
  if (size >= maxSize)
    return nullptr; // if the queue if full, exit
  PriorityItem<T>* newPriorityItem = new PriorityItem<T>{priority, item, size};
  heap[size] = newPriorityItem; // temporary position
  size++;
  upRestore(size-1);
  return newPriorityItem; // return control pointer
}

/**
 * Function for decrease the priority of an item in the queue.
 *
 * @param newPriority The new priority value of the item.
 * @param pi The read-only pointer of the item you want to modify.
 */
template <class T>
void BinHeapPQ<T>::decrease(py_t newPriority, const PriorityItem<T>* pi) {
  if (newPriority >= pi->priority)
    return; // if the newPriority isn't lesser then the current priority, nothing to do
  heap[pi->pos]->priority = newPriority;
  upRestore(pi->pos);
}

/**
 * Function for increase the priority of an item in the queue.
 *
 * @param newPriority The new priority value of the item.
 * @param pi The read-only pointer of the item you want to modify.
 */
template <class T>
void BinHeapPQ<T>::increase(py_t newPriority, const PriorityItem<T>* pi) {
  if (newPriority <= pi->priority)
    return; // if the newPriority isn't greater then the current priority, nothing to do
  heap[pi->pos]->priority = newPriority;
  downRestore(pi->pos);
}

/**
 * Function for delete the minimum priority item (and update the queue).
 */
template <class T>
void BinHeapPQ<T>::deleteMin() {
  if (size <= 0)
    return; // nothing to delete
  
  if (size > 1)
    swap(heap[0], heap[size-1]);
  delete heap[size-1]; // de-allocate
  size--;
  
  if (size > 1)
    downRestore(0); // restore the heap
}
