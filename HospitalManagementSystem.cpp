// ============================================================
//    HOSPITAL MANAGEMENT SYSTEM
// ============================================================

#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <fstream>
using namespace std;

const int MAX_PATIENTS = 100;

// ============================================================
// PATIENT STRUCTURE
// ============================================================
struct Patient {
    int    id;
    string name;
    int    age;
    string disease;
    int    priority;
};

// ============================================================
// GLOBAL DATA
// ============================================================
Patient patients[MAX_PATIENTS];
int     patientCount = 0;
int     nextID       = 1;

stack<Patient> undoStack;
queue<string>  appointmentQ;

// ============================================================
// UTILITY
// ============================================================
void displayPatient(Patient p) {
    cout << "ID: "          << p.id
         << " | Name: "    << p.name
         << " | Age: "     << p.age
         << " | Disease: " << p.disease
         << " | Priority: "<< p.priority << "\n";
}

void displayAll() {
    if (patientCount == 0) { cout << "No patients registered.\n"; return; }
    cout << "\n--- All Patients ---\n";
    cout << "---------------------------------------------------\n";
    for (int i = 0; i < patientCount; i++)
        displayPatient(patients[i]);
    cout << "---------------------------------------------------\n";
    cout << "Total Patients: " << patientCount << "\n";
}

// ============================================================
// LINKED LIST — Patient History
// Insert O(1)  |  Traverse O(n)
// ============================================================
struct ListNode {
    Patient   data;
    ListNode* next;
    ListNode(Patient p) : data(p), next(NULL) {}
};

class PatientHistory {
public:
    ListNode* head;
    PatientHistory() : head(NULL) {}

    void addToHistory(Patient p) {
        ListNode* node = new ListNode(p);
        node->next = head;
        head = node;
    }

    void display() {
        if (!head) { cout << "No history found.\n"; return; }
        cout << "\n--- Patient History (Linked List) ---\n";
        cout << "---------------------------------------------------\n";
        ListNode* t = head;
        while (t) { displayPatient(t->data); t = t->next; }
        cout << "---------------------------------------------------\n";
    }
} history;

// ============================================================
// AVL TREE — Priority-Based Patient Grouping
//
// Key   : priority (1, 2, or 3)
// Value : linked list of all patients with that priority
//
//O(log n)
// ============================================================
struct BucketNode {
    Patient     data;
    BucketNode* next;
    BucketNode(Patient p) : data(p), next(NULL) {}
};

struct AVLNode {
    int         priority;
    BucketNode* bucket;
    int         count;
    AVLNode*    left;
    AVLNode*    right;
    int         height;

    AVLNode(int pr) : priority(pr), bucket(NULL), count(0),
                      left(NULL), right(NULL), height(1) {}
};

int avlHeight(AVLNode* n)  { return n ? n->height : 0; }
int avlMax(int a, int b)   { return (a > b) ? a : b; }
int getBalance(AVLNode* n) { return n ? avlHeight(n->left) - avlHeight(n->right) : 0; }

AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x  = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left  = T2;
    y->height = avlMax(avlHeight(y->left),  avlHeight(y->right))  + 1;
    x->height = avlMax(avlHeight(x->left),  avlHeight(x->right))  + 1;
    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y  = x->right;
    AVLNode* T2 = y->left;
    y->left  = x;
    x->right = T2;
    x->height = avlMax(avlHeight(x->left),  avlHeight(x->right))  + 1;
    y->height = avlMax(avlHeight(y->left),  avlHeight(y->right))  + 1;
    return y;
}

AVLNode* avlInsert(AVLNode* node, Patient p) {
    if (!node) {
        AVLNode* newNode = new AVLNode(p.priority);
        newNode->bucket  = new BucketNode(p);
        newNode->count   = 1;
        return newNode;
    }

    if (p.priority < node->priority)
        node->left  = avlInsert(node->left,  p);
    else if (p.priority > node->priority)
        node->right = avlInsert(node->right, p);
    else {
        BucketNode* bn = new BucketNode(p);
        bn->next       = node->bucket;
        node->bucket   = bn;
        node->count++;
        return node;
    }

    node->height = 1 + avlMax(avlHeight(node->left), avlHeight(node->right));

    int bal = getBalance(node);
    if (bal >  1 && p.priority < node->left->priority)  return rotateRight(node);
    if (bal < -1 && p.priority > node->right->priority) return rotateLeft(node);
    if (bal >  1 && p.priority > node->left->priority)  { node->left = rotateLeft(node->left);  return rotateRight(node); }
    if (bal < -1 && p.priority < node->right->priority) { node->right = rotateRight(node->right); return rotateLeft(node); }
    return node;
}

