#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <chrono>
#include <random>
#include <utility>
using namespace std;

class Airport
{
public:
    string name;
    string city;
    string country;
    string IATA;
    string ICAO;

    double latitude;
    double longitude;

    Airport* left;
    Airport* right;

    // Constructor
    Airport(string airportName, string airportCity,
            string airportCountry, string iata,
            string icao, double lat, double lon)
    {
        name = airportName;
        city = airportCity;
        country = airportCountry;
        IATA = iata;
        ICAO = icao;

        latitude = lat;
        longitude = lon;

        left = nullptr;
        right = nullptr;
    }
};

class AirportBST
{
private:
    Airport* root;

    // Traversal helpers
    void inorderHelper(Airport* node, ofstream& file);
    void preorderHelper(Airport* node);

    // Deletion helpers
    Airport* removeHelper(Airport* node, string code);
    Airport* findMinNode(Airport* node);

    // Height and depth helpers

    // BST validation
    bool validateHelper(Airport* node, string lo, string hi);

    // Range query
    void rangeHelper(Airport* node, string from, string to, 
                     int& visited, int& found);
    
    // Balanced BST construction
    Airport* balancedHelper(
        const vector<Airport>& airports,
        int left, int right);
    
    vector<Airport> records;

    // Write IATA codes in preorder to a separate file
    void savePreorderHelper(Airport* node, ofstream& file);

public:
    // Constructor
    AirportBST()
    {
        root = nullptr;
    }

    // Destructor
    ~AirportBST()
    {
        clearTree();
    }

    // Prevent copying
    AirportBST(const AirportBST&) = delete;
    AirportBST& operator=(const AirportBST&) = delete;

    // Build and insert
    bool insert(Airport* airport);
    void loadAirports(string filename);
    int height();
    double averageDepth();

    // Search
    Airport* search(string code);
    string findMin();
    string findMax();

    // Delete
    void remove(string code);

    // Traversals and validation
    void saveInorder(string filename);
    void printPreorder();
    void printLevelOrder(int maxLevels = 3);
    bool validate();

    // Range queries
    void rangeQuery(string from, string to);

    // Balanced construction
    void buildBalanced(const vector<Airport>& airports);

    // Memory management
    void clearTree();

    string findLeaf();

    string findOnlyRightChild();

    string findTwoChildren();

    void savePreorder(string filename);

    void loadPreorder(string filename, const AirportBST& source);

    const vector<Airport>& getRecords() const;
    void buildSorted(const vector<Airport>& airports);

    Airport* searchSilent(const string& code) const;
};


//=============================================================
// TASK 1
//=============================================================
bool readFile(const string& filename, vector<string>& lines)
{
    ifstream file(filename);
    
    if (!file.is_open())
    {
        return false;
    }

    string line;

    while (getline(file, line))
    {
        lines.push_back(line);
    }

    return true;
}

vector<string> parseCSV(const string& line)
{
    vector<string> fields;
    string field;
    bool inQuotes = false;

    for (int i = 0; i < (int)line.size(); i++)
    {
        char ch = line[i];

        if (ch == '"')
        {
            if (inQuotes && i + 1 < (int)line.size() && line[i + 1] == '"')
            {
                field += '"';
                i++;
            }
            else
            {
                inQuotes = !inQuotes;
            }
        }
        else if (ch == ',' && !inQuotes)
        {
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field += ch;
        }
    }

    fields.push_back(field);
    return fields;
}

