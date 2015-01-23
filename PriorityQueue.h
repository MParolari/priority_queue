/**
 * @file PriorityQueue.h
 * @author MParolari	<https://github.com/MParolari>
 *
 * @section DESCRIPTION
 * A priority queue model/interface.
 * https://github.com/MParolari/priority_queue
 *
 * @section LICENSE
 * GNU GPLv2 - see LICENSE file
 */

/**
 * Type of priorities used, you can change it if you want a different range of values.
 */
typedef unsigned int		py_t;
/**
 * Type used for operation and iteration of array.
 * It must be unsigned integer; you can change it if you want store more element in the queue.
 */
typedef unsigned short int	pos_t;

/**
 * A "Priority Item" is an object stored in the priority queue.
 *
 * You haven't to create or manipulate them. It associates the value/item
 * stored with its priority and its position.
 * A "reference" (aka a const pointer) to a priority item is returned
 * when you emplace an item/value in the queue; you can use this pointer
 * for monitoring or referring to the priority item.
 */
template <class T>
struct PriorityItem {
  py_t	priority; /**< Priority of this item.    */
  T	item;     /**< The value of item stored. */
  pos_t pos;      /**< Position in the queue.    */
};

template <class T>
class PriorityQueue {
public:
  virtual bool isEmpty() = 0;
  virtual T min() = 0;
  virtual const PriorityItem<T>* emplace(py_t, T) = 0;
  virtual void decrease(py_t, const PriorityItem<T>*) = 0;
  virtual void increase(py_t, const PriorityItem<T>*) = 0; 
  virtual void deleteMin() = 0;
};