bool removeFromBucket(AVLNode* node, Patient p) {
    if (!node) return false;
    if      (p.priority < node->priority) return removeFromBucket(node->left,  p);
    else if (p.priority > node->priority) return removeFromBucket(node->right, p);
    else {
        BucketNode* prev = NULL;
        BucketNode* cur  = node->bucket;
        while (cur) {
            if (cur->data.id == p.id) {
                if (prev) prev->next    = cur->next;
                else       node->bucket = cur->next;
                delete cur;
                node->count--;
                return true;
            }
            prev = cur; cur = cur->next;
        }
    }
    return false;
}

void avlInOrder(AVLNode* root) {
    if (!root) return;
    avlInOrder(root->left);
    string label = (root->priority == 1) ? "Critical" :
                   (root->priority == 2) ? "Urgent"   : "Routine";
    cout << "\n  [Priority " << root->priority << " — " << label
         << " | " << root->count << " patient(s)]\n";
    BucketNode* bn = root->bucket;
    while (bn) { cout << "    "; displayPatient(bn->data); bn = bn->next; }
    avlInOrder(root->right);
}

AVLNode* avlFind(AVLNode* root, int priority) {
    if (!root)                       return NULL;
    if (priority == root->priority)  return root;
    if (priority <  root->priority)  return avlFind(root->left,  priority);
    return                                  avlFind(root->right, priority);
}

AVLNode* avlRoot = NULL;

// ============================================================
// FEATURE 1: ADD PATIENT
// ============================================================
void addPatient() {
    if (patientCount >= MAX_PATIENTS) { cout << "Patient list full!\n"; return; }

    Patient p;
    p.id = nextID++;
    cout << "\n--- Add New Patient ---\n";
    cout << "Assigned Patient ID: " << p.id << "\n";

    cout << "Enter Name: ";
    cin.ignore();
    getline(cin, p.name);

    cout << "Enter Age: ";
    cin >> p.age;

    cout << "Enter Disease: ";
    cin.ignore();
    getline(cin, p.disease);

    cout << "Enter Priority (1=Critical, 2=Urgent, 3=Routine): ";
    cin >> p.priority;

    patients[patientCount++] = p;
    history.addToHistory(p);
    avlRoot = avlInsert(avlRoot, p);

    cout << "[Success] Patient " << p.name << " added!\n";
}

// ============================================================
// FEATURE 3: SEARCH
// Linear O(n)  |  Binary O(log n)  |  AVL Priority O(log n)
// ============================================================
void linearSearchByName() {
    string name;
    cout << "Enter patient name: ";
    cin.ignore();
    getline(cin, name);

    bool found = false;
    cout << "\n[Linear Search] Results:\n";
    for (int i = 0; i < patientCount; i++)
        if (patients[i].name == name) { displayPatient(patients[i]); found = true; }
    if (!found) cout << "No patient found with name: " << name << "\n";
}

void binarySearchByID() {
    int searchID;
    cout << "Enter patient ID: ";
    cin >> searchID;

    Patient sorted[MAX_PATIENTS];
    for (int i = 0; i < patientCount; i++) sorted[i] = patients[i];
    for (int i = 0; i < patientCount - 1; i++)
        for (int j = 0; j < patientCount - i - 1; j++)
            if (sorted[j].id > sorted[j+1].id) {
                Patient tmp = sorted[j]; sorted[j] = sorted[j+1]; sorted[j+1] = tmp;
            }

    int lo = 0, hi = patientCount - 1;
    bool found = false;
    cout << "\n[Binary Search] Searching...\n";
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if      (sorted[mid].id == searchID) { displayPatient(sorted[mid]); found = true; break; }
        else if (sorted[mid].id <  searchID)  lo = mid + 1;
        else                                   hi = mid - 1;
    }
    if (!found) cout << "Patient with ID " << searchID << " not found.\n";
}

void avlSearchByPriority() {
    int pr;
    cout << "Enter priority (1=Critical, 2=Urgent, 3=Routine): ";
    cin >> pr;
    if (pr < 1 || pr > 3) { cout << "Invalid priority!\n"; return; }

    AVLNode* node = avlFind(avlRoot, pr);
    if (!node || node->count == 0) { cout << "No patients found with that priority.\n"; return; }

    string label = (pr == 1) ? "Critical" : (pr == 2) ? "Urgent" : "Routine";
    cout << "\n[AVL Search] Priority " << pr << " (" << label
         << ") — " << node->count << " patient(s):\n";
    cout << "---------------------------------------------------\n";
    BucketNode* bn = node->bucket;
    while (bn) { displayPatient(bn->data); bn = bn->next; }
    cout << "---------------------------------------------------\n";
}

