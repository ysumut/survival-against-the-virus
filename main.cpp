/*
Yusuf Umut Bulak
*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

int map_size, turn_count;
vector<vector<string>> entityInfo, turnInfo;
string eachTurnForFile = "";

vector<string> split(string s, char delimeter) {
    vector<string> split_array;
    string item = "";

    for (int i = 0; i < s.length(); i++) {
        
        if (s[i] != delimeter) {
            item += s[i];
        }
        if (s[i] == delimeter || i == s.length() -1) {
            split_array.push_back(item);
            item = "";
        }
    }

    return split_array;
}

void readInputFile() {
    string inputText;
    ifstream inputTextFile("input.txt");

    int i = 0;
    while (getline(inputTextFile, inputText)) {
        vector<string> split_arr = split(inputText, ' ');

        if (split_arr[0] == "size")
            map_size = stoi(split_arr[1]);

        if (split_arr[0] == "turn_count")
            turn_count = stoi(split_arr[1]);

        if (split_arr[0] == "entity") {
            if(split_arr.size() == 5)
                entityInfo.push_back({ split_arr[1], split_arr[2], split_arr[3], split_arr[4] });
            else 
                entityInfo.push_back({ split_arr[1], split_arr[2], split_arr[3], "" });
        }

        if (split_arr[0] == "turn") {
            vector<string> turn_item;

            for (int j = 0; j < split_arr.size(); j++) {
                if(j != 0)
                    turn_item.push_back(split_arr[j]);
            }
            turnInfo.push_back(turn_item);
        }

        i++;
    }

    inputTextFile.close();
}


class Entity {
private:
    int counter_infection = 0, counter_death = 0, counter_disappear = 0;

public:
    int id, location[2];
    string gates;
    bool is_infected = false, is_dead = false, is_immunity = false, is_disappear = false;
    bool wait_infection = false;

    void getInfo() {
        cout << "\n";
        cout << "id: " << id << "\n";
        cout << "gates: " << gates << "\n";
        cout << "location: " << location[0] << "x" << location[1] << "\n";
        cout << "isInfected: " << is_infected << "\n";
        cout << "isDead: " << is_dead << "\n";
        cout << "isImmunity: " << is_immunity << "\n";
        cout << "isDisappear: " << is_disappear << "\n\n";
    }

    bool canDie() {
        int count = 0;

        for (char each : gates) {
            if (each == 'A' || each == 'B' || each == 'C' || each == 'D') count++;
        }

        if (count >= 3) return true;
        else return false;
    }

    bool canInfect() { // enfekte olabilir mi
        bool it_can = false;

        for (char each : gates) {
            if (each == 'A' || each == 'B') it_can = true;
        }
        return it_can;
    }

    void infectionCounter() {
        if (wait_infection && !is_immunity) {
            // Bağışıklık kazanmış ise enfekte olmayacak

            if (++counter_infection == 3) {
                is_infected = true;
            }
        }
    }

    void deathCounter() {
        if (is_infected) {
            counter_death++;

            if (counter_death == 14 && canDie()) {
                is_dead = true;
            }
            if (counter_death == 30 && !canDie()) {
                is_immunity = true;
                is_infected = false;
            }
        }
    }

    void disappearCounter() {
        if (is_dead) {
            
            if (++counter_disappear == 5) {
                is_disappear = true;
            }
        }
    }

    void INFECT(Entity& e) {
        double xDistance = abs(this->location[0] - e.location[0]); 
        double yDistance = abs(this->location[1] - e.location[1]);
        
        if (xDistance <= 3 && yDistance <=3) {
            if (this->is_infected && e.canInfect()) // entity1 enfekte ise ve entity2 enfekte olabiliyor ise
                e.wait_infection = true;

            if (e.is_infected && this->canInfect())
                this->wait_infection = true;
        }
    }
};

vector<Entity> createEntity() {
    vector<Entity> allEntities;

    for (int i = 0; i < entityInfo.size(); i++) {
        Entity e;
        
        e.id = stoi(entityInfo[i][0]);
        e.gates = entityInfo[i][1];

        vector<string> location = split(entityInfo[i][2], 'x');
        e.location[0] = stoi(location[0]);
        e.location[1] = stoi(location[1]);

        if (entityInfo[i][3] == "infected") e.is_infected = true;

        allEntities.push_back(e);
    }

    return allEntities;
}

void writeFileEachTurn(vector<Entity> allEntities, int turn_number) {
    vector<string> map(map_size);
    
    for (int i = 0; i < map_size; i++) {
        map[i] = "-";

        for (int j = 0; j < map_size; j++) {
            map[i] += " ";
        }

        map[i] += "-";
    }
    
    for (Entity e : allEntities) {
        if (e.is_disappear) continue;

        int x = e.location[0];
        int y = e.location[1];

        char code = 'O';

        if (e.is_dead) code = 'D';
        else if (e.is_infected) code = 'X';
        
        if (e.is_immunity) code = 'O';

        map[x - 1][y] = code;
    }

    string addLines = "";
    for (int i = 0; i < map_size + 2; i++)
        addLines += "-";

    string addMap = "";
    for (string each : map)
        addMap += each + "\n";


    
    string new_line = "\n";
    if (turn_count == turn_number) new_line = "";

    eachTurnForFile += "Turn " + to_string(turn_number) + ":\n" + addLines + "\n" + addMap + addLines + new_line;

    // Dosya Yazma
    if (turn_count == turn_number) {
        ofstream writeTurnsFile("turns.txt");

        writeTurnsFile << eachTurnForFile;
        writeTurnsFile.close();
    }
}

void writeOutputFile(vector<Entity> allEntities) {
    int normal = 0, infected = 0, dead = 0, recovered = 0;
    string entities = "";

    int i = 0;
    for (Entity e : allEntities) {
        string infection_result = "";
        
        if (e.is_immunity) {
            recovered++;
            infection_result = " recovered";
        }
        else if (!e.is_infected) {
            normal++;
            infection_result = " normal";
        }
        else if (e.is_dead) {
            dead++;
            infection_result = " dead";
        }
        else if (e.is_infected) {
            infected++;
            infection_result = " infected";
        }

        string new_line = "\n";
        if (++i == allEntities.size()) new_line = "";

        entities += "entity " + to_string(e.id) + " " + to_string(e.location[0]) + "x" + to_string(e.location[1]) + 
            infection_result + new_line;
    }

    // Dosya Yazma
    ofstream writeFile("output.txt");

    writeFile << "Normal   : " << normal << "\nInfected : " << infected << "\nDead     : " << dead << "\nRecovered: " << recovered
        << "\n" << entities ;

    writeFile.close();
}

void turn_process() {
    vector<Entity> allEntities = createEntity();

    /* Başlangıç konumlarına göre kontrol yapılır.
    for (int i = 0; i < allEntities.size(); i++) {
        for (int j = 0; j < allEntities.size(); j++) {
            if (i >= j) continue;

            Entity& e1 = allEntities[i];
            Entity& e2 = allEntities[j];

            e1.INFECT(e2);
        }
    }*/
    

    for (vector<string> each_turn : turnInfo) {
        
        // Yeni konumları belirlenir ve entity'lerin son durumları kontrol edilir.
        for (Entity& e : allEntities) {
            e.disappearCounter();
            e.deathCounter();
            e.infectionCounter();
            
            if (!e.is_dead) { // eğer ölmemiş ise konum değiştir
                vector<string> location = split(each_turn[e.id], 'x');
                e.location[0] = stoi(location[0]);
                e.location[1] = stoi(location[1]);
            }
        }

        for (int i = 0; i < allEntities.size(); i++) {
            for (int j = 0; j < allEntities.size(); j++) {
                
                Entity& e1 = allEntities[i];
                Entity& e2 = allEntities[j];
                
                if (i >= j || e1.is_disappear || e2.is_disappear) continue;  // İki entity'den biri bile yok olmuş ise, INFECT yapmıyoruz.

                e1.INFECT(e2);
            }
        }

        writeFileEachTurn(allEntities, stoi(each_turn[0]));
    }

    writeOutputFile(allEntities);

    for (Entity e : allEntities) {
        e.getInfo();
    }
}

int main()
{
    readInputFile();
    
    cout << "map_size: " << map_size << "\n";
    cout << "turn_count: " << turn_count << "\n";

    turn_process();

    return 0;
}