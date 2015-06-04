#ifndef _LIST_H_
#define _LIST_H_

#include <stddef.h>

#define LIST_INITIALIZER ((struct list) {0, 0})
#define LIST_INITIALIZER2(var) { &var, &var }

struct list {
    list *next;
    list *prev;

    void init();
    list *front();
    list *back();

    void insert_front(list *e);
    void insert_back(list *e);
    list *pop_front();
    list *pop_back();

    bool empty();
    void remove();

    template<typename U, typename X>
    U *entry(X U::* x);

    template<typename A>
    void for_each(A &a);

    template<typename A>
    void for_each_reverse(A &a);

#define list_for_each(iterator, head)                                   \
    for ((iterator) = (head)->next; (iterator) != (head); (iterator) = (iterator)->next)

#define list_for_each_rev(iterator, head)                               \
    for ((iterator) = (head)->prev; (iterator) != (head); (iterator) = (iterator)->prev)
};



inline void list::init() {
    next = prev = this;
}

inline list *list::front() {
    return next;
}

inline list *list::back() {
    return prev;
}

inline void list::insert_front(list *e) {
    e->next = next;
    e->prev = this;
    next->prev = e;
    next = e;
}

inline void list::insert_back(list *e) {
    e->next = this;
    e->prev = prev;
    prev->next = e;
    prev = e;
}

inline list *list::pop_front() {
    list *l = front();
    l->remove();
    return l;
}

inline list *list::pop_back() {
    list *l = back();
    l->remove();
    return l;
}

inline bool list::empty() {
    return this->next == this;
}

inline void list::remove() {
    prev->next = next;
    next->prev = prev;
}

template<typename U, typename X>
inline U *list::entry(X U::* x) {
    size_t ofs = (size_t)&(((U *)0)->*x); // offsetof(U, x)
    return (U *)((char *)this - ofs);
}

template<typename A>
inline void list::for_each(A &a) {
    list *l = next;
    while (l != this) {
        a(l);
        l = l->next;
    }
}

template<typename A>
inline void list::for_each_reverse(A &a) {
    list *l = prev;
    while (l != this) {
        a(l);
        l = l->prev;
    }
}

#endif // _LIST_H_