void searchPatient() {
    cout << "\n--- Search Patient ---\n";
    cout << "1. Linear Search (by Name)     - O(n)\n";
    cout << "2. Binary Search (by ID)       - O(log n)\n";
    cout << "3. AVL Search    (by Priority) - O(log n)\n";
    cout << "Choose: ";
    int c; cin >> c;
    if      (c == 1) linearSearchByName();
    else if (c == 2) binarySearchByID();
    else if (c == 3) avlSearchByPriority();
    else cout << "Invalid choice!\n";
}

// ============================================================
// FEATURE 4: SORT
// Bubble O(n^2)  |  Merge O(n log n)  |  Quick O(n log n) avg
// ============================================================
void bubbleSortByID() {
    cout << "\n[Bubble Sort] Sorting by ID...\n";
    for (int i = 0; i < patientCount - 1; i++)
        for (int j = 0; j < patientCount - i - 1; j++)
            if (patients[j].id > patients[j+1].id) {
                Patient tmp = patients[j]; patients[j] = patients[j+1]; patients[j+1] = tmp;
            }
    cout << "[Done] Sorted by ID.\n";
    displayAll();
}

void mergeByName(Patient arr[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    Patient L[MAX_PATIENTS], R[MAX_PATIENTS];
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].name <= R[j].name) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSortByName(Patient arr[], int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        mergeSortByName(arr, l, m);
        mergeSortByName(arr, m + 1, r);
        mergeByName(arr, l, m, r);
    }
}

void sortByName() {
    cout << "\n[Merge Sort] Sorting by Name...\n";
    mergeSortByName(patients, 0, patientCount - 1);
    cout << "[Done] Sorted by Name.\n";
    displayAll();
}

int partitionByPriority(Patient arr[], int lo, int hi) {
    int pivot = arr[hi].priority, i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j].priority <= pivot) {
            i++;
            Patient tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        }
    }
    Patient tmp = arr[i+1]; arr[i+1] = arr[hi]; arr[hi] = tmp;
    return i + 1;
}

void quickSortPriority(Patient arr[], int lo, int hi) {
    if (lo < hi) {
        int pi = partitionByPriority(arr, lo, hi);
        quickSortPriority(arr, lo, pi - 1);
        quickSortPriority(arr, pi + 1, hi);
    }
}

void sortByPriority() {
    Patient copy[MAX_PATIENTS];
    for (int i = 0; i < patientCount; i++) copy[i] = patients[i];
    quickSortPriority(copy, 0, patientCount - 1);
    cout << "\n[Quick Sort] Sorted by Priority:\n";
    cout << "---------------------------------------------------\n";
    for (int i = 0; i < patientCount; i++) displayPatient(copy[i]);
    cout << "---------------------------------------------------\n";
}

void sortPatients() {
    if (patientCount == 0) { cout << "No patients to sort!\n"; return; }
    cout << "\n--- Sort Patients ---\n";
    cout << "1. Bubble Sort (by ID)       - O(n^2)\n";
    cout << "2. Merge Sort  (by Name)     - O(n log n)\n";
    cout << "3. Quick Sort  (by Priority) - O(n log n) avg\n";
    cout << "Choose: ";
    int c; cin >> c;
    if      (c == 1) bubbleSortByID();
    else if (c == 2) sortByName();
    else if (c == 3) sortByPriority();
    else cout << "Invalid choice!\n";
}

// ============================================================
// FEATURE 5: DELETE PATIENT
// ============================================================
void deletePatient() {
    if (patientCount == 0) { cout << "No patients to delete!\n"; return; }
    int delID;
    cout << "\n--- Delete Patient ---\n";
    cout << "Enter Patient ID to delete: ";
    cin >> delID;

    bool found = false;
    for (int i = 0; i < patientCount; i++) {
        if (patients[i].id == delID) {
            Patient p = patients[i];
            undoStack.push(p);
            removeFromBucket(avlRoot, p);
            cout << "[Deleted] "; displayPatient(p);
            for (int j = i; j < patientCount - 1; j++)
                patients[j] = patients[j+1];
            patientCount--;
            found = true; break;
        }
    }
    if (!found) cout << "Patient ID " << delID << " not found!\n";
}

