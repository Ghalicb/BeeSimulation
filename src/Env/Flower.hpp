#ifndef FLOWER_HPP
#define FLOWER_HPP
#include "Collider.hpp"
#include <SFML/Graphics.hpp>

class Flower : public Collider
{
public:
    Flower(Vec2d pos, double rayon, double pollen);
    double takePollen(double polTaken);
    double getPollen() const;
    void drawOn(sf::RenderTarget& target) const;
    // Gère le développement des fleurs en fonction de leur environnement
    void update(sf::Time dt);

private:
    double nbPollen_;
    // Permet de garder en mémoire la texture choisit pour une fleur
    int indice_;

};

#endif

