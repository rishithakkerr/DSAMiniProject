# Hospital Management System
# Data Structures and Algorithms Mini Project

---

## 1. Project Overview

The Hospital Management System is a console-based application developed in C++ to demonstrate the practical implementation of various Data Structures and Algorithms (DSA). The system allows hospital staff to manage patient records, search and sort patient data, maintain appointment queues, assign beds based on priority, and store data using files.

The project integrates multiple DSA concepts such as Arrays, Linked Lists, Stacks, Queues, AVL Trees, Sorting Algorithms, Searching Algorithms, Greedy Algorithms, and File Handling into a single real-world application.

---

# 2. Objectives

- Manage patient records efficiently.
- Demonstrate the use of different Data Structures.
- Compare searching and sorting techniques.
- Implement priority-based patient management.
- Simulate real hospital operations such as appointments and bed allocation.
- Provide file storage and retrieval of patient records.

---

# 3. Data Structures Used

## 3.1 Array

**Purpose:** Stores all active patient records.

### Operations
- Add Patient
- View Patients
- Sort Patients
- Search Patients

### Complexity
- Insertion: O(1)
- Traversal: O(n)

---

## 3.2 Linked List

**Purpose:** Maintains patient history.

Each newly added patient is inserted at the beginning of the linked list.

### Advantages
- Dynamic memory allocation.
- Fast insertion at head.

### Complexity
- Insert: O(1)
- Display History: O(n)

---

## 3.3 Stack (LIFO)

**Purpose:** Implements Undo Delete functionality.

Whenever a patient is deleted, their information is pushed onto a stack.

When Undo is selected:
- Top patient is popped.
- Record is restored.

### Complexity
- Push: O(1)
- Pop: O(1)

---

## 3.4 Queue (FIFO)

**Purpose:** Appointment Management.

Patients are added to the queue when booking appointments and served in the same order.

### Complexity
- Enqueue: O(1)
- Dequeue: O(1)

---

## 3.5 AVL Tree

**Purpose:** Priority-based patient grouping.

Patients are grouped according to:

| Priority | Category |
|-----------|----------|
| 1 | Critical |
| 2 | Urgent |
| 3 | Routine |

Each AVL node stores:
- Priority value
- Bucket (Linked List of patients)
- Patient count

### Advantages
- Self-balancing tree
- Fast searching
- Efficient grouping

### Complexity
- Insert: O(log n)
- Search: O(log n)
- Delete from Bucket: O(log n)

---

# 4. Algorithms Implemented

## 4.1 Linear Search

Searches patients by name.

### Complexity
- Best Case: O(1)
- Worst Case: O(n)

---

## 4.2 Binary Search

Searches patients by ID after sorting.

### Complexity
- O(log n)

---

## 4.3 AVL Tree Search

Searches all patients belonging to a specific priority level.

### Complexity
- O(log n)

---

# 5. Sorting Techniques

## 5.1 Bubble Sort

Used for sorting by Patient ID.

### Complexity
- Worst Case: O(n²)

---

## 5.2 Merge Sort

Used for sorting by Patient Name.

### Complexity
- O(n log n)

---

## 5.3 Quick Sort

Used for sorting by Patient Priority.

### Complexity
- Average Case: O(n log n)

---

# 6. Greedy Bed Assignment

The system assigns available beds based on patient priority.

### Strategy
1. Collect patients from AVL Tree in sorted order.
2. Critical patients receive beds first.
3. Remaining patients are placed on the waiting list.

### Complexity
- Collection: O(n)
- Assignment: O(1) per patient

---

# 7. File Handling

The system supports permanent storage using text files.

## Save Operation
Stores:
- ID
- Name
- Age
- Disease
- Priority

into `patients.txt`.

## Load Operation
Reads records from the file and rebuilds:
- Patient Array
- Linked List History
- AVL Tree Structure

### Complexity
- Save: O(n)
- Load: O(n)

---

# 8. Features of the System

### Patient Management
- Add Patient
- View Patients
- Delete Patient
- Undo Delete

### Searching
- Linear Search by Name
- Binary Search by ID
- AVL Search by Priority

### Sorting
- Bubble Sort by ID
- Merge Sort by Name
- Quick Sort by Priority

### Data Structure Demonstrations
- Linked List History
- Stack Undo Operation
- Queue Appointment System
- AVL Priority Management

### Hospital Operations
- Bed Assignment
- Priority Tracking

### Storage
- Save Data to File
- Load Data from File

---

# 9. Menu Structure

1. Add Patient
2. View All Patients
3. Search Patient
4. Sort Patients
5. Delete Patient
6. Undo Delete
7. Appointment Queue
8. View History
9. View Priority List
10. Bed Assignment
11. File Save / Load
0. Exit

---

# 10. Sample Workflow

1. Add a new patient.
2. Patient is stored in:
   - Array
   - Linked List History
   - AVL Tree
3. Search patient using Name, ID, or Priority.
4. Sort records using different algorithms.
5. Delete a patient.
6. Restore using Undo Delete.
7. Book appointments through Queue.
8. Assign beds using Priority-Based Greedy Algorithm.
9. Save data to file.

---

# 11. Learning Outcomes

Through this project, the following DSA concepts were implemented and understood:

- Arrays
- Linked Lists
- Stacks
- Queues
- AVL Trees
- Linear Search
- Binary Search
- Bubble Sort
- Merge Sort
- Quick Sort
- Greedy Algorithms
- File Handling
- Time Complexity Analysis

---

# 12. Conclusion

The Hospital Management System successfully demonstrates the practical application of multiple Data Structures and Algorithms within a real-world healthcare scenario. By integrating searching, sorting, priority management, file storage, and resource allocation techniques, the project provides a comprehensive understanding of how DSA concepts can be used to build efficient and scalable software systems.