// ============================================================
// FEATURE 6: UNDO DELETE (Stack — LIFO)
// ============================================================
void undoDelete() {
    cout << "\n--- Undo Delete (Stack) ---\n";
    if (undoStack.empty()) { cout << "Nothing to undo! Stack is empty.\n"; return; }
    Patient p = undoStack.top(); undoStack.pop();
    patients[patientCount++] = p;
    avlRoot = avlInsert(avlRoot, p);
    cout << "[Undo] Restored: "; displayPatient(p);
}

// ============================================================
// FEATURE 7: APPOINTMENT QUEUE (Queue — FIFO)
// ============================================================
void appointmentMenu() {
    cout << "\n--- Appointment Queue ---\n";
    cout << "1. Book Appointment\n";
    cout << "2. Call Next Patient\n";
    cout << "3. View All Appointments\n";
    cout << "Choose: ";
    int c; cin >> c;

    if (c == 1) {
        string name;
        cout << "Enter patient name: ";
        cin.ignore(); getline(cin, name);
        appointmentQ.push(name);
        cout << "[Queued] " << name << " added. Queue size: " << appointmentQ.size() << "\n";
    } else if (c == 2) {
        if (appointmentQ.empty()) cout << "No appointments in queue!\n";
        else {
            cout << "[Serving] " << appointmentQ.front() << "\n";
            appointmentQ.pop();
            cout << "Remaining: " << appointmentQ.size() << "\n";
        }
    } else if (c == 3) {
        if (appointmentQ.empty()) { cout << "Queue is empty.\n"; return; }
        queue<string> tmp = appointmentQ;
        cout << "\n--- Appointment Queue ---\n";
        int pos = 1;
        while (!tmp.empty()) { cout << pos++ << ". " << tmp.front() << "\n"; tmp.pop(); }
    } else {
        cout << "Invalid choice!\n";
    }
}

// ============================================================
// FEATURE 9: VIEW PRIORITY LIST (AVL Tree)
// In-order traversal = Critical -> Urgent -> Routine
// ============================================================
void viewPriorityList() {
    if (!avlRoot) { cout << "AVL Tree is empty.\n"; return; }
    cout << "\n--- Priority List (AVL Tree) ---\n";
    cout << "Critical (1) -> Urgent (2) -> Routine (3)\n";
    cout << "===================================================\n";
    avlInOrder(avlRoot);
    cout << "===================================================\n";
}

// ============================================================
// FEATURE 10: GREEDY BED ASSIGNMENT
// Pulls patients from AVL in priority order (Critical first)
// O(n) collect + O(1) per assignment
// ============================================================
int collectInOrder(AVLNode* root, Patient out[], int idx) {
    if (!root) return idx;
    idx = collectInOrder(root->left, out, idx);
    BucketNode* bn = root->bucket;
    while (bn) { out[idx++] = bn->data; bn = bn->next; }
    idx = collectInOrder(root->right, out, idx);
    return idx;
}

void greedyBedAssignment() {
    if (patientCount == 0) { cout << "No patients registered!\n"; return; }
    int beds;
    cout << "\n--- Greedy Bed Assignment ---\n";
    cout << "Enter number of available beds: ";
    cin >> beds;
    if (beds <= 0) { cout << "No beds available!\n"; return; }

    Patient sorted[MAX_PATIENTS];
    int total = collectInOrder(avlRoot, sorted, 0);

    cout << "\n[Greedy] Assigning " << beds << " bed(s) — Critical patients first:\n";
    cout << "---------------------------------------------------\n";
    int assigned = 0;
    for (int i = 0; i < total && assigned < beds; i++) {
        cout << "Bed " << (assigned + 1) << " --> "; displayPatient(sorted[i]);
        assigned++;
    }
    if (assigned < total) {
        cout << "[Waiting] " << total - assigned << " patient(s) still waiting:\n";
        for (int i = assigned; i < total; i++) displayPatient(sorted[i]);
    }
    cout << "Assigned: " << assigned << "/" << total << "\n";
}

// ============================================================
// FEATURE 11: FILE I/O
// ============================================================
void saveToFile() {
    ofstream f("patients.txt");
    if (!f) { cout << "Error opening file!\n"; return; }
    for (int i = 0; i < patientCount; i++)
        f << patients[i].id       << "\n"
          << patients[i].name     << "\n"
          << patients[i].age      << "\n"
          << patients[i].disease  << "\n"
          << patients[i].priority << "\n";
    f.close();
    cout << "[File] " << patientCount << " patient(s) saved to patients.txt\n";
}

