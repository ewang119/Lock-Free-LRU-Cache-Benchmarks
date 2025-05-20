#include <parlay/primitives.h>
#include <parlay/random.h>
#include <parlay/io.h>
#include <parlay/internal/get_time.h>
#include <parlay/internal/group_by.h>
#include <flock/flock.h>
#include "hash.h"
#include "dlist.h"
#include <bits/stdc++.h>
using namespace std;

struct Node {
    int key;
    int value;
    Node *next;
    Node *prev;

    Node(int k, int v) {
        key = k;
        value = v;
        next = nullptr;
        prev = nullptr;
    }
};

class LRUCache
{
  public:
  
    int capacity;
    LfHashMap<int, LfDlist<int, int>::node*> cacheMap;
    parlay::sequence<LfHashMap<int, LfDlist<int, int>::node*>::slot> cacheMapSlot;
    LfDlist<int, int> dList;
    LfDlist<int, int>::node *dListNode;
    LfDlist<int, int>::node *head;
    LfDlist<int, int>::node *tail;
    
    LRUCache(int capacity) {
        this->capacity = capacity;
        cacheMapSlot = cacheMap.empty(capacity + 128);
        dListNode = dList.empty(capacity + 128);
        head = dList.insert(dListNode, -1, 1);
        tail = dList.insert(head, -1, 1);

    }

int get(int key) {
    
    auto res = cacheMap.find(cacheMapSlot,key);
    if (res.has_value()) {
        // Move to front
        auto oldNode = res.value();
        int key = oldNode->key;

        dList.remove(dListNode, oldNode);
        dList.insert(head, key, key);
        LfDlist<int, int>::node *newNode = res.value();
        cacheMap.remove(cacheMapSlot, key);
        cacheMap.insert(cacheMapSlot, key, newNode); // New node added

        return key;
    } else {
        return -1;
    }
}

void put(int key, int value) {
    std::optional<LfDlist<int, int>::node *> val = cacheMap.find(cacheMapSlot,key);
    if (val.has_value()) {
        auto *nodeToDelete = val.value();
        int keytoremove = nodeToDelete->key;
        dList.remove(tail, nodeToDelete);
        cacheMap.remove(cacheMapSlot, keytoremove);
    }
    auto *inserted = dList.insert(head, value, value);
    cacheMap.insert(cacheMapSlot, key, inserted);
    if (cacheMap.capacity(cacheMapSlot) > capacity) {
        auto *nodeToDelete = tail->prev.load();
        int keytoremove = nodeToDelete->key;
        dList.remove(tail, nodeToDelete);
        cacheMap.remove(cacheMapSlot, keytoremove);
    }
}
};