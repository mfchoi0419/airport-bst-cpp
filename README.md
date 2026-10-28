# Airport Code Lookup Service with a Binary Search Tree

## 1. Problem Statement
This project implements a fast lookup service for airport information based on their three-letter IATA codes. The core data structure is a custom Binary Search Tree (BST). The application supports loading data from a real-world dataset, searching, inserting, deleting, traversals, range queries, and validating the BST property. It also includes a performance benchmark comparing different tree construction methods.

## 2. How to Build and Run
We use g++ with C++17 standards. Open your terminal in the project root directory and run:

g++ -std=c++17 -O2 src/*.cpp -o airports
./airports

(Note: Ensure that the data/airports.dat file is present in the data/ folder before running.)

## 3. Sample Session
Below is a sample output of the program demonstrating the main features (Find, Range, Remove, Validate):

========== TASK 2: SEARCH ==========
Searching for BOS:
Search path: GKA -> ... -> BOS
Comparisons: 12
Airport Found!
IATA: BOS
Name: General Edward Lawrence Logan International Airport
City: Boston

========== TASK 5: RANGE QUERY ==========
Airports from BOS to BWI:
BOS - General Edward Lawrence Logan International Airport
BOU - Bourges Airport
... (output truncated for brevity) ...
BWI - Baltimore/Washington International Thurgood Marshall Airport
Matching airports: 113
Nodes visited: 127

========== TASK 3: DELETION ==========
--- D2a: Only Left Child (BOS) ---
Deleting node with only left child: BOS
Replacement BOR: FOUND
BST Validation: VALID

## 4. Task 6: Tree Shape Experiment
We tested different tree building methods using the same 6,072 airports. We also measured the time taken for 10,000 random searches.

| Build Order | Height | Average Depth | Time for 10,000 searches | Explanation |
| :--- | :--- | :--- | :--- | :--- |
| File Order | 177 | 18.3233 | 1.547 ms | The file order is highly unbalanced due to alphabetic clustering, making it worse than a random shuffle. |
| Sorted Order | 6071 | 3035.5 | 134.527 ms | Inserting sorted data creates a degenerate tree (a linked list), causing O(n) search time. |
| Balanced | 12 | 10.6532 | 1.511 ms | Picking the middle element recursively ensures height is approx log2(n), giving O(log n) search time. |
| Random Shuffle (Bonus) | N/A | N/A | N/A | (Bonus not implemented in this version) |

## 5. Data Credit
The data used in this project is sourced from OpenFlights.org (OpenFlights airport database, airports.dat). Data credit to OpenFlights.org.
