#include "linkedpq.h"

LinkedPQ::LinkedPQ() {
    front = NULL;
}

LinkedPQ::~LinkedPQ() {
    clear();
}

void LinkedPQ::clear() {
    PNode *current = front;
    while (current != NULL) {
        PNode *node = current;
        current = current->next;
        delete node;
    }
    front = NULL;
}

string LinkedPQ::frontName() {
    if (front == NULL)
        throw runtime_error("Patient queue empty!");
    return front->name;
}

int LinkedPQ::frontPriority() {
    if (front == NULL)
        throw runtime_error("Patient queue empty!");
    return front->priority;
}

bool LinkedPQ::isEmpty() {
    return front == NULL;
}

void LinkedPQ::newPatient(string name, int priority) {
    PNode *node = new PNode(name, priority);
    insertNode(node);
}

string LinkedPQ::processPatient() {
    if (front == NULL)
        throw runtime_error("Patient queue empty!");

    PNode *node = front;
    front = front->next;
    string name = node->name;
    delete node;

    return name;
}

void LinkedPQ::upgradePatient(string name, int oldPriority, int newPriority) {
    PNode *node;

    if (front != NULL && front->name == name && front->priority == oldPriority) {
        node = front;
        front = front->next;
    }
    else {
        PNode *current = front;
        while (current->next != NULL &&
               (current->next->name != name || current->next->priority != oldPriority))
            current = current->next;

        if (current->next != NULL) {
            node = current->next;
            current->next = current->next->next;
        }
        else
            throw runtime_error("Patient with given priority not found!");
    }

    node->priority = newPriority;
    insertNode(node);
}

void LinkedPQ::insertNode(PNode *node) {
    if (front == NULL || front->priority > node->priority) {
        node->next = front;
        front = node;
    }
    else {
        PNode *current = front;
        while (current->next != NULL && current->next->priority <= node->priority)
            current = current->next;

        node->next = current->next;
        current->next = node;
    }
}

void LinkedPQ::debug() {}

ostream& operator <<(ostream& out, const LinkedPQ& queue) {
    PNode *current = queue.front;
    cout << "{";
    while (current != NULL) {
        out << *current;
        if (current->next != NULL)
            cout << ", ";
        current = current->next;
    }
    cout << "}";
    return out;
}
