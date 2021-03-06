#ifndef SCOUTBEE_HPP
#define SCOUTBEE_HPP
#include "Bee.hpp"

class ScoutBee : public Bee
{
public:
    // getConfig() passe par l'appel d'une méthode statique
    static j::Value const& getScoutConfig();
    j::Value const& getConfig() const override;
    ScoutBee(Hive& hive, Vec2d pos);
    virtual ~ScoutBee();
    // Prépare le changement d'état
    virtual void onEnterState(State etat) override;
    // Définit l'action à faire selon l'état
    virtual void onState(State etat, sf::Time dt) override;
    void drawOn(sf::RenderTarget& target) const;

    // Permet l'interraction entre abeille
    void interact(Bee* other);
    void interactWith(ScoutBee* other);
    // -> Seul l'interraction WorkerBee/ScoutBee nous intéresse ici
    void interactWith(WorkerBee* other);

private:
    static const State IN_HIVE;
    static const State SEEK_FLOWER;
    static const State BACK_HIVE;
    // Le nombre de fois qu'une information a été partagée
    unsigned int shared_;

};



#endif