void AirportBST::loadAirports(string filename)
{
    vector<string> lines;

    if (!readFile(filename, lines))
    {
        cout << "Error: cannot open " << filename << endl;
        return;
    }

    clearTree();
    records.clear();

    int inserted = 0;
    int skipped = 0;
    int duplicates = 0;

    for (int i = 0; i < static_cast<int>(lines.size()); i++)
    {
        const string& line = lines[i];
        vector<string> fields = parseCSV(line);

        // Check required fields
        if (fields.size() < 8)
        {
            skipped++;
            continue;
        }

        // Skip airports without valid IATA codes
        string iata = fields[4];

        if (iata == "\\N" || iata.size() != 3)
        {
            skipped++;
            continue;
        }

        try
        {
            string name = fields[1];
            string city = fields[2];
            string country = fields[3];
            string icao = fields[5];

            double latitude = stod(fields[6]);
            double longitude = stod(fields[7]);

            // Create separate node for BST
            Airport* airport = new Airport(
                name, city, country, iata,
                icao, latitude, longitude
            );

            if (insert(airport))
            {
                records.emplace_back(
                    name, city, country,
                    iata, icao, latitude,
                    longitude);
                inserted++;
            }
            else
            {
                duplicates++;
            }
        }
        catch (...)
        {
            skipped++;
        }
    }

    cout << "Loaded " << inserted << " airports" << endl;
    cout << "Skipped " << skipped << " airports" << endl;
    cout << "Duplicates " << duplicates << " airports" << endl;
}

bool AirportBST::insert(Airport* airport)
{
    // Empty tree
    if (root == nullptr)
    {
        root = airport;
        return true;
    }

    Airport* cur = root;

    while (cur != nullptr)
    {
        // Smaller IATA code
        if (airport->IATA < cur->IATA)
        {
            if (cur->left == nullptr)
            {
                cur->left = airport;
                return true;
            }

            cur = cur->left;
        }
        // Larger IATA code
        else if (airport->IATA > cur->IATA)
        {
            if (cur->right == nullptr)
            {
                cur->right = airport;
                return true;
            }

            cur = cur->right;
        }
        // Duplicate IATA code
        else
        {
            cout << "Duplicate IATA code: " << airport->IATA << endl;
            delete airport;
            return false;
        }
    }

    return false;
}

void AirportBST::clearTree()
{
    if (root == nullptr)
    {
        return;
    }

    vector<Airport*> nodes;
    nodes.push_back(root);

    while (!nodes.empty())
    {
        Airport* cur = nodes.back();
        nodes.pop_back();

        if (cur->left != nullptr)
        {
            nodes.push_back(cur->left);
        }

        if (cur->right != nullptr)
        {
            nodes.push_back(cur->right);
        }

        delete cur;
    }

    root = nullptr;
}

int AirportBST::height()
{
    if (root == nullptr)
    {
        return -1;
    }

    queue<Airport*> q;
    q.push(root);

    int h = -1;
    
    while (!q.empty())
    {
        int levelSize = q.size();
        h++;

        for (int i = 0; i < levelSize; i++)
        {
            Airport* cur = q.front();
            q.pop();

            if (cur->left != nullptr)
            {
                q.push(cur->left);
            }

            if (cur->right != nullptr)
            {
                q.push(cur->right);
            }
        }
    }

    return h;
}

double AirportBST::averageDepth()
{
    if (root == nullptr)
    {
        return 0.0;
    }

    queue<pair<Airport*, int>> q;
    q.push({root, 0});

    long long totalDepth = 0;
    int count = 0;

    while (!q.empty())
    {
        Airport* cur = q.front().first;
        int depth = q.front().second;
        q.pop();

        totalDepth += depth;
        count++;

        if (cur->left != nullptr)
        {
            q.push({cur->left, depth + 1});
        }

        if (cur->right != nullptr)
        {
            q.push({cur->right, depth + 1});
        }
    }

    return static_cast<double>(totalDepth) / count;
}

//=============================================================
// TASK 2
//=============================================================
Airport* AirportBST::search(string code)
{
    Airport* cur = root;
    int comparisons = 0;

    cout << "Search path: ";

    while (cur != nullptr)
    {
        comparisons++;

        cout << cur->IATA;

        if (code == cur->IATA)
        {
            cout << endl;
            cout << "Comparisons: " << comparisons << endl;
            return cur;
        }
        else if (code < cur->IATA)
        {
            cout << " -> ";
            cur = cur->left;
        }
        else
        {
            cout << " -> ";
            cur = cur->right;
        }
    }

    cout << "NULL" << endl;
    cout << "Comparisons: " << comparisons << endl;
    return nullptr;
}

