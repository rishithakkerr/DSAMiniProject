Hospital Management System:
A console-based Hospital Management System built in C++ as a Data Structures & Algorithms project. The system simulates core hospital workflows — from registering patients to assigning beds — while demonstrating a range of DSA concepts applied to real-world scenarios.

What it does:
Patients are stored with an ID, name, age, diagnosis, and a priority level (Critical, Urgent, or Routine). From there, the system supports the full patient lifecycle: add, search, sort, delete, and restore records, all through an interactive menu.

Key features & the DSA behind them:
AVL Tree — groups patients by priority for O(log n) insertions and lookups, powering the priority-based bed assignment
Greedy Bed Assignment — allocates available beds to the most critical patients first using an in-order AVL traversal
Linked List — maintains a chronological patient history log
Stack — enables single-step undo on deletions
Queue — manages appointment booking in a first-come, first-served order
Multiple sorting algorithms — Bubble Sort (by ID), Merge Sort (by name), and Quick Sort (by priority)
Multiple search modes — Linear Search by name, Binary Search by ID, and AVL-based priority search
File I/O — saves and loads patient records to/from disk for persistence across sessions
