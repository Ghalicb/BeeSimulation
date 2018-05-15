#include "ScoutBee.hpp"
#include "WorkerBee.hpp"
#include <Application.hpp>
#include <Utility/Vec2d.hpp>
#include "Env.hpp"


using namespace std;

const State ScoutBee::IN_HIVE = createUid();
const State ScoutBee::SEEK_FLOWER = createUid();
const State ScoutBee::BACK_HIVE = createUid();


j::Value const& ScoutBee::getScoutConfig()
{
    return getAppConfig()["simulation"]["bees"]["scout"];
}


j::Value const& ScoutBee::getConfig() const
{
    return getScoutConfig();
}

ScoutBee::ScoutBee(Hive& hive, Vec2d pos)
    : Bee(hive, pos, getScoutConfig()["size"].toDouble(),
          getScoutConfig()["energy"]["initial"].toDouble(),
          getScoutConfig()["speed"].toDouble(),
{
    IN_HIVE, SEEK_FLOWER, BACK_HIVE
}),
shared_(0)
{}

ScoutBee::~ScoutBee()
{}

/* ---------------------------------------------------------------------
 * EVOLUTION ET DESSIN DES SCOUTBEES
 */

void ScoutBee::onEnterState(State etat)
{

    if(etat == IN_HIVE) {
        setMood(Repos);
    }

    if(etat == SEEK_FLOWER) {
        setMood(Aleatoire);
        // Comme elle cherche une fleure, elle n'en a pas en mémoire
        setMemory(false);
    }
    if(etat == BACK_HIVE) {
        setMood(Cible);
        setCible(getHiveRef().getPosition());
    }


}

void ScoutBee::onState(State etat, sf::Time dt)
{
    // Dans la hive, l'abeille doit reprendre de l'énergie (si nécessaire),
    // puis partir si elle a partagé son information
    if(etat == IN_HIVE) {
        if( getEnergie() < getConfig()["energy"]["to leave hive"].toDouble() ) {
            double qtPollen;
            qtPollen = getHiveRef().takeNectar((getConfig()["energy"]["consumption rates"]["eating"].toDouble())* dt.asSeconds());
            addEnergie(qtPollen);
        } else {
            if(!getMemory() or shared_ >= 1 ) {
                nextState();
                shared_ = 0;
            }
        }
    }
    if(etat == SEEK_FLOWER) {
        Collider bodyRange(getPosition(), getConfig()["size"].toDouble()+getConfig()["visibility range"].toDouble());
        Flower* fleurVue(nullptr);
        fleurVue = getAppEnv().getCollidingFlower(bodyRange);
        if(getEnergie() < getConfig()["energy"]["to seek flowers"].toDouble() or fleurVue != nullptr) {
            if(fleurVue != nullptr) {
                setMemoryFlower(fleurVue->getPosition());
                setMemory(true);
            }
            // L'abeille rentre car elle n'a plus d'energie(n'a rien trouvé)
            else {
                setMemory(false);
            }
            nextState();
        }
    }

    if(etat == BACK_HIVE) {
        if(isPointInside(getHiveRef().getPosition())) {
            nextState();
        }
    }
}

void ScoutBee::drawOn(sf::RenderTarget& target) const // methode de dessin polymorphique
{
    Bee::drawOn(target);

    if ((isDebugOn()) and !(getAppEnv().stopDebugOn())) {
        Vec2d textePos(getPosition()- 1.5* getConfig()["size"].toDouble()*Vec2d::fromAngle(-PI/2)); // juste au dessous de l'abeille
        string textEnergy = to_nice_string(getEnergie());
        int textsize(10);
        string textState;
        // Affiche l'énergie et le type des abeilles (ici Scout)
        auto const text = buildText( "Scout : Energie " + textEnergy , textePos, getAppFont(), textsize, sf::Color::White);
        target.draw(text);

        // Affiche l'état de l'abeille
        if(getEnergie() < getConfig()["energy"]["to leave hive"].toDouble()) {
            textState = "in_hive_eat";
        }
        if(!getMemory() or shared_ >=1 ) {
            textState = "in_hive_leaving";
        }
        if(getEnergie() < getConfig()["energy"]["to leave hive"].toDouble() and getMemory()) { // a une fleur en memoire :)
            textState = "in_hive_sharing " + to_nice_string(shared_);
        }
        if(getState() == SEEK_FLOWER) {
            textState = "seek_flower";
        }
        if(getState() == BACK_HIVE) {
            textState = "back_To_hive";
        }
        textePos += Vec2d::fromAngle(-PI/2)*10.0;
        auto const text2 = buildText( textState, textePos, getAppFont(), textsize, sf::Color::White);
        target.draw(text2);
    }

    // "Signal" les abeilles gagnantes
    winText(target);
}

/* ---------------------------------------------------------------------
 * GESTION DES INTERRACTIONS ENTRE ABEILLES
 */

void ScoutBee::interact(Bee* other)
{
    other->interactWith(this);
}

/* A noter que même si cette fonction est un prédicat, on ne peut mettre
 de const, car elle ne serait plus une spécialisation de celle de la
 super classe virtuelle */
void ScoutBee::interactWith(ScoutBee* other)
{}

void ScoutBee::interactWith(WorkerBee* other)
{
    // Pour interragir, il ne faut pas que la position ait déjà été "trop" partagé
    if(getMemory() and shared_ < getConfig()["sharing"]["max"].toInt()) {
        if( !((*other).getMemory()) ) {
            (*other).learnFlowerLocation(getMemoryFlower());
            ++shared_;
        }
    }
}
