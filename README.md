# ComS327-Pokemon (1.08)
### Author:
Ethan Hancock, ehancock@iastate.edu
### TA Notes: 
> When the player runs into a trainer, and a pokemon battle is triggered, the trainer and player's pokemon data is displayed beneath the map, requiring a terminal with a larger window size/height.
> When a pokemon battle is triggered, the trainer's pokemon is randomly chosen out of their held pokemon, right now I'm not marking them as defeated after the battle, so you an enter a battle with them over and over again to verify that the pokemon you battle are the same.

### Description:
#### Major Changes:
 - Data from 1.07 CSV parsing is being used to generate pokemon, allowing for random encounters in tall grass and simple battle sequences.

#### Methods Added
 - pokemonBattle()
    - Called when the player walks into contact with a trainer, displaying the player's starter pokemon stats as well as the trainer's pokemon stats
 - createPokemon()
    - Used to create a pokemon, accepts a pointer to a pokemon variable to populate with stats and things
 - givePlayerPokemon()
    - Called once at the beginning to allow the user to make a selection from 3 random level 1 pokemon for their start
 - encounterPokemon()
    - 10% chance of being called when the player walks into a tall grass terrain square.
    - Displays a window telling the player they found a pokemon, and also displaying the pokemon's stats.