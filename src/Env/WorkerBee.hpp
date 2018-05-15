#ifndef WORKERBEE_HPP
#define WORKERBEE_HPP
#include "Bee.hpp"

class WorkerBee : public Bee
{
public:
    static j::Value const& getWorkerConfig();
    j::Value const& getConfig() const override;
    WorkerBee(Hive& hive, Vec2d pos);
    virtual ~WorkerBee();
    // Définit l'action à faire selon l'état
    void onState(State etat, sf::Time dt);
    // Prépare le changement d'état
    virtual void onEnterState(State etat);
    virtual void drawOn(sf::RenderTarget& target) const;

    // Aide à la communication pour le partage d'une position d'une fleur
    void learnFlowerLocation(const Vec2d& flowerPos);
    /* Permet l'interraction entre abeilles (pour les Worker
    * => double dispatch (on appelle l'interaction des Scout)
    */
    void interact(Bee* other);
    void interactWith(ScoutBee* other);
    // Pas d'interraction Worker/Worker ici
    void interactWith(WorkerBee* other);

private:
    static const State IN_HIVE;
    static const State TO_FLOWER;
    static const State TAKE_POLLEN;
    static const State BACK_HIVE;
    // Qte de pollen que l'abeille à réussi à charger
    double load_;

};


#endif
