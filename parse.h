#ifndef PARSE_H
#define PARSE_H
using namespace std;
    class pokemonStats {
        public:
            int id;
            int stat_id;
            int base_stat;
            int effort;
    };

    class stats {
        public:
            int id;
            int damage_class_id;
            string identifier;
            int is_battle_only;
            int game_index;
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

    extern std::vector<stats> allStats;
    extern std::vector<pokemon> allPokemon;
    extern std::vector<typeNames> allTypeNames;
    extern std::vector<experience> allExperience;
    extern std::vector<moves> allMoves;
    extern std::vector<pokemonMoves> allPokemonMoves;
    extern std::vector<pokemonSpecies> allPokemonSpecies;
    extern std::vector<pokemonStats> allPokemonStats;
    extern std::vector<pokemonTypes> allPokemonTypes;

    extern const char* validNames[10];

    int verifyFileName(const char* fileName);

    int parseFile(const char* fileName);

    void readFile(std::string fileName);
#endif
