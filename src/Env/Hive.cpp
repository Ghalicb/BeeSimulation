// Pas besoin d'inclure Hive.hpp, car déjà inclut dans Bee.hpp
#include "Bee.hpp"
#include <Application.hpp>
#include "../Utility/Utility.hpp"
#include "Random/Random.hpp"
#include "WorkerBee.hpp"
#include "ScoutBee.hpp"
#include "Env.hpp"
#include <array>

using namespace std;

Hive::Hive(Vec2d pos, double rayon)
    : Collider(pos, rayon)
{
    nbNectar_ = getAppConfig()["simulation"]["hive"]["initial"]["nectar"].toDouble();
}

Hive::~Hive()
{
    destroyBees();
}

/* ---------------------------------------------------------------------
 * GESTION DE LA RUCHE ET DE SES ABEILLES
 */

void Hive::destroyBees()
{
    for (auto& bee : bees_) {
        delete bee;
    }

    bees_.clear();
}

WorkerBee* Hive::addWorker()
{
    Vec2d dx = uniform(0.0, getRadius()) * Vec2d::fromRandomAngle();
    WorkerBee* wBee = new WorkerBee(*this, this->getPosition() + dx);
    bees_.push_back(wBee);

    return wBee;
}

ScoutBee* Hive::addScout()
{
    Vec2d dx = uniform(0.0, getRadius()) * Vec2d::fromRandomAngle();
    ScoutBee* sBee = new ScoutBee(*this, this->getPosition() + dx);
    bees_.push_back(sBee);

    return sBee;
}

void Hive::update(sf::Time dt)
{

    double nectarMinRep = getAppConfig()["simulation"]["hive"]["reproduction"]["nectar threshold"].toDouble();
    unsigned int maxBee = getAppConfig()["simulation"]["hive"]["reproduction"]["max bees"].toDouble();
    double proba = getAppConfig()["simulation"]["hive"]["reproduction"]["scout probability"].toDouble();

    // Si assez de nectar, on rajoute une abeille d'un type aléatoire (jusqu'à une certaine limite)
    if ( (nbNectar_ >= nectarMinRep) and (bees_.size() < maxBee) ) {
        if (bernoulli(proba)) {
            addWorker();
        } else {
            addScout();
        }
    }

    // S'occupe de l'évolution des abeilles appartenant à une ruche
    vector<Bee*> inHiveBees;
    for(auto& bee : bees_) {
        bee->update(dt);
        if(bee->isDead()) {
            delete bee;
            bee = nullptr;
        } else {
            if(this->isColliding(*bee)) {
                inHiveBees.push_back(bee);
            }
        }
    }
    bees_.erase(std::remove(bees_.begin(), bees_.end(), nullptr),bees_.end());

    // Fait interagir les abeilles dans la ruche
    if (inHiveBees.size() >= 1) {
        for(unsigned int i(0); i < (inHiveBees.size())-1 ; ++i) {
            for(unsigned int j(i+1); j < inHiveBees.size(); ++j) {
                inHiveBees[i]->interact(inHiveBees[j]);
            }
        }
    }

    // Le tableau est automatiquement vidé à la fin de la fonction (fin de portée)
}

void Hive::drawOn(sf::RenderTarget& targetWindow) const
{

    auto const& texture = getAppTexture((getAppConfig()["simulation"]["hive"]["texture"]["nul"]).toString());
    auto const&	texture_h = getAppTexture((getAppConfig()["simulation"]["hive"]["texture"]["honey"]).toString());
    auto const& textureH = getAppTexture((getAppConfig()["simulation"]["hive"]["texture"]["honeyyy"]).toString());
    int indiceText(0);

    array<sf::Texture, 3> textures = {texture, texture_h, textureH};

    // Fait varier la texture de la ruche selon sa qte de pollen (miel)

    if (nbNectar_ <= 1500) {
        indiceText = 0;
    } else if (nbNectar_ <= 2000) {
        indiceText = 1;
    } else if (nbNectar_ < 2500) {
        indiceText = 2;
    } else if (nbNectar_ >= 2500) {
        indiceText = 2;
        getAppEnv().setFinish(true);
    }

    auto hiveSprite = buildSprite(getPosition(), 2.5 * getRadius(), textures[indiceText]);
    targetWindow.draw(hiveSprite);
}

void Hive::dropPollen(double qte)
{
    nbNectar_ += qte;
}

double Hive::takeNectar(double qte)
{
    if (qte > nbNectar_) {
        double nectarI(nbNectar_);
        nbNectar_ = 0;
        return nectarI;
    }

    else {
        nbNectar_ -= qte;
    }

    return qte;
}

double Hive::getNectar() const
{
    return nbNectar_;
}

std::vector<Bee*> Hive::getBees() const
{
    return bees_;
}


