#include "Bee.hpp"
#include <Application.hpp>
#include "../Utility/Utility.hpp"
#include "../Random/Random.hpp"
#include "Env.hpp"


using namespace std;

Bee::Bee(Hive& hive, Vec2d pos, double rayon, double energie, double ampV, vector<State> etats)
    : Collider(pos, rayon), CFSM(etats), hive_(hive), energie_(energie), memory_(false), mood_(Repos)
{
    vitesse_ = ampV * Vec2d::fromRandomAngle();
}

Bee::Bee(const Bee& copyBee)
    : Collider(copyBee), CFSM(copyBee.getStates()),hive_(copyBee.hive_), energie_(copyBee.energie_), vitesse_(copyBee.vitesse_), mood_(copyBee.mood_)
{}

Bee::~Bee()
{}

/* ---------------------------------------------------------------------
 * GETTERS ET SETTERS UTILES
 */

double Bee::getEnergie() const
{
    return energie_;
}

Vec2d Bee::getMemoryFlower() const
{
    return memoryFlower_;
}

bool Bee::getMemory() const
{
    return memory_;
}
Hive& Bee::getHiveRef() const
{
    return hive_;
}
void Bee::setMemoryFlower(const Vec2d& flowerPos)
{
    memoryFlower_ = flowerPos;
    setMemory(true);
}
void Bee::setMemory(bool memoire)
{
    memory_ = memoire;
}

void Bee::setMood(Mood mood)
{
    mood_ = mood;
}

void Bee::setCible(const Vec2d& pos)
{
    cible_ = pos;
}

/* ---------------------------------------------------------------------
 * EVOLUTION ET DESSIN DES ABEILLES
 */

void Bee::addEnergie(double qt)
{
    energie_ += qt;
}

bool Bee::isDead() const
{
    return energie_ == 0;
}

j::Value const& Bee::getConfig() const
{
    return getAppConfig()["simulation"]["bees"]["generic"];
}

void Bee::update(sf::Time dt)
{
    action(dt);
    move(dt);
    energyLoss(dt);
}

void Bee::energyLoss(sf::Time dt)
{
    // Perd moins d'énergie au repos qu'en déplacement
    if (mood_ == Repos) {
        double tauxIdle = getConfig()["energy"]["consumption rates"]["idle"].toDouble();
        if (energie_ - (tauxIdle * dt.asSeconds()) >= 0) {
            energie_ -= tauxIdle * dt.asSeconds();
        }
        // Si plus d'énergie, on ne prélève pas de quantité "négative" d'énergie
        else {
            energie_ = 0;
        }
    }

    else {
        double tauxMoving = getConfig()["energy"]["consumption rates"]["moving"].toDouble();
        if (energie_ - (tauxMoving * dt.asSeconds()) >= 0) {
            energie_ -= tauxMoving * dt.asSeconds();
        } else {
            energie_ = 0;
        }
    }
}

void Bee::move(sf::Time dt)
{
    if (mood_ == Aleatoire) {
        moveRandomBee(dt);
    }

    if (mood_ == Cible) {
        targetMove(dt);
    }
}

void Bee::moveRandomBee(sf::Time dt)
{
    double prob = getConfig()["moving behaviour"]["random"]["rotation probability"].toDouble();
    double alphaMax = getConfig()["moving behaviour"]["random"]["rotation angle max"].toDouble();

    // On modifie l'angle de la vitesse de l'abeille
    if (bernoulli(prob)) {
        double alpha = uniform(-alphaMax, alphaMax);
        vitesse_.rotate(alpha);
    }

    // Si elle peut voler => elle se déplace ...
    if (getAppEnv().isFlyable(getPosition())) {
        Collider::move(vitesse_ * dt.asSeconds());
    }
    // sinon elle tente de passer par un autre chemin
    else {
        changeAngle(dt);
    }
}

void Bee::targetMove(sf::Time dt)
{
    if (avoidanceClock_ < sf::Time::Zero) {
        vitesse_ = directionTo(cible_).normalised() * vitesse_.length();
    } else {
        avoidanceClock_ -= dt;
    }

    if (!(getAppEnv().isFlyable(getPosition()))) {
        float delay = getConfig()["moving behaviour"]["target"]["avoidance delay"].toDouble();

        changeAngle(dt);
        avoidanceClock_ = sf::seconds(delay);
    } else {
        Collider::move(vitesse_ * dt.asSeconds());
    }
}

void Bee::changeAngle(sf::Time dt)
{
    double beta = - PI/4;

    // Changement d'angle autant probable à gauche qu'à droite
    if (bernoulli(0.5)) {
        beta *= -1;
    }

    vitesse_.rotate(beta);
    Collider::move(vitesse_ * dt.asSeconds());
}

void Bee::drawOn(sf::RenderTarget& target) const
{
    auto const& texture = getAppTexture(getConfig()["texture"].toString());
    auto beeSprite = buildSprite(getPosition(), getRadius(), texture);

    double alpha(vitesse_.Vec2d::angle());
    // Changement d'angle de la texture
    if ( (alpha >= PI/2) or (alpha <= -PI/2) ) {
        beeSprite.scale(1, -1);
    }

    beeSprite.rotate(alpha /DEG_TO_RAD);

    target.draw(beeSprite);

    if (isDebugOn()) {
        drawBeeZone(target, getPosition());
    }
}

void Bee::drawBeeZone(sf::RenderTarget& target, Vec2d const& position) const
{
    double size = getConfig()["size"].toDouble();
    double thickness(0.0);
    sf::Color color(0,0,0);

    // Change le marquage de l'abeille selon son état
    if (mood_ == Aleatoire) {
        thickness = 5.0;
        color = sf::Color::Black;
    }

    if (mood_ == Cible) {
        thickness = 3.0;
        color = sf::Color::Blue;
    }

    auto shape = buildAnnulus(position, size, color, thickness);
    target.draw(shape);
}

void Bee::winText(sf::RenderTarget& target) const
{
    // Si Condition de victoire ...
    if ((isDebugOn()) and (getAppEnv().stopDebugOn()) and (getHiveRef().getNectar() > 2500)) {
        Vec2d textePosWin(getPosition()- getConfig()["size"].toDouble()*Vec2d::fromAngle(-PI/2));
        string textWin = "WinBee";
        int textsizeWin(12);

        sf::Color colorWin(255,215,0);
        // Désigne les abeilles appartenant à la ruche gagnante
        auto const text3 = buildText( textWin, textePosWin, getAppFont(), textsizeWin, colorWin);
        target.draw(text3);
    }
}
