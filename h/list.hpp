#ifndef LIST_HPP_
#define LIST_HPP_

template<typename T>
class List {
private:
    struct Elem {
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {
        }
    };

    Elem *head, *tail;

public:
    List() : head(nullptr), tail(nullptr) {
    };

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T *data) {
        Elem *new_elem = new Elem(data, head);
        head = new_elem;
        if (!tail) tail = head;
    }

    void addLast(T *data) {
        Elem *new_elem = new Elem(data, 0);
        if (tail) {
            tail->next = new_elem;
            tail = new_elem;
        } else {
            head = tail = new_elem;
        }
    }

    T *removeFirst() {
        if (!head) return 0;
        Elem *old_head = head;
        head = head->next;
        if (!head) tail = 0;

        T *ret = old_head->data;
        delete old_head;
        return ret;
    }

    T *peakFirst() {
        if (!head) return 0;
        return head->data;
    }

    T *removeLast() {
        if (!tail) return 0;
        Elem *prev = 0;
        for (Elem *cur = head; cur && cur != tail; cur = cur->next) {
            prev = cur;
        }
        Elem *old_tail = tail;
        if (prev) prev->next = nullptr;
        else head = nullptr;
        tail = prev;

        T *ret = old_tail->data;
        delete old_tail;
        return ret;
    }

    T *peakLast() {
        if (!tail) return 0;
        return tail->data;
    }
};
#endif
