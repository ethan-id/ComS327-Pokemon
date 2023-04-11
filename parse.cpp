#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <sys/stat.h>
#include <bits/stdc++.h>
#include "parse.h"
using namespace std;

int dataType;
const char* validNames[10] = {"pokemon", "moves", "pokemon_moves", "pokemon_species", "experience", "type_names", "pokemon_stats", "stats", "pokemon_types"};

class pokemonStats {
    public:
        int id;
        int stat_id;
        int base_stat;
        int effort;
};

class stats {
    public:
        string id;
        string damage_class_id;
        string identifier;
        string is_battle_only;
        string game_index;
};

class typeNames {
    public:
        int type_id;
        int local_language_id;
        string name;
};

class experience {
    public:
        int growth_rate_id;
        int level;
        int experience;
};

class moves {
    public:
        int id;
        string identifier;
        int generation_id;
        int type_id;
        int power;
        int pp;
        int accuracy;
        int priority;
        int target_id;
        int damage_class_id;
        int effect_id;
        int effect_chance;
        int contest_type_id;
        int contest_effect_id;
        int super_contest_effect_id;
};

class pokemonMoves {
    public:
        int pokemon_id;
        int version_group_id;
        int move_id;
        int pokemon_move_method_id;
        int level;
        int order;
};

class pokemonSpecies {
    public:
        int id;
        string identifier;
        int generation_id;
        int evolves_from_species_id;
        int evolution_chain_id;
        int color_id;
        int shape_id;
        int habitat_id;
        int gender_rate;
        int capture_rate;
        int base_happiness;
        int is_baby;
        int hatch_counter;
        int has_gender_differences;
        int growth_rate_id;
        int forms_switchable;
        int is_legendary;
        int is_mythical;
        int order;
        int conquest_order;
};

class pokemonTypes {
    public:
        int pokemon_id;
        int type_id;
        int slot;
};

class pokemon {
    public:
        int id;
        string identifier;
        int species_id;
        int height;
        int weight;
        int base_experience;
        int order;
        int is_default;
};

int verifyFileName(const char* fileName) {
    int retVal = 0;

    for (int i = 0; i < 9; i++) {
        if (strcmp(fileName, validNames[i]) == 0) {
            retVal = 1;
            dataType = i;
        }
    }

    return retVal;
}

int parseFile(const char* fileName) {
    string file = fileName;

    string path1 = "/share/cs327/pokedex/pokedex/data/csv/";
    string path2 = getenv("HOME");
    path2 += "/.poke327/pokedex/pokedex/data/csv/";

    struct stat sb;

    if (stat("/share/cs327/pokedex/pokedex/data/csv/", &sb) == 0) {
        readFile(path1 + file + ".csv");
    } else if (stat(strcat(getenv("HOME"), "/.poke327/pokedex/pokedex/data/csv/"), &sb) == 0) {
        readFile(path2 + file + ".csv");
    } else {
        cout << "Database not found";
    }

    return 0;
}

