#ifndef FLOWERGENERATOR_HPP
#define FLOWERGENERATOR_HPP
#include <SFML/Graphics.hpp>

class FlowerGenerator
{
public:
    FlowerGenerator();
    void update(sf::Time dt);
    void reset();


private:
    // Compte le temps écoulé depuis la dernière plante créée
    sf::Time compteur_;

};


#endif