void loadFromFile() {
    ifstream f("patients.txt");
    if (!f) { cout << "Error: patients.txt not found!\n"; return; }
    patientCount = 0;
    Patient p;
    int count = 0;
    while (f >> p.id) {
        f.ignore();
        getline(f, p.name);
        f >> p.age;
        f.ignore();
        getline(f, p.disease);
        f >> p.priority;
        patients[patientCount++] = p;
        history.addToHistory(p);
        avlRoot = avlInsert(avlRoot, p);
        if (p.id >= nextID) nextID = p.id + 1;
        count++;
    }
    f.close();
    cout << "[File] " << count << " patient(s) loaded from patients.txt\n";
}

void fileMenu() {
    cout << "\n--- File Operations ---\n";
    cout << "1. Save Patients to File\n";
    cout << "2. Load Patients from File\n";
    cout << "Choose: ";
    int c; cin >> c;
    if      (c == 1) saveToFile();
    else if (c == 2) loadFromFile();
    else cout << "Invalid choice!\n";
}

// ============================================================
// MAIN
// ============================================================
int main() {

    cout << "============================================\n";
    cout << "   HOSPITAL MANAGEMENT SYSTEM (DSA Project)\n";
    cout << "============================================\n";
    cout << "[System] 3 sample patients loaded.\n";

    int choice;
    do {
        cout << "\n============================================\n";
        cout << "               MAIN MENU\n";
        cout << "============================================\n";
        cout << " 1.  Add Patient\n";
        cout << " 2.  View All Patients\n";
        cout << " 3.  Search Patient  (Linear / Binary / AVL)\n";
        cout << " 4.  Sort Patients   (Bubble / Merge / Quick)\n";
        cout << " 5.  Delete Patient\n";
        cout << " 6.  Undo Delete     (Stack)\n";
        cout << " 7.  Appointment Queue\n";
        cout << " 8.  View History    (Linked List)\n";
        cout << " 9.  View Priority List (AVL Tree)\n";
        cout << " 10. Bed Assignment     (Greedy)\n";
        cout << " 11. File Save / Load\n";
        cout << " 0.  Exit\n";
        cout << "============================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input! Please enter a number.\n";
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:  addPatient();           break;
            case 2:  displayAll();           break;
            case 3:  searchPatient();        break;
            case 4:  sortPatients();         break;
            case 5:  deletePatient();        break;
            case 6:  undoDelete();           break;
            case 7:  appointmentMenu();      break;
            case 8:  history.display();      break;
            case 9:  viewPriorityList();     break;
            case 10: greedyBedAssignment();  break;
            case 11: fileMenu();             break;
            case 0:
                cout << "\n[Goodbye] Exiting Hospital Management System.\n";
                break;
            default:
                cout << "Invalid choice! Please enter 0-11.\n";
        }

    } while (choice != 0);

    return 0;
}

/*
============================================================
BIG O SUMMARY TABLE
============================================================

Operation               | DSA Used               | Time        | Space
------------------------|------------------------|-------------|-------
Add Patient             | Array                  | O(1)        | O(1)
Add to History          | Linked List            | O(1)        | O(1)
AVL Insert              | AVL Tree + bucket list | O(log n)    | O(1)
AVL Delete              | AVL Tree + bucket list | O(log n)    | O(1)
AVL Priority Search     | AVL Tree               | O(log n)    | O(1)
View All                | Array                  | O(n)        | O(1)
Linear Search (name)    | Array                  | O(n)        | O(1)
Binary Search (ID)      | Sorted Array           | O(log n)    | O(1)
Bubble Sort (ID)        | Array                  | O(n^2)      | O(1)
Merge Sort (name)       | Array                  | O(n log n)  | O(n)
Quick Sort (priority)   | Array                  | O(n log n)  | O(log n)
Delete Patient          | Array + AVL bucket     | O(n)        | O(1)
Undo Delete             | Stack                  | O(1)        | O(1)
Book Appointment        | Queue                  | O(1)        | O(1)
Serve Appointment       | Queue                  | O(1)        | O(1)
Priority List Display   | AVL in-order           | O(n)        | O(1)
Greedy Bed Assignment   | AVL in-order collect   | O(n)        | O(n)
Save to File            | Sequential File        | O(n)        | O(1)
Load from File          | Sequential File        | O(n)        | O(1)

============================================================
*/
