#include "WorkerBee.hpp"
#include "ScoutBee.hpp"
#include "Env.hpp"
#include <Application.hpp>
#include <Utility/Vec2d.hpp>


using namespace std;

const State WorkerBee::IN_HIVE = createUid();
const State WorkerBee::TO_FLOWER = createUid();
const State WorkerBee::TAKE_POLLEN = createUid();
const State WorkerBee::BACK_HIVE = createUid();

j::Value const& WorkerBee::getWorkerConfig()
{
    return getAppConfig()["simulation"]["bees"]["worker"];
}

j::Value const& WorkerBee::getConfig() const
{
    return getWorkerConfig();
}

WorkerBee::WorkerBee(Hive& hive, Vec2d pos)
    : Bee(hive, pos, getWorkerConfig()["size"].toDouble(),
          getWorkerConfig()["energy"]["initial"].toDouble(),
          getWorkerConfig()["speed"].toDouble(),
{
    IN_HIVE, TO_FLOWER, TAKE_POLLEN, BACK_HIVE
}), load_(0)
{}

WorkerBee::~WorkerBee()
{}

/* ---------------------------------------------------------------------
 * EVOLUTION ET DESSIN DES WORKERBEES
 */

void WorkerBee::onState(State etat, sf::Time dt)
{
    // Une fois dans la ruche, la buttineuse peut soit déposer son polen,
    // soit se nourrir avant de repartir
    if(etat == IN_HIVE) {
        if(load_ > 0) {
            double initLoad(load_);
            double transfer = getConfig()["transfer rate"].toDouble()* dt.asSeconds();
            if(load_ > transfer) {
                getHiveRef().dropPollen(transfer);
                load_ -= transfer;
            }
            if(load_ <= transfer) {
                getHiveRef().dropPollen(initLoad);
                load_ = 0;
            }
        }

        if(load_ == 0) {
            if( getEnergie() < getConfig()["energy"]["to leave hive"].toDouble() ) {
                double qtPollen;
                qtPollen = getHiveRef().takeNectar((getConfig()["energy"]["consumption rates"]["eating"].toDouble())* dt.asSeconds());
                addEnergie(qtPollen);
            } else {
                if(getMemory()) { // true ==> elle a une fleur en memoire
                    nextState();
                }
            }
        }
    }

    Collider bodyFlower( getMemoryFlower(), getAppConfig()["simulation"]["env"]["initial"]["flower"]["size"]["manual"].toDouble() );
    Collider bodyRange(getPosition(), getConfig()["size"].toDouble()+getConfig()["visibility range"].toDouble());

    if(etat == TO_FLOWER) {
        // Si la butineuse collide la fleur en mémoire ...
        if(bodyRange.isColliding(bodyFlower)) {
            if( getAppEnv().getCollidingFlower(bodyRange) != nullptr ) {
                // et que le centre du Collider de la ruche est dans le collider de la fleur,
                if( isPointInside(bodyFlower.getPosition()) ) {
                    // => TakePollen
                    nextState();
                }
            } else
                // ... sinon elle rentre à la ruche
            {
                setIndiceState(3);
                onEnterState(BACK_HIVE);
            }
        }
    }

    if(etat == TAKE_POLLEN) {
        Flower* workFlower = getAppEnv().getCollidingFlower(bodyRange);
        if( (load_ < getConfig()["max pollen capacity"].toDouble()) and (workFlower != nullptr)) {
            load_ += workFlower->takePollen( (getConfig()["harvest rate"].toDouble())* dt.asSeconds() );
        } else {
            nextState();
        }
    }

    if(etat == BACK_HIVE) {
        if(isPointInside(getHiveRef().getPosition())) {
            nextState();
        }
    }
}

void WorkerBee::onEnterState(State etat)
{
    if(etat == IN_HIVE) {
        setMood(Repos);
    }

    if(etat == TO_FLOWER ) {
        setCible(getMemoryFlower());
        setMood(Cible);
    }
    if( etat == TAKE_POLLEN ) {
        setMood(Repos);
    }
    if(etat == BACK_HIVE) {
        setMemory(false);
        setMood(Cible);
        setCible(getHiveRef().getPosition());
    }
}

void WorkerBee::drawOn(sf::RenderTarget& target) const
{
    Bee::drawOn(target);
    if ((isDebugOn()) and !(getAppEnv().stopDebugOn())) {
        Vec2d textePos(getPosition()- getConfig()["size"].toDouble()*Vec2d::fromAngle(-PI/2));
        int textsize(10);
        string textEnergy = to_nice_string(getEnergie());
        string textState;
        // Affiche l'énergie et le type des abeilles (ici Worker)
        auto const text = buildText( " Worker: Energie " + textEnergy , textePos, getAppFont(), textsize, sf::Color::White);
        target.draw(text);
        // Affiche l'état de l'abeille
        if(getState() == IN_HIVE and load_ > 0) {
            textState = "in_hive_pollen";
        }

        if(getState() == IN_HIVE and load_ == 0 and getEnergie() < getConfig()["energy"]["to leave hive"].toDouble()) {
            textState = "in_hive_eat";
        }

        if(getState() == IN_HIVE and load_ == 0 and getEnergie() >= getConfig()["energy"]["to leave hive"].toDouble() and !getMemory() ) {
            textState = "in_hive_no_flower" ;
        }

        if(getState() == IN_HIVE and load_ == 0 and getEnergie() < getConfig()["energy"]["to leave hive"].toDouble() and getMemory() ) {
            textState = "in_hive_???";
        }

        if(getState() == TO_FLOWER) {
            textState = "to_flower";
        }

        if(getState() == TAKE_POLLEN) {
            textState = "collecting_pollen";
        }

        if(getState() == BACK_HIVE) {
            textState = "back_to_hive";
        }

        textePos += Vec2d::fromAngle(-PI/2)*10.0;
        auto const text2 = buildText(textState, textePos, getAppFont(), textsize, sf::Color::White);
        target.draw(text2);
    }

    winText(target);
}

/* ---------------------------------------------------------------------
 * GESTION DES INTERRACTIONS ENTRE ABEILLES
 */

void WorkerBee::learnFlowerLocation(const Vec2d& flowerPos)
{
    setMemoryFlower(flowerPos);
}

void WorkerBee::interact(Bee* other)
{
    other->interactWith(this);
}

void WorkerBee::interactWith(ScoutBee* other)
{
    (*other).interactWith(this);
}

// Aucune interraction entre deux WorkerBee
void WorkerBee::interactWith(WorkerBee* other)
{}
