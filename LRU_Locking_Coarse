#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <bits/stdc++.h>
#include <mutex>

struct Node {
    int key, value;
    Node *next, *prev;
    std::mutex lock;

    Node(int k, int v) {
        key = k;
        value = v;
        next = nullptr;
        prev = nullptr;
    }
};

// LRU Cache class
class LRUCache
{
  public:
  
    // Constructor to initialize the cache with a given capacity
    int capacity;
    std::unordered_map<int, Node *> cacheMap;
    Node *head, *tail;
    std::mutex lock;

    LRUCache(int capacity) {
        this->capacity = capacity;
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    // Function to get the value for a given key
    int get(int key) {
        
        // Acquire lock
        std::scoped_lock guard(lock);
      
        if (cacheMap.find(key) == cacheMap.end())
            return -1;

        Node* node = cacheMap[key];
        remove(node);
        add(node);  // Move to front (MRU)
        return node->value;
    }

    // Function to put a key-value pair into the cache
    void put(int key, int value) {

        std::scoped_lock guard(lock);

        if (cacheMap.find(key) != cacheMap.end()) {
            Node* oldNode = cacheMap[key];
            remove(oldNode);
            delete oldNode;
        }

        Node* node = new Node(key, value);
        add(node);
        cacheMap[key] = node;

        if (cacheMap.size() > capacity) {
            Node* lru = tail->prev;
            remove(lru);
            cacheMap.erase(lru->key);
            delete lru;
        }
    }

    // Add a node right after the head 
    void add(Node *node) {

        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    // Remove a node from the list
    void remove(Node *node) {

        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
};

#endif
