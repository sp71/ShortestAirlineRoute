/*
 Satinder Singh
 
 Finds shortest path between any two given cities within file given.
 Lists all possible cities user is interested from substring of city name.
 
 */

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <cstdlib>
#include <fstream>
using namespace std;

const int SIZE = 150;

struct map{
    map();
    string city;
    map * next;
    int depth;
    int dist;
};

map::map(){
    depth = 0;
    next = NULL;
    dist = 0;
}

vector <string> findSub(const vector<map*> &orig, string sub){
    vector<string> v;
    for (unsigned int i=0; i < SIZE; i++)
        if ( orig[i]->city.find(sub) != -1)
            v.push_back(orig[i]->city);
    return v;
}

void insert(map *traverse, string _city){
    unsigned int letter, lastNum, distance=0;
    for(letter = lastNum = _city.size()-1; !isalpha(_city[letter]); letter--){
        if( isdigit(_city[letter]) )
            lastNum = letter;
    }
    if( letter < _city.size()-1 ){
        distance = atoi(_city.substr(lastNum).c_str()); // place the mileage to the next empty To
        _city.erase(letter+1, _city.size());
    }
    
    map *node = new map;
    node->city = _city;
    node->next = NULL;
    node->depth = 0;
    while (traverse->next != NULL)
        traverse = traverse->next;
    traverse->next = node;
    node->dist = distance;
}

int indexFind(vector<map*> air, string s){
    for (unsigned int i=0; i < SIZE; i++)
        if ( air[i]->city == s )
            return i;
    return -1; // did not find string
}

// must pass vectors by reference
vector<map*> input(){
    ifstream fin;
    string fileName;
    cout << "Enter File Name: ";
    cin >> fileName;
    fin.open(fileName.c_str());
    if (!fin) {
        cout << "File Input Error";
        exit(0);
    }
    vector<map*> air(SIZE);
    for (int i=0; i < SIZE; i++) // constructor initalizes
        air[i] = new map;
    string f, f2;
    char d; // skips spaces and captures first characters
    int i=-1, index;
    while ( fin >> f ){
        if ( f == "From:"){
            i++;
            fin >> d;
            getline(fin, f); // this without fin >> d would capture spaces before
            air[i]->city = d+f;
        }
        else if (f == "To"){
            fin >> d >> d; // kill colon and capture first char
            getline(fin,f);
            insert(air[i], d+f);
        }
        else{
            getline(fin, f2); // extra To cities
            insert(air[i], f+f2);
        }
    }
    fin.close();
    fin.open("connections.txt");
    bool first = true;
    while (fin >> f){
        if (f == "To"){
            fin >> d >> d; // kill colon and capture first char
            while (f != "From:" && !fin.eof()){
                if (!first) fin >> d;
                getline(fin,f);
                f =d + f;
                if (f.substr(0,5) == "From:"){
                    break;
                }
                index = indexFind(air, f);
                if ( index == -1){
                    air[++i]->city = f;
                    air[i]->next = NULL;
                }
                first = false;
            }
            first = true;
        }
    }
    return air;
}

string userInput(string s){
    string input;
    do{
        cout << s <<" (city Name) or substring (length > 2): ";
        getline(cin, input);
        if (input.size() < 3) cout << "\nSorry - not long Enough\n";
    }while ( input.size() < 3);
    return input;
}

string print(vector<string> v){
    unsigned int x;
    for (unsigned int i=0; i < v.size(); i++)
        cout << i+1 << " :" << v[i] << endl;
    do{
        cout << "Choose valid #: ";
        cin >> x;
    }while (x > v.size() || x < 1);
    cin.ignore();
    return v[x-1];
}

int goAgain(){
    int x;
    cout << "\nContinue? (1  or 0): ";
    cin >> x;
    cin.ignore();
    return x;
}

string ask(vector<string> &list, const vector<map*> &allFrom, string msg){
    list.clear();
    while (list.empty()){
        list = findSub(allFrom, userInput(msg));
        if ( list.empty() ){
            cout << "Sorry - No results...Try again\n";
        }
    }
    return print(list);
}

void insertQ( map* traverse, queue <map *> &q, string to){
    while (traverse != NULL && traverse->city != to){
        q.push(traverse);
        traverse = traverse->next;
    }
}

void parentFinder(queue <map *> & q, map *kid){
    while (q.front() != kid ){
        q.pop();
    }
}

bool isKid( map * tempo, map* kid, vector<map*> air){
    int in = indexFind(air,tempo->city);
    map * t = air[in]->next;
    while (t != NULL){
        if (t->city == kid->city)
            return true;
        t = t->next;
    }
    return false;
}

void bfs(vector<map*> air, string from, string to){
    queue <map*> q;
    stack <map*> stk;
    map* list, kid, *tempo;
    stack <string> printer;
    int inFrom = indexFind(air, from), depth = 1, toIndex;
    stk.push(air[inFrom]);
    if (stk.top()->next == NULL) {
        cout << "Cannot find Anything";
        return;
    }
    q.push(air[inFrom]);
    list = q.front();
    while (stk.top()->city != to){
        inFrom = indexFind(air, list->city);
        map* temp = list->next;
        while (list->next != NULL && list->city != to){
            list->next->depth = depth;
            stk.push(list->next);
            q.push(list->next);
            list = list->next;
        }
        q.pop();
        depth = q.front()->depth + 1;
        if (depth == 50){
            cout << " Theres no connections!" << endl;
            return;
        }
        toIndex = indexFind(air, q.front()->city);
        list = air[toIndex];
    }
    tempo = stk.top();
    printer.push(tempo->city);
    while (!stk.empty()){
        int temp = tempo->depth;
        --temp;
        if ((temp) == stk.top()->depth && isKid(stk.top(),tempo, air)){
            printer.push(stk.top()->city);
            tempo = stk.top();
        }
        stk.pop();
    }
    while (!printer.empty()){
        cout << printer.top();
        if (printer.size() != 1 && depth != 0)
        {
            cout << " -> ";
        }
        printer.pop();
    }
    cout << endl;
}

int main()
{
    vector<map*> allFrom = input();
    string from, to;
    vector<string> fromList, toList;
    cin.ignore();
    do
    {
        from = ask(fromList, allFrom, "From");
        to = ask(toList, allFrom, "To");
        bfs(allFrom, from, to);
    }while ( goAgain() );
    return 0;
}

