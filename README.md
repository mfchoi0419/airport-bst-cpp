# Airport Code Lookup Service with a Binary Search Tree

**Team Members:** Amy Ziccarello, Michael Choi,Lingtong.Ge

## 1. Problem Overview
For this project, we built an Airport Code Lookup Service using a custom Binary Search Tree (BST). The main goal was to store real-world airport data and support fast lookups, insertions, deletions, and range queries. We implemented custom queues and stacks using linked lists to handle level-order traversal and memory cleanup. Finally, we tested how different insertion orders (file order, sorted, and balanced) affect the tree's height and overall search performance.

## 2. How to Build and Run
We compiled our code using g++ with C++17 standards. Open your terminal in the project root directory and run:

g++ -std=c++17 -O2 src/*.cpp -o airports
./airports

(Note: Make sure the data/airports.dat file is in the data/ folder before running.)

## 3. Sample Session
Here is a quick look at what our program outputs when running the main features:

========== TASK 2: SEARCH ==========
Searching for BOS:
Search path: GKA -> ... -> BOS
Comparisons: 12
Airport Found! (IATA: BOS, Name: General Edward Lawrence Logan International Airport)

========== TASK 5: RANGE QUERY ==========
Airports from BOS to BWI:
...
Matching airports: 113
Nodes visited: 127

========== TASK 3: DELETION ==========
Deleting node with only left child: BOS
Replacement BOR: FOUND
BST Validation: VALID

## 4. Task 6: Tree Shape Experiment
We ran a test with the same 6,072 airports using three different building methods. We also recorded the time it took to execute 10,000 random searches on each tree.

| Build Order | Height | Average Depth | Time for 10,000 searches | What we observed |
| :--- | :--- | :--- | :--- | :--- |
| File Order | 177 | 18.3233 | 1.547 ms | We noticed this tree is highly unbalanced. Because the original data file has some alphabetic clustering, inserting them in this order makes it perform worse than a random shuffle. |
| Sorted Order | 6071 | 3035.5 | 134.527 ms | As expected, inserting sorted data created a degenerate tree (basically a linked list). This caused O(n) search time, making it super slow compared to the others. |
| Balanced | 12 | 10.6532 | 1.511 ms | By picking the middle element recursively, we got a height of exactly 12 (which is approx log2(6072)). It gave us the best performance for searching. |
| Random Shuffle (Bonus) | N/A | N/A | N/A | We didn't implement this bonus part, but based on the logic, it would avoid the alphabetic clustering and perform much better than file order. |

## 5. Data Credit
The data used in this project is sourced from OpenFlights.org (OpenFlights airport database, airports.dat). Data credit to OpenFlights.org.
