// doubly linked list
#include <flock/flock.h>
#define Range_Search 1

template <typename K, typename V>
struct LfDlist
{

  struct alignas(64) node : flck::lock
  {
    bool is_end;
    flck::atomic_write_once<bool> removed;
    flck::atomic<node *> prev;
    flck::atomic<node *> next;
    K key;
    V value;
    flck::atomic<int> version_num;
    node(K key, V value, node *next, node *prev)
        : key(key), value(value), is_end(false), removed(false),
          next(next), prev(prev), version_num(0) {};
    node(node *next)
        : is_end(true), removed(false), next(next), prev(nullptr), version_num(0) {}
  };

  flck::memory_pool<node> node_pool;

  auto find_location(node *root, K k)
  {
    node *nxt = (root->next).load();
    while (true)
    {
      node *nxt_nxt = (nxt->next).load(); // prefetch
      if (nxt->is_end || nxt->key >= k)
        break;
      nxt = nxt_nxt;
    }
    return nxt;
  }

  static constexpr int init_delay = 200;
  static constexpr int max_delay = 2000;

  node *insert(node *root, K k, V v)
  {
    return flck::with_epoch([&]
                            {
      int delay = init_delay;
      while (true) {
        int vn = root->version_num.load();
        node* next = root->next.load();
        node* prev = root;
        bool res = prev->try_lock([=] {
          if (root->version_num.load() != vn) return false;
              if (!prev->removed.load() && (prev->next).load() == next) {
                node* new_node = node_pool.new_obj(k, v, next, prev);
                prev->next = new_node;
                next->prev = new_node;
                root->version_num = vn + 1;
                return true;
              } else return false;
            });
        if ( res)//(prev->is_end || prev->key < k) &&m
          return root->next.load(); // YES just return the node
        for (volatile int i = 0; i < delay; i++);
        delay = std::min(2 * delay, max_delay);
      } });
  }

  bool remove(node *root, node *loc)
  { // Remove a node
    return flck::with_epoch([&]
                            {
      int delay = init_delay;
      
      while (true) {
        int vn = root->version_num.load();
        if (loc->is_end) return false;
        node* prev = (loc->prev).load();
        if (prev->removed.load()){
          return false;
        }
        if ((prev->next).load() != loc) {
          return false;
        }
        if (prev->try_lock([=] {
          if (root->version_num.load() != vn) {
            return false;
          }
              if (prev->removed.load() || (prev->next).load() != loc)
                return false;
              return loc->try_lock([=] {
                node* next = (loc->next).load();
                loc->removed = true;
                prev->next = next;
                next->prev = prev;
                node_pool.retire(loc);
                root->version_num = vn + 1;
                return true;
              });
            }))
          return true;
        for (volatile int i = 0; i < delay; i++);
        delay = std::min(2 * delay, max_delay);
      } });
  }

  std::optional<V> find_(node *root, K k)
  {
    node *loc = find_location(root, k);
    if (!loc->is_end && loc->key == k)
      return loc->value;
    else
      return {};
  }

  std::optional<V> find(node *root, K k)
  {
    return flck::with_epoch([&]
                            { return find_(root, k); });
  }

  template <typename AddF>
  void range_(node *root, AddF &add, K start, K end)
  {
    auto nxt = find_location(root, start);
    while (!nxt->is_end && nxt->key <= end)
    {
      add(nxt->key, nxt->value);
      nxt = nxt->next.load();
    }
  }

  node *empty()
  {
    node *tail = node_pool.new_obj(nullptr);
    node *head = node_pool.new_obj(tail);
    tail->prev = head;
    return head;
  }

  node *empty(size_t n) { return empty(); }

  void print(node *p)
  {
    node *ptr = (p->next).load();
    while (!ptr->is_end)
    {
      std::cout << ptr->key << ", ";
      ptr = (ptr->next).load();
    }
    std::cout << std::endl;
  }

  void retire(node *p, bool is_start = true)
  {
    if (is_start || !p->is_end)
      retire(p->next.load(), false);
    node_pool.retire(p);
  }

  long check(node *p)
  {
    node *ptr = (p->next).load();
    if (ptr->is_end)
      return 0;
    K k = ptr->key;
    ptr = (ptr->next).load();
    long i = 1;
    while (!ptr->is_end)
    {
      i++;
      if (ptr->key <= k)
      {
        std::cout << "bad key: " << k << ", " << ptr->key << std::endl;
        abort();
      }
      k = ptr->key;
      ptr = (ptr->next).load();
    }
    return i;
  }

  void clear() { node_pool.clear(); }
  // void reserve(size_t n) { node_pool.reserve(n); }
  // void shuffle(size_t n) { node_pool.shuffle(n); }
  // void stats() { node_pool.stats(); }
};