string AirportBST::findMin()
{
    if (root == nullptr)
    {
        return "";
    }

    Airport* cur = root;

    while (cur->left != nullptr)
    {
        cur = cur->left;
    }

    return cur->IATA;
}

string AirportBST::findMax()
{
    if (root == nullptr)
    {
        return "";
    }

    Airport* cur = root;

    while (cur->right != nullptr)
    {
        cur = cur->right;
    }

    return cur->IATA;
}

string AirportBST::findLeaf()
{
    if (root == nullptr)
    {
        return "";
    }

    queue<Airport*> q;
    q.push(root);

    while (!q.empty())
    {
        Airport* cur = q.front();
        q.pop();

        if (cur->left == nullptr && cur->right == nullptr)
        {
            return cur->IATA;
        }

        if (cur->left != nullptr)
        {
            q.push(cur->left);
        }

        if (cur->right != nullptr)
        {
            q.push(cur->right);
        }
    }

    return "";
}

//=============================================================
// TASK 3
//=============================================================
void AirportBST::remove(string code)
{
    root = removeHelper(root, code);
}

Airport* AirportBST::removeHelper(Airport* node, string code)
{
    // Target not found
    if (node == nullptr)
    {
        return nullptr;
    }

    // Search left subtree
    if (code < node->IATA)
    {
        node->left = removeHelper(node->left, code);
    }
    // Search right subtree
    else if (code > node->IATA)
    {
        node->right = removeHelper(node->right, code);
    }
    // Target found
    else
    {
        // D1: Leaf Node
        if (node->left == nullptr && node->right == nullptr)
        {
            cout << "Deleting leaf node: " << node->IATA << endl;
            delete node;
            return nullptr;
        }
        // D2: Only left child
        else if (node->right == nullptr)
        {
            Airport* child = node->left;

            cout << "Deleting node with only left child: " 
                 << node->IATA << endl;
            
            delete node;
            return child;
        }
        // D2: Only right child
        else if (node->left == nullptr)
        {
            Airport* child = node->right;

            cout << "Deleting node with only right child: " 
                 << node->IATA << endl;

            delete node;
            return child;
        }
        else
        {
            // D3: Node has two children
            Airport* successor = findMinNode(node->right);

            cout << "Deleting node with two children: " 
                 << node->IATA << endl;

            cout << "Inorder successor: " << successor->IATA << endl;

            // Copy airport info, not child pointers
            node->name = successor->name;
            node->city = successor->city;
            node->country = successor->country;
            node->IATA = successor->IATA;
            node->ICAO = successor->ICAO;
            node->latitude = successor->latitude;
            node->longitude = successor->longitude;

            // Delete original successor node
            node->right = removeHelper(node->right, successor->IATA);
        }
    }

    return node;
}

string AirportBST::findOnlyRightChild()
{
    if (root == nullptr)
    {
        return "";
    }

    queue<Airport*> q;
    q.push(root);

    while (!q.empty())
    {
        Airport* cur = q.front();
        q.pop();

        if (cur->left == nullptr && cur->right != nullptr)
        {
            return cur->IATA;
        }

        if (cur->left != nullptr)
        {
            q.push(cur->left);
        }

        if (cur->right != nullptr)
        {
            q.push(cur->right);
        }
    }

    return "";
}

Airport* AirportBST::findMinNode(Airport* node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    Airport* cur = node;

    while (cur->left != nullptr)
    {
        cur = cur->left;
    }

    return cur;
}

string AirportBST::findTwoChildren()
{
    if (root == nullptr)
    {
        return "";
    }

    queue<Airport*> q;
    q.push(root);

    while (!q.empty())
    {
        Airport* cur = q.front();
        q.pop();

        if (cur->left != nullptr && cur->right != nullptr)
        {
            return cur->IATA;
        }

        if (cur->left != nullptr)
        {
            q.push(cur->left);
        }

        if (cur->right != nullptr)
        {
            q.push(cur->right);
        }
    }

    return "";
}

void AirportBST::printLevelOrder(int maxLevels)
{
    if (root == nullptr)
    {
        cout << "Tree is empty" << endl;
        return;
    }

    queue<Airport*> q;
    q.push(root);

    int level = 0;

    while (!q.empty() && level < maxLevels)
    {
        int levelSize = q.size();

        cout << "Level " << level << ": ";

        for (int i = 0; i < levelSize; i++)
        {
            Airport* cur = q.front();
            q.pop();

            cout << cur->IATA << " ";

            if (cur->left != nullptr)
            {
                q.push(cur->left);
            }

            if (cur->right != nullptr)
            {
                q.push(cur->right);
            }
        }

        cout << endl;
        level++;
    }
}

bool AirportBST::validate()
{
    return validateHelper(root, "", "");
}

bool AirportBST::validateHelper(Airport* node, string lo, string hi)
{
    if (node == nullptr)
    {
        return true;
    }

    if (!lo.empty() && node->IATA <= lo)
    {
        return false;
    }

    if (!hi.empty() && node->IATA >= hi)
    {
        return false;
    }

    return validateHelper(node->left, lo, node->IATA) &&
           validateHelper(node->right, node->IATA, hi);
}

//=============================================================
// TASK 4
//=============================================================
void AirportBST::inorderHelper(Airport* node, ofstream& file)
{
    // Base case
    if (node == nullptr)
    {
        return;
    }

    // Visit left subtree
    inorderHelper(node->left, file);

    // Print current airport
    file << node->IATA
         << " | " << node->name
         << " | " << node->city
         << " | " << node->country
         << endl;
    
    // Visit right subtree
    inorderHelper(node->right, file);
}

void AirportBST::saveInorder(string filename)
{
    ofstream file(filename);

    if (!file.is_open())
    {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    inorderHelper(root, file);

    file.close();

    cout << "inorder traversal saved to " << filename << endl;
}

void AirportBST::preorderHelper(Airport* node)
{
    if (node == nullptr)
    {
        return;
    }

    cout << node->IATA << " ";

    preorderHelper(node->left);
    preorderHelper(node->right);
}

void AirportBST::printPreorder()
{
    preorderHelper(root);
    cout << endl;
}

void AirportBST::savePreorderHelper(Airport* node, ofstream& file)
{
    if (node == nullptr)
    {
        return;
    }

    file << node->IATA << endl;

    savePreorderHelper(node->left, file);
    savePreorderHelper(node->right, file);
}

void AirportBST::savePreorder(string filename)
{
    ofstream file(filename);

    if (!file.is_open())
    {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    savePreorderHelper(root, file);

    file.close();
    
    cout << "Preorder traversal saved to " << filename << endl;
}

void AirportBST::loadPreorder(string filename, const AirportBST& source)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error opening preorder file: " << filename << endl;
        return;
    }

    // Prevent rebuilding a tree from itself
    if (this == &source)
    {
        cout << "Error: Source and destination must be different" << endl;
        return;
    }

    clearTree();
    records.clear();

    string code;
    int loaded = 0;
    int missing = 0;

    while (file >> code)
    {
        bool found = false;

        for (int i = 0; i < static_cast<int>(source.records.size()); i++)
        {
            const Airport& airport = source.records[i];
            if (airport.IATA == code)
            {
                Airport* newAirport = new Airport(
                    airport.name,
                    airport.city,
                    airport.country,
                    airport.IATA,
                    airport.ICAO,
                    airport.latitude,
                    airport.longitude
                );

                if (insert(newAirport))
                {
                    records.emplace_back(
                        airport.name,
                        airport.city,
                        airport.country,
                        airport.IATA,
                        airport.ICAO,
                        airport.latitude,
                        airport.longitude
                    );

                    loaded++;
                }
                else
                {
                    // Assumes insert() deletes rejected nodes
                    cout << "Duplicate IATA: " << code << endl;
                }

                found = true;
                break;
            }
        }

        if (!found)
        {
            cout << "Missing airport record: "
                 << code << endl;
            
            missing++;
        }
    }

    file.close();

    cout << "Reconstructed airport: " << loaded << endl;
    cout << "Missing airport records: " << missing << endl;
}

//=============================================================
// TASK 5
//=============================================================
void AirportBST::rangeHelper(Airport* node, string from, string to, 
                             int& visited, int& found)
{
    if (node == nullptr)
    {
        return;
    }

    visited++;

    // Explore left subtree only when necessary
    if (node->IATA > from)
    {
        rangeHelper(node->left, from, to, visited, found);
    }

    // Check whether current airport is in range
    if (node->IATA >= from && node->IATA <= to)
    {
        cout << node->IATA << " - "
             << node->name << endl;

        found++;
    }

    // Explore right subtree only when necessary
    if (node->IATA < to)
    {
        rangeHelper(node->right, from, to, visited, found);
    }
}

void AirportBST::rangeQuery(string from, string to)
{
    if (from > to)
    {
        cout << "Invalid range: lower bound exceeds upper bound"
             << endl;

        return;
    }

    int visited = 0;
    int found = 0;

    cout << "\nAirports from " << from
         << " to " << to << ":" << endl;

    rangeHelper(root, from, to, visited, found);

    cout << "\nMatching airports: " << found << endl;
    cout << "Nodes visited: " << visited << endl;
}

//=============================================================
// TASK 6
//=============================================================
const vector<Airport>& AirportBST::getRecords() const
{
    return records;
}

void AirportBST::buildSorted(const vector<Airport>& airports)
{
    // Copy records before clearing the destination tree
    vector<Airport> sortedAirports = airports;

    clearTree();
    records.clear();

    sort(sortedAirports.begin(), sortedAirports.end(),
        [](const Airport& a, const Airport& b)
        {
            return a.IATA < b.IATA;
        });

    for (int i = 0; i < static_cast<int>(sortedAirports.size()); i++)
    {
        const Airport& airport = sortedAirports[i];
        Airport* newAirport = new Airport(
            airport.name,
            airport.city,
            airport.country,
            airport.IATA,
            airport.ICAO,
            airport.latitude,
            airport.longitude
        );

        if (insert(newAirport))
        {
            records.emplace_back(
                airport.name,
                airport.city,
                airport.country,
                airport.IATA,
                airport.ICAO,
                airport.latitude,
                airport.longitude
            );
        }

    }
}

Airport* AirportBST::balancedHelper(
    const vector<Airport>& airports,
    int left,
    int right)
{
    if (left > right)
    {
        return nullptr;
    }

    int mid = left + (right - left) / 2;

    const Airport& airport = airports[mid];

    Airport* node = new Airport(
        airport.name,
        airport.city,
        airport.country,
        airport.IATA,
        airport.ICAO,
        airport.latitude,
        airport.longitude
    );

    node->left = balancedHelper(airports, left, mid - 1);
    node->right = balancedHelper(airports, mid + 1, right);

    return node;
}

void AirportBST::buildBalanced(const vector<Airport>& airports)
{
    vector<Airport> sortedAirports = airports;

    sort(sortedAirports.begin(), sortedAirports.end(),
        [](const Airport& a, const Airport& b)
        {
            return a.IATA < b.IATA;
        });
    
    clearTree();
    records.clear();

    for (int i = 0; i < static_cast<int>(sortedAirports.size()); i++)
    {
        const Airport& airport = sortedAirports[i];
        records.emplace_back(
            airport.name, airport.city, airport.country,
            airport.IATA, airport.ICAO,
            airport.latitude, airport.longitude
        );
    }

    root = balancedHelper(sortedAirports, 0, 
        static_cast<int>(sortedAirports.size()) - 1);
}

Airport* AirportBST::searchSilent(const string& code) const
{
    Airport* current = root;

    while (current != nullptr)
    {
        if (code == current->IATA)
        {
            return current;
        }
        else if (code < current->IATA)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }

    return nullptr;
}

double benchmarkSearch(
    const AirportBST& tree,
    const vector<string>& queries)
{
    int found = 0;

    auto start = chrono::steady_clock::now();

    for (int i = 0; i < static_cast<int>(queries.size()); i++)
    {
        const string& code = queries[i];
        if (tree.searchSilent(code) != nullptr)
        {
            found++;
        }
    }

    auto end = chrono::steady_clock::now();

    chrono::duration<double, milli> elapsed = end - start;

    cout << "Successful searches: " << found << endl;

    return elapsed.count();
}

//=============================================================
// MAIN
//=============================================================


int main()
{
    AirportBST tree;

    // =========================================================
    // TASK 1: LOAD AIRPORT DATA AND TREE METRICS
    // =========================================================
    cout << "========== TASK 1: LOAD DATA ==========" << endl;

    tree.loadAirports("airports.dat");

    cout << "BST Validation: "
         << (tree.validate() ? "VALID" : "INVALID") << endl;
    cout << "Tree Height: " << tree.height() << endl;
    cout << "Average Depth: " << tree.averageDepth() << endl;


    // =========================================================
    // TASK 2: SEARCH OPERATIONS
    // =========================================================
    cout << "\n========== TASK 2: SEARCH ==========" << endl;

    cout << "\nSearching for BOS:" << endl;
    Airport* result = tree.search("BOS");

    if (result != nullptr)
    {
        cout << "Airport Found!" << endl;
        cout << "IATA: " << result->IATA << endl;
        cout << "Name: " << result->name << endl;
        cout << "City: " << result->city << endl;
        cout << "Country: " << result->country << endl;
        cout << "ICAO: " << result->ICAO << endl;
        cout << "Latitude: " << result->latitude << endl;
        cout << "Longitude: " << result->longitude << endl;
    }

    cout << "\nSearching for missing airport XYZ:" << endl;
    if (tree.search("XYZ") == nullptr)
    {
        cout << "XYZ not found!" << endl;
    }

    string minCode = tree.findMin();
    string maxCode = tree.findMax();

    cout << "\nMinimum IATA Code: " << minCode << endl;
    cout << "Maximum IATA Code: " << maxCode << endl;

    cout << "\nSearching for minimum:" << endl;
    tree.search(minCode);

    cout << "\nSearching for maximum:" << endl;
    tree.search(maxCode);

    cout << "\nSearching below minimum (000):" << endl;
    tree.search("000");

    cout << "\nSearching above maximum (ZZZ):" << endl;
    tree.search("ZZZ");

    cout << "\nSearching for root (GKA):" << endl;
    tree.search("GKA");

    cout << "\nSearching for deep node (ZZV):" << endl;
    tree.search("ZZV");

    string leafCode = tree.findLeaf();

    if (!leafCode.empty())
    {
        cout << "\nSearching for leaf (" << leafCode << "):" << endl;
        tree.search(leafCode);
    }


    // =========================================================
    // TASK 3: BST DELETION CASES
    // =========================================================
    cout << "\n========== TASK 3: DELETION ==========" << endl;

    // D1: Delete a leaf
    cout << "\n--- D1: Leaf Node (AAC) ---" << endl;
    cout << "Before deletion:" << endl;
    tree.search("AAC");
    tree.printLevelOrder();

    tree.remove("AAC");

    cout << "\nAfter deletion:" << endl;
    tree.search("AAC");
    tree.printLevelOrder();

    cout << "Deletion: "
         << (tree.searchSilent("AAC") == nullptr ? "SUCCESS" : "FAILED")
         << endl;
    cout << "BST Validation: "
         << (tree.validate() ? "VALID" : "INVALID") << endl;


    // D2a: Delete a node with only a left child
    cout << "\n--- D2a: Only Left Child (BOS) ---" << endl;
    cout << "Before deletion:" << endl;
    tree.search("BOS");
    tree.printLevelOrder();

    tree.remove("BOS");

    cout << "\nAfter deletion:" << endl;
    tree.search("BOS");
    tree.printLevelOrder();

    cout << "Replacement BOR: "
         << (tree.searchSilent("BOR") != nullptr ? "FOUND" : "MISSING")
         << endl;
    cout << "BST Validation: "
         << (tree.validate() ? "VALID" : "INVALID") << endl;


    // D2b: Delete a node with only a right child
    cout << "\n--- D2b: Only Right Child ---" << endl;

    string rightOnlyCode = tree.findOnlyRightChild();

    if (!rightOnlyCode.empty())
    {
        cout << "Selected airport: " << rightOnlyCode << endl;

        cout << "\nBefore deletion:" << endl;
        tree.search(rightOnlyCode);
        tree.printLevelOrder();

        tree.remove(rightOnlyCode);

        cout << "\nAfter deletion:" << endl;
        tree.search(rightOnlyCode);
        tree.printLevelOrder();

        cout << "Deletion: "
             << (tree.searchSilent(rightOnlyCode) == nullptr
                 ? "SUCCESS" : "FAILED") << endl;
        cout << "BST Validation: "
             << (tree.validate() ? "VALID" : "INVALID") << endl;
    }


    // D3: Delete a node with two children
    cout << "\n--- D3: Two Children ---" << endl;

    string twoChildCode = "AEY";

    if (!twoChildCode.empty())
    {
        cout << "Selected airport: " << twoChildCode << endl;

        cout << "\nBefore deletion:" << endl;
        tree.search(twoChildCode);
        tree.printLevelOrder();

        tree.remove(twoChildCode);

        cout << "\nAfter deletion:" << endl;
        tree.search(twoChildCode);
        tree.printLevelOrder();

        cout << "Deletion: "
             << (tree.searchSilent(twoChildCode) == nullptr
                 ? "SUCCESS" : "FAILED") << endl;
        cout << "BST Validation: "
             << (tree.validate() ? "VALID" : "INVALID") << endl;
    }


    // Insert and delete a temporary airport
    cout << "\n--- QQQ Insertion and Deletion ---" << endl;

    Airport* fakeAirport = new Airport(
        "Test Airport", "Test City", "Test Country",
        "QQQ", "TQQQ", 42.0, -71.0
    );

    if (tree.insert(fakeAirport))
    {
        cout << "QQQ inserted successfully!" << endl;

        cout << "BST Validation after insertion: "
             << (tree.validate() ? "VALID" : "INVALID")
             << endl;

        cout << "\nSearching for QQQ:" << endl;
        tree.search("QQQ");

        cout << "\nBefore deletion:" << endl;
        tree.printLevelOrder();

        tree.remove("QQQ");

        cout << "\nAfter deletion:" << endl;
        tree.printLevelOrder();

        cout << "QQQ Deletion: "
             << (tree.searchSilent("QQQ") == nullptr
                 ? "SUCCESS" : "FAILED") << endl;
        cout << "BST Validation: "
             << (tree.validate() ? "VALID" : "INVALID") << endl;
    }
    else
    {
        cout << "QQQ insertion failed!" << endl;
    }

    // D4: Delete the root node
    cout << "\n--- D4: Root Deletion (GKA) ---" << endl;

    // Create a fresh tree to preserve the original root
    AirportBST rootTest;
    rootTest.loadAirports("airports.dat");

    cout << "\nBefore root deletion:" << endl;
    rootTest.printLevelOrder();

    // Confirm that GKA exists
    cout << "\nSearching for root GKA:" << endl;
    rootTest.search("GKA");

    // Delete the root
    cout << "\nDeleting root GKA:" << endl;
    rootTest.remove("GKA");

    cout << "\nAfter root deletion:" << endl;
    rootTest.printLevelOrder();

    // Confirm the old root no longer exists
    cout << "\nSearching for deleted root GKA:" << endl;
    Airport* rootResult = rootTest.search("GKA");

    if (rootResult == nullptr)
    {
        cout << "Root deletion successful!" << endl;
    }
    else
    {
        cout << "ERROR: GKA still exists!" << endl;
    }

    // Validate the BST after deletion
    cout << "BST Validation: "
         << (rootTest.validate() ? "VALID" : "INVALID")
         << endl;

    // =========================================================
    // TASK 4: TRAVERSALS AND RECONSTRUCTION
    // =========================================================
    cout << "\n========== TASK 4: TRAVERSALS ==========" << endl;

    // Use a fresh BST so deletion tests do not change traversal results
    AirportBST originalTree;
    originalTree.loadAirports("airports.dat");

    originalTree.saveInorder("inorder.txt");
    originalTree.savePreorder("preorder.txt");

    AirportBST reconstructed;
    reconstructed.loadPreorder("preorder.txt", originalTree);
    reconstructed.savePreorder("preorder_reconstructed.txt");

    cout << "\nOriginal Height: " << originalTree.height() << endl;
    cout << "Reconstructed Height: " << reconstructed.height() << endl;

    cout << "\nOriginal Average Depth: "
         << originalTree.averageDepth() << endl;
    cout << "Reconstructed Average Depth: "
         << reconstructed.averageDepth() << endl;

    cout << "\nOriginal Top Three Levels:" << endl;
    originalTree.printLevelOrder();

    cout << "\nReconstructed Top Three Levels:" << endl;
    reconstructed.printLevelOrder();

    cout << "\nReconstructed BST Validation: "
         << (reconstructed.validate() ? "VALID" : "INVALID")
         << endl;


    // =========================================================
    // TASK 5: RANGE QUERY
    // =========================================================
    cout << "\n========== TASK 5: RANGE QUERY ==========" << endl;

    originalTree.rangeQuery("BOS", "BWI");


    // =========================================================
    // TASK 6: TREE CONSTRUCTION AND BENCHMARKING
    // =========================================================
    cout << "\n========== TASK 6: TREE COMPARISON ==========" << endl;

    AirportBST sortedTree;
    AirportBST balancedTree;

    const vector<Airport>& airports = originalTree.getRecords();

    sortedTree.buildSorted(airports);
    balancedTree.buildBalanced(airports);

    cout << "\nFile-Order BST:" << endl;
    cout << "Height: " << originalTree.height() << endl;
    cout << "Average Depth: " << originalTree.averageDepth() << endl;
    cout << "Validation: "
         << (originalTree.validate() ? "VALID" : "INVALID") << endl;

    cout << "\nSorted-Order BST:" << endl;
    cout << "Height: " << sortedTree.height() << endl;
    cout << "Average Depth: " << sortedTree.averageDepth() << endl;
    cout << "Validation: "
         << (sortedTree.validate() ? "VALID" : "INVALID") << endl;

    cout << "\nBalanced BST:" << endl;
    cout << "Height: " << balancedTree.height() << endl;
    cout << "Average Depth: " << balancedTree.averageDepth() << endl;
    cout << "Validation: "
         << (balancedTree.validate() ? "VALID" : "INVALID") << endl;

    cout << "\nBalanced Tree Top Three Levels:" << endl;
    balancedTree.printLevelOrder();


    // Generate 10,000 reproducible search queries
    vector<string> queries;
    mt19937 rng(42);

    if (!airports.empty())
    {
        uniform_int_distribution<int> distribution(
            0, static_cast<int>(airports.size()) - 1
        );

        for (int i = 0; i < 10000; i++)
        {
            queries.push_back(airports[distribution(rng)].IATA);
        }

        cout << "\n--- SEARCH BENCHMARK (10,000 Queries) ---" << endl;

        double fileTime = benchmarkSearch(originalTree, queries);
        double sortedTime = benchmarkSearch(sortedTree, queries);
        double balancedTime = benchmarkSearch(balancedTree, queries);

        cout << "\nFile-Order BST: " << fileTime << " ms" << endl;
        cout << "Sorted-Order BST: " << sortedTime << " ms" << endl;
        cout << "Balanced BST: " << balancedTime << " ms" << endl;
    }

    cout << "\n========== ALL TASKS COMPLETE ==========" << endl;

    return 0;
}