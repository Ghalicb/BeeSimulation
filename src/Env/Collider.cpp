#include <iostream>
#include <cassert>
#include "Collider.hpp"
#include "Application.hpp"
#include <array>

using namespace std;


/* ---------------------------------------------------------------------
 * MISE EN PLACE DE LA CLASSE COLLIDER
 */

void Collider::clamping()
{
    ::clamping(position_);
}

Collider::Collider (Vec2d pos, double ray)
    : position_(pos), rayon_(ray)
{
    // Verifie si rayon positif (if not, programme plante)
    assert (rayon_ >= 0);
    // Converti la position donné dans le monde torique
    clamping();
}

Collider::Collider (const Collider& ColCopie)
    : position_(ColCopie.position_), rayon_(ColCopie.rayon_)
{}

double Collider::getRadius() const
{
    return rayon_;
}

const Vec2d& Collider::getPosition() const
{
    return position_;
}

Collider& Collider::operator=(const Collider& Col2)
{
    // This distingue position_ de l'instance courrante de celle de col
    this->position_ = Col2.position_;
    this->rayon_ = Col2.rayon_;
    return *this;
}

/* --------------------------------------------------------------------
 * GESTION DU DEPLACEMENT DANS UN MONDE TORIQUE
 */

Vec2d Collider::directionTo(const Vec2d& to) const
{
    auto worldSize = getApp().getWorldSize();
    auto width  = worldSize.x; // largeur
    auto height = worldSize.y; // hauteur

    // Definit le to le plus petit avec la contrainte "monde torique"
    array<double, 3> tabToX({to.x, to.x + width, to.x - width});
    array<double, 3> tabToY({to.y, to.y + height, to.y - height});
    Vec2d testTo;
    Vec2d ToMin;
    // distanceMin initialisé à la valeur maximale de la map
    double distanceMin( sqrt(height*height + width*width));
    double distanceTest(0.0);

    for (size_t i(0) ; i < tabToX.size() ; ++i) {
        testTo.x = tabToX[i];

        for (size_t j(0) ; j < tabToY.size() ; ++j) {
            testTo.y = tabToY[j];
            // position_ correspond au from
            distanceTest = distance(position_, testTo);
            if (distanceMin > distanceTest) {
                distanceMin = distanceTest;
                ToMin = testTo;
            }

        }
    }
    // On peut utiliser - pour Vec2d, car déjà surchargé dans la classe
    return (ToMin - position_);
}

Vec2d Collider::directionTo(const Collider& toC) const
{
    return directionTo(toC.position_);
}

double Collider::distanceTo(const Vec2d& to) const
{
    // .length, car length = méthode de Vec2d
    return (directionTo(to)).length();
}

double Collider::distanceTo(const Collider& toC) const
{
    return (directionTo(toC)).length();
}

void Collider::move(const Vec2d& dx)
{
    position_ += dx;
    // Verifie si la nouvelle position se trouve tjrs dans le monde torique
    clamping();
}

void Collider::operator+=(const Vec2d& dx)
{
    move(dx);
}

/* --------------------------------------------------------------------
 * GESTION DES COLLISIONS
 */


bool Collider::isColliderInside(const Collider& autre) const
{
    double DeltaPos(distanceTo(autre.position_));
    double DeltaRay(rayon_ - autre.rayon_);

    return ( (rayon_ >= autre.rayon_) and (DeltaPos <= DeltaRay) );
}

bool Collider::isColliding(const Collider& autre) const
{
    double DeltaPos(distanceTo(autre.position_));
    double SommeRay(rayon_ + autre.rayon_);

    return ( (DeltaPos <= SommeRay) );
}

bool Collider::isPointInside(const Vec2d& point) const
{
    return (distanceTo(point) <= rayon_);
}

bool Collider::operator>(const Collider& body2) const
{
    if (isColliderInside(body2)) {
        return true;
    }

    return false;
}

bool Collider::operator|(const Collider& body2) const
{
    if (isColliding(body2)) {
        return true;
    }

    return false;
}

bool Collider::operator>(const Vec2d& point) const
{
    if (isPointInside(point)) {
        return true;
    }

    return false;
}

ostream& operator<<(ostream& sortie, const Collider& col)
{
    sortie << "Collider: position = <" << col.getPosition() << ">"
           << "radius = <" << col.getRadius() << ">" << endl ;

    return sortie;
}

/* --------------------------------------------------------------------
 * FONCTIONS INDEPENDANTES DE LA CLASSE COLLIDER
 */

void clamping(Vec2d& position)
{
    auto worldSize = getApp().getWorldSize();
    auto width  = worldSize.x; // largeur
    auto height = worldSize.y; // hauteur

    // Fonctionne comme des "modulos"
    while(position.x < 0) {
        position.x += width;
    }
    while(position.x > width) {
        position.x -= width;
    }
    while(position.y < 0) {
        position.y += height;
    }
    while(position.y > height) {
        position.y -= height;
    }
}





