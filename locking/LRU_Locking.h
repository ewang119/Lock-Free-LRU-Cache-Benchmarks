// C++ program to implement LRU Least Recently Used)
#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <bits/stdc++.h>
#include <mutex>
#include <tbb/concurrent_hash_map.h>

struct Node {
    int key, value;
    Node *next, *prev;
    std::mutex node_lock;

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
    
    // Declare Intel TBB concurrent hash map
    using CacheMap = tbb::concurrent_hash_map<int, Node*>;
    CacheMap cacheMap;

    Node *head, *tail;
    std::mutex hashmap_lock;

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
        CacheMap::accessor acc;
        if (!cacheMap.find(acc, key)) return -1;

        Node* node = acc->second;

        {
            // Acquire locks in order
            std::vector<std::mutex*> locks = 
                {&node->prev->node_lock, &node->node_lock, &node->next->node_lock};
            std::sort(locks.begin(), locks.end());
            std::scoped_lock lock(*locks[0], *locks[1], *locks[2]);

            remove(node);
            add(node);
        }

        return node->value;
    }

    // Function to put a key-value pair into the cache
    void put(int key, int value) {
        Node* newNode = new Node(key, value);

        CacheMap::accessor acc;
        if (cacheMap.find(acc, key)) {
            Node* oldNode = acc->second;
            {
                // Acquire locks in order
                std::vector<std::mutex*> locks = {&oldNode->prev->node_lock, &oldNode->node_lock, &oldNode->next->node_lock};
                std::sort(locks.begin(), locks.end());
                std::scoped_lock lock(*locks[0], *locks[1], *locks[2]);

                remove(oldNode);
                delete oldNode;
            }
            acc->second = newNode;
        } else {
            cacheMap.insert({key, newNode});
        }

        {
            // Acquire locks in order
            std::vector<std::mutex*> locks = {&head->node_lock, &head->next->node_lock};
            std::sort(locks.begin(), locks.end());
            std::scoped_lock lock(*locks[0], *locks[1]);

            add(newNode);
        }

        if (cacheMap.size() > capacity) {
            Node* lru = tail->prev;
            {
                // Acquire locks in order
                std::vector<std::mutex*> locks = {&lru->prev->node_lock, &lru->node_lock, &lru->next->node_lock};
                std::sort(locks.begin(), locks.end());
                std::scoped_lock lock(*locks[0], *locks[1], *locks[2]);

                remove(lru);
                cacheMap.erase(lru->key);
                delete lru;
            }
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