#ifndef HIVE_HPP
#define HIVE_HPP
#include "Collider.hpp"
#include <SFML/Graphics.hpp>

// Prédéclaration nécessaire pour régler le problème de dépendance circulaire
class Bee;
class ScoutBee;
class WorkerBee;

class Hive : public Collider
{
public:
    Hive(Vec2d pos, double rayon);
    ~Hive();
    Hive(const Hive&) = delete;
    Hive& operator=(Hive&) = delete;
    void destroyBees();
    // Ajoute une WorkerBee
    WorkerBee* addWorker();
    // Ajoute une ScoutBee
    ScoutBee* addScout();
    // Développement de la ruche et de son contenu
    void update(sf::Time dt);
    // Dessin de la ruche avec variation de textures
    void drawOn(sf::RenderTarget& targetWindow) const;
    // Ajout/Retrait de nectar
    void dropPollen(double qte);
    double takeNectar(double qte);
    double getNectar() const;
    std::vector<Bee*> getBees() const;



private:
    std::vector<Bee*> bees_;
    double nbNectar_;



};


#endif