void readFile(string fileName) {
    ifstream csvFile;
    csvFile.open(fileName);

    std::string segment;
    std::vector<std::string> seglist;

    getline(csvFile, segment);

    if(strcmp(validNames[dataType], "stats") == 0) {
        std::vector<stats> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            stats statsObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        statsObj.id = token;
                        break;
                    case 1 :
                        statsObj.damage_class_id = token;
                        break;
                    case 2 :
                        statsObj.identifier = token;
                        break;
                    case 3 :
                        statsObj.is_battle_only = token;
                        break;
                }
                elementPos++;
            }
            statsObj.game_index = element;
            objVec.push_back(statsObj);
        }

        for (auto & element : objVec) {
            cout << "ID: " << element.id << endl;
            cout << "Damage Class ID: " << element.damage_class_id << endl;
            cout << "Identifier: " << element.identifier << endl;
            cout << "Is Battle Only: " << element.is_battle_only << endl;
            cout << "Game Index: " << element.game_index << endl;
            cout << "\n";
        }
    }

    if(strcmp(validNames[dataType], "pokemon") == 0) {
        std::vector<pokemon> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            pokemon pokemonObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        pokemonObj.id = stoi(token);
                        break;
                    case 1 :
                        pokemonObj.identifier = token;
                        break;
                    case 2 :
                        pokemonObj.species_id = stoi(token);
                        break;
                    case 3 :
                        pokemonObj.height = stoi(token);
                        break;
                    case 4 :
                        pokemonObj.weight = stoi(token);
                        break;
                    case 5 :
                        pokemonObj.base_experience = stoi(token);
                        break;
                    case 6 :
                        pokemonObj.order = stoi(token);
                        break;
                }
                elementPos++;
            }
            pokemonObj.is_default = stoi(element);
            objVec.push_back(pokemonObj);
        }

        for (auto & element : objVec) {
            cout << "ID: " << element.id << endl;
            cout << "Identifier: " << element.identifier << endl;
            cout << "Species ID: " << element.species_id << endl;
            cout << "Height: " << element.height << endl;
            cout << "Weight: " << element.weight << endl;
            cout << "Base Experience: " << element.base_experience << endl;
            cout << "Order: " << element.order << endl;
            cout << "Is Default: " << element.is_default << endl;
            cout << "\n";
        }
    }

    if(strcmp(validNames[dataType], "type_names") == 0) {
        std::vector<typeNames> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            typeNames typeNameObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        typeNameObj.type_id = stoi(token);
                        break;
                    case 1 :
                        typeNameObj.local_language_id = stoi(token);
                        break;
                }
                elementPos++;
            }
            typeNameObj.name = element;
            objVec.push_back(typeNameObj);
        }

        for (auto & element : objVec) {
            cout << "Type ID: " << element.type_id << endl;
            cout << "Local Language ID: " << element.local_language_id << endl;
            cout << "Name: " << element.name << endl;
            cout << "\n";
        }
    }

    if(strcmp(validNames[dataType], "experience") == 0) {
        std::vector<experience> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            experience expObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        expObj.growth_rate_id = stoi(token);
                        break;
                    case 1 :
                        expObj.level = stoi(token);
                        break;
                }
                elementPos++;
            }
            expObj.experience = stoi(element);
            objVec.push_back(expObj);
        }

        for (auto & element : objVec) {
            cout << "Growth Rate ID: " << element.growth_rate_id << endl;
            cout << "Level: " << element.level << endl;
            cout << "Experience: " << element.experience << endl;
            cout << "\n";
        }
    }

    if(strcmp(validNames[dataType], "moves") == 0) {
        std::vector<moves> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            moves moveObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        try {
                            moveObj.id = stoi(token);
                        } catch(exception& e) {
                            moveObj.id = INT_MAX;
                        }
                        break;
                    case 1 :
                        try {
                            moveObj.identifier = token;
                        } catch(exception& e) {
                            moveObj.identifier = "";
                        }
                        break;
                    case 2 :
                        try {
                            moveObj.generation_id = stoi(token);
                        } catch(exception& e) {
                            moveObj.generation_id = INT_MAX;
                        }
                        break;
                    case 3 :
                        try {
                            moveObj.type_id = stoi(token);
                        } catch(exception& e) {
                            moveObj.type_id = INT_MAX;
                        }
                        break;
                    case 4 :
                        try {
                            moveObj.power = stoi(token);
                        } catch(exception& e) {
                            moveObj.power = INT_MAX;
                        }
                        break;
                    case 5 :
                        try {
                            moveObj.pp = stoi(token);
                        } catch(exception& e) {
                            moveObj.pp = INT_MAX;
                        }
                        break;
                    case 6 :
                        try {
                            moveObj.accuracy = stoi(token);
                        } catch(exception& e) {
                            moveObj.accuracy = INT_MAX;
                        }
                        break;
                    case 7 :
                        try {
                            moveObj.priority = stoi(token);
                        } catch(exception& e) {
                            moveObj.priority = INT_MAX;
                        }
                        break;
                    case 8 :
                        try {
                            moveObj.target_id = stoi(token);
                        } catch(exception& e) {
                            moveObj.target_id = INT_MAX;
                        }
                        break;
                    case 9 :
                        try {
                            moveObj.damage_class_id = stoi(token);
                        } catch(exception& e) {
                            moveObj.damage_class_id = INT_MAX;
                        }
                        break;
                    case 10 :
                        try {
                            moveObj.effect_id = stoi(token);
                        } catch(exception& e) {
                            moveObj.effect_id = INT_MAX;
                        }
                        break;
                    case 11 :
                        try {
                            moveObj.effect_chance = stoi(token);
                        } catch(exception& e) {
                            moveObj.effect_chance = INT_MAX;
                        }
                        break;
                    case 12 :
                        try {
                            moveObj.contest_type_id = stoi(token);
                        } catch(exception& e) {
                            moveObj.contest_type_id = INT_MAX;
                        }
                        break;
                    case 13 :
                        try {
                            moveObj.contest_effect_id = stoi(token);
                        } catch(exception& e) {
                            moveObj.contest_effect_id = INT_MAX;
                        }
                        break;
                }
                elementPos++;
            }
            try {
                moveObj.super_contest_effect_id = stoi(element);
            } catch(exception& e) {
                moveObj.super_contest_effect_id = INT_MAX;
            }
            objVec.push_back(moveObj);
        }

        for (auto & element : objVec) {
            cout << "ID: " << (element.id == INT_MAX ? "" : std::to_string(element.id)) << endl;
            cout << "Identifier: " << element.identifier << endl;
            cout << "Generation ID: " << (element.generation_id == INT_MAX ? "" : std::to_string(element.id)) << endl;
            cout << "Type ID: " << (element.type_id == INT_MAX ? "" : std::to_string(element.type_id))  << endl;
            cout << "Power: " << (element.power == INT_MAX ? "" : std::to_string(element.power)) << endl;
            cout << "PP: " << (element.pp == INT_MAX ? "" : std::to_string(element.pp)) << endl;
            cout << "Accuracy: " << (element.accuracy == INT_MAX ? "" : std::to_string(element.accuracy)) << endl;
            cout << "Priority: " << (element.priority == INT_MAX ? "" : std::to_string(element.priority)) << endl;
            cout << "Target ID: " << (element.target_id == INT_MAX ? "" : std::to_string(element.target_id)) << endl;
            cout << "Damage Class ID: " << (element.damage_class_id == INT_MAX ? "" : std::to_string(element.damage_class_id)) << endl;
            cout << "Effect ID: " << (element.effect_id == INT_MAX ? "" : std::to_string(element.effect_id))<< endl;
            cout << "Effect Chance: " << (element.effect_chance == INT_MAX ? "" : std::to_string(element.effect_chance)) << endl;
            cout << "Contest Type ID: " << (element.contest_type_id == INT_MAX ? "" : std::to_string(element.contest_type_id)) << endl;
            cout << "Super Contest Type ID: " << (element.super_contest_effect_id == INT_MAX ? "" : std::to_string(element.super_contest_effect_id)) << endl;
            cout << "\n";
        }
    }

    if(strcmp(validNames[dataType], "pokemon_moves") == 0) {
        std::vector<pokemonMoves> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            pokemonMoves pokemonMoveObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        try {
                            pokemonMoveObj.pokemon_id = stoi(token);
                        } catch(exception& e) {
                            pokemonMoveObj.pokemon_id = INT_MAX;
                        }
                        break;
                    case 1 :
                        try {
                            pokemonMoveObj.version_group_id = stoi(token);
                        } catch(exception& e) {
                            pokemonMoveObj.version_group_id = INT_MAX;
                        }
                        break;
                    case 2 :
                        try {
                            pokemonMoveObj.move_id = stoi(token);
                        } catch(exception& e) {
                            pokemonMoveObj.move_id = INT_MAX;
                        }
                        break;
                    case 3 :
                        try {
                            pokemonMoveObj.pokemon_move_method_id = stoi(token);
                        } catch(exception& e) {
                            pokemonMoveObj.pokemon_move_method_id = INT_MAX;
                        }
                        break;
                    case 4 :
                        try {
                            pokemonMoveObj.level = stoi(token);
                        } catch(exception& e) {
                            pokemonMoveObj.level = INT_MAX;
                        }
                        break;
                }
                elementPos++;
            }
            try {
                pokemonMoveObj.order = stoi(element);
            } catch(exception& e) {
                pokemonMoveObj.order = INT_MAX;
            }
            objVec.push_back(pokemonMoveObj);
        }

        for (auto & element : objVec) {
            cout << "Pokemon ID: " << (element.pokemon_id == INT_MAX ? "" : std::to_string(element.pokemon_id)) << endl;
            cout << "Version Group ID: " << (element.version_group_id == INT_MAX ? "" : std::to_string(element.version_group_id)) << endl;
            cout << "Move ID: " << (element.move_id == INT_MAX ? "" : std::to_string(element.move_id)) << endl;
            cout << "Move Method ID: " << (element.pokemon_move_method_id == INT_MAX ? "" : std::to_string(element.pokemon_move_method_id))  << endl;
            cout << "Level: " << (element.level == INT_MAX ? "" : std::to_string(element.level)) << endl;
            cout << "Order: " << (element.order == INT_MAX ? "" : std::to_string(element.order)) << endl;
            cout << "\n";
        }
    }

    if(strcmp(validNames[dataType], "pokemon_species") == 0) {
        std::vector<pokemonSpecies> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            pokemonSpecies speciesObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        try {
                            speciesObj.id = stoi(token);
                        } catch(exception& e) {
                            speciesObj.id = INT_MAX;
                        }
                        break;
                    case 1 :
                        try {
                            speciesObj.identifier = token;
                        } catch(exception& e) {
                            speciesObj.identifier = "";
                        }
                        break;
                    case 2 :
                        try {
                            speciesObj.generation_id = stoi(token);
                        } catch(exception& e) {
                            speciesObj.generation_id = INT_MAX;
                        }
                        break;
                    case 3 :
                        try {
                            speciesObj.evolves_from_species_id = stoi(token);
                        } catch(exception& e) {
                            speciesObj.evolves_from_species_id = INT_MAX;
                        }
                        break;
                    case 4 :
                        try {
                            speciesObj.evolution_chain_id = stoi(token);
                        } catch(exception& e) {
                            speciesObj.evolution_chain_id = INT_MAX;
                        }
                        break;
                    case 5 :
                        try {
                            speciesObj.color_id = stoi(token);
                        } catch(exception& e) {
                            speciesObj.color_id = INT_MAX;
                        }
                        break;
                    case 6 :
                        try {
                            speciesObj.shape_id = stoi(token);
                        } catch(exception& e) {
                            speciesObj.shape_id = INT_MAX;
                        }
                        break;
                    case 7 :
                        try {
                            speciesObj.habitat_id = stoi(token);
                        } catch(exception& e) {
                            speciesObj.habitat_id = INT_MAX;
                        }
                        break;
                    case 8 :
                        try {
                            speciesObj.gender_rate = stoi(token);
                        } catch(exception& e) {
                            speciesObj.gender_rate = INT_MAX;
                        }
                        break;
                    case 9 :
                        try {
                            speciesObj.capture_rate = stoi(token);
                        } catch(exception& e) {
                            speciesObj.capture_rate = INT_MAX;
                        }
                        break;
                    case 10 :
                        try {
                            speciesObj.base_happiness = stoi(token);
                        } catch(exception& e) {
                            speciesObj.base_happiness = INT_MAX;
                        }
                        break;
                    case 11 :
                        try {
                            speciesObj.is_baby = stoi(token);
                        } catch(exception& e) {
                            speciesObj.is_baby = INT_MAX;
                        }
                        break;
                    case 12 :
                        try {
                            speciesObj.hatch_counter = stoi(token);
                        } catch(exception& e) {
                            speciesObj.hatch_counter = INT_MAX;
                        }
                        break;
                    case 13 :
                        try {
                            speciesObj.has_gender_differences = stoi(token);
                        } catch(exception& e) {
                            speciesObj.has_gender_differences = INT_MAX;
                        }
                        break;
                    case 14 :
                        try {
                            speciesObj.growth_rate_id = stoi(token);
                        } catch(exception& e) {
                            speciesObj.growth_rate_id = INT_MAX;
                        }
                        break;
                    case 15 :
                        try {
                            speciesObj.forms_switchable = stoi(token);
                        } catch(exception& e) {
                            speciesObj.forms_switchable = INT_MAX;
                        }
                        break;
                    case 16 :
                        try {
                            speciesObj.is_legendary = stoi(token);
                        } catch(exception& e) {
                            speciesObj.is_legendary = INT_MAX;
                        }
                        break;
                    case 17 :
                        try {
                            speciesObj.is_mythical = stoi(token);
                        } catch(exception& e) {
                            speciesObj.is_mythical = INT_MAX;
                        }
                        break;
                    case 18 :
                        try {
                            speciesObj.order = stoi(token);
                        } catch(exception& e) {
                            speciesObj.order = INT_MAX;
                        }
                        break;
                }
                elementPos++;
            }
            try {
                speciesObj.conquest_order = stoi(element);
            } catch(exception& e) {
                speciesObj.conquest_order = INT_MAX;
            }
            objVec.push_back(speciesObj);
        }

        for (auto & element : objVec) {
            cout << "ID: " << (element.id == INT_MAX ? "" : std::to_string(element.id)) << endl;
            cout << "Identifier: " << element.identifier << endl;
            cout << "Generation ID: " << (element.generation_id == INT_MAX ? "" : std::to_string(element.id)) << endl;
            cout << "Evolves From Species ID: " << (element.evolves_from_species_id == INT_MAX ? "" : std::to_string(element.evolves_from_species_id))  << endl;
            cout << "Evolution Chain ID: " << (element.evolution_chain_id == INT_MAX ? "" : std::to_string(element.evolution_chain_id)) << endl;
            cout << "Color ID: " << (element.color_id == INT_MAX ? "" : std::to_string(element.color_id)) << endl;
            cout << "Shape ID: " << (element.shape_id == INT_MAX ? "" : std::to_string(element.shape_id)) << endl;
            cout << "Habitat ID: " << (element.habitat_id == INT_MAX ? "" : std::to_string(element.habitat_id)) << endl;
            cout << "Gender Rate: " << (element.gender_rate == INT_MAX ? "" : std::to_string(element.gender_rate)) << endl;
            cout << "Capture Rate: " << (element.capture_rate == INT_MAX ? "" : std::to_string(element.capture_rate)) << endl;
            cout << "Base Happiness: " << (element.base_happiness == INT_MAX ? "" : std::to_string(element.base_happiness))<< endl;
            cout << "Is Baby: " << (element.is_baby == INT_MAX ? "" : std::to_string(element.is_baby)) << endl;
            cout << "Hatch Counter: " << (element.hatch_counter == INT_MAX ? "" : std::to_string(element.hatch_counter)) << endl;
            cout << "Has Gender Differences: " << (element.has_gender_differences == INT_MAX ? "" : std::to_string(element.has_gender_differences)) << endl;
            cout << "Growth Rate ID: " << (element.growth_rate_id == INT_MAX ? "" : std::to_string(element.growth_rate_id)) << endl;
            cout << "Forms Switchable: " << (element.forms_switchable == INT_MAX ? "" : std::to_string(element.forms_switchable)) << endl;
            cout << "Is Legendary: " << (element.is_legendary == INT_MAX ? "" : std::to_string(element.is_legendary)) << endl;
            cout << "Is Mythical: " << (element.is_mythical == INT_MAX ? "" : std::to_string(element.is_mythical)) << endl;
            cout << "Order: " << (element.order == INT_MAX ? "" : std::to_string(element.order)) << endl;
            cout << "Conquest Order: " << (element.conquest_order == INT_MAX ? "" : std::to_string(element.conquest_order)) << endl;
            cout << "\n";
        }
    }

    if(strcmp(validNames[dataType], "pokemon_stats") == 0) {
        std::vector<pokemonStats> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            pokemonStats statsObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        statsObj.id = stoi(token);
                        break;
                    case 1 :
                        statsObj.stat_id = stoi(token);
                        break;
                    case 2 :
                        statsObj.base_stat = stoi(token);
                        break;
                }
                elementPos++;
            }
            statsObj.effort = stoi(element);
            objVec.push_back(statsObj);
        }

        for (auto & element : objVec) {
            cout << "ID: " << element.id << endl;
            cout << "Stat ID: " << element.stat_id << endl;
            cout << "Base Stat: " << element.base_stat << endl;
            cout << "Effort: " << element.effort << endl;
            cout << "\n";
        }
    }

    if(strcmp(validNames[dataType], "pokemon_types") == 0) {
        std::vector<pokemonTypes> objVec;

        // Parse all lines of file into seglist. Adding each line as a std::string 'segment'
        while(getline(csvFile, segment)) {
            seglist.push_back(segment);
        }

        // For each std::string 'segment' in seglist, split the line by commas, 
        for (auto & element : seglist) {
            pokemonTypes typesObj;
            std::vector<std::string> elements;

            std::string delimiter = ",";

            size_t pos = 0;
            std::string token;
            int elementPos = 0;
            while ((pos = element.find(delimiter)) != std::string::npos) {
                token = element.substr(0, pos);
                element.erase(0, pos + delimiter.length());

                switch(elementPos) {
                    case 0 :
                        typesObj.pokemon_id = stoi(token);
                        break;
                    case 1 :
                        typesObj.type_id = stoi(token);
                        break;
                }
                elementPos++;
            }
            typesObj.slot = stoi(element);
            objVec.push_back(typesObj);
        }

        for (auto & element : objVec) {
            cout << "Pokemon ID: " << element.pokemon_id << endl;
            cout << "Type ID: " << element.type_id << endl;
            cout << "Slot: " << element.slot << endl;
            cout << "\n";
        }
    }

    csvFile.close();
}
