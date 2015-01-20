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

typedef unsigned int py_t; // priority type
typedef unsigned short int pos_t; // position type, must be integer

template <class item_t>
struct PriorityItem {
  py_t priority;
  item_t item;
  pos_t pos; // position in heap
};

// non-stable priority queue with static dimension
template <class item_t>
class PriorityQueue {
  pos_t maxSize; //TODO access with a read-only reference is safer? // const pos_t& m = maxSize;
  pos_t size;
  PriorityItem<item_t>** heap; // array of pointer
  void swap(PriorityItem<item_t>* &, PriorityItem<item_t>* &);
  void upRestore(pos_t);
  void downRestore(pos_t);
public:
  PriorityQueue(pos_t);
  ~PriorityQueue();
  bool isEmpty();
  bool isFull();
  item_t min(); // may throw an exception
  //TODO is the item was deleted, the pointer is unsafe
  const PriorityItem<item_t>* emplace(py_t, item_t);
  void decrease(py_t, const PriorityItem<item_t>*);
  void increase(py_t, const PriorityItem<item_t>*); 
  void deleteMin();
};

/**
 * Init the priority queue.
 *
 * @param maxSize The maximum size (number of items) you want to.
 */
template <class item_t>
PriorityQueue<item_t>::PriorityQueue(pos_t maxSize) {
  this->maxSize = maxSize;
  size = 0;
  heap = new PriorityItem<item_t>*[maxSize] {nullptr}; //TODO legal/usefull init?
}

template <class item_t>
PriorityQueue<item_t>::~PriorityQueue() {
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
template <class item_t>
bool PriorityQueue<item_t>::isEmpty() {
  return (size == 0);
}

/**
 * Check if the priority queue id full.
 *
 * @return True only if the current size is the maximum size.
 */
template <class item_t>
bool PriorityQueue<item_t>::isFull() {
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
template <class item_t>
item_t PriorityQueue<item_t>::min() {
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
template <class item_t>
void PriorityQueue<item_t>::swap(PriorityItem<item_t>* &a, PriorityItem<item_t>* &b) {
  pos_t a_pos = a->pos, b_pos = b->pos; // positions must be preserved (and updated)
  PriorityItem<item_t>* tmp = a;
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
template <class item_t>
void PriorityQueue<item_t>::upRestore(pos_t i) {
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
template <class item_t>
void PriorityQueue<item_t>::downRestore(pos_t i) {
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
template <class item_t>
const PriorityItem<item_t>* PriorityQueue<item_t>::emplace(py_t priority, item_t item) {
  if (size >= maxSize)
    return nullptr; // if the queue if full, exit
  PriorityItem<item_t>* newPriorityItem = new PriorityItem<item_t>{priority, item, size};
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
template <class item_t>
void PriorityQueue<item_t>::decrease(py_t newPriority, const PriorityItem<item_t>* pi) {
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
template <class item_t>
void PriorityQueue<item_t>::increase(py_t newPriority, const PriorityItem<item_t>* pi) {
  if (newPriority <= pi->priority)
    return; // if the newPriority isn't greater then the current priority, nothing to do
  heap[pi->pos]->priority = newPriority;
  downRestore(pi->pos);
}

/**
 * Function for delete the minimum priority item (and update the queue).
 */
template <class item_t>
void PriorityQueue<item_t>::deleteMin() {
  if (size <= 0)
    return; // nothing to delete
  
  if (size > 1)
    swap(heap[0], heap[size-1]);
  delete heap[size-1]; // de-allocate
  size--;
  
  if (size > 1)
    downRestore(0); // restore the heap
}
