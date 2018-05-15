#include <iostream>
#include "../Utility/Vec2d.hpp"
#ifndef COLLIDER_HPP
#define COLLIDER_HPP

class Collider
{
public:
    Collider (Vec2d pos, double ray);
    Collider (const Collider& ColCopie);
    double getRadius() const;
    const Vec2d& getPosition() const;
    Collider& operator=(const Collider& Col2);
    // Appelle la fonction clamping, utilisé sans paramètre !
    void clamping();

    // Renvoie un vecteur entre from (instance courante) et to
    Vec2d directionTo(const Vec2d& to) const;
    Vec2d directionTo(const Collider& toC) const;
    // Permet d'obtenir la longueur entre from et to
    double distanceTo(const Vec2d& to) const;
    double distanceTo(const Collider& toC) const;
    // Déplace la position_ (de from à to par exemple)
    void move(const Vec2d& dx);
    // Permet d'écrire += pour appliquer move à un Vec2d
    void operator+=(const Vec2d& dx);

    // Test si un Collider se trouve dans le Collider de l'instance courante
    bool isColliderInside(const Collider& autre) const;
    // Test si un Collider est en collision avec le Collider de l'instance courante
    bool isColliding(const Collider& autre) const;
    // Test si un point est à l'intérieur du Collider de l'instance courante
    bool isPointInside(const Vec2d& point) const;
    // Permet d'écrire > pour tester "isColliderInside"
    bool operator>(const Collider& body2) const;
    // Permet d'écrire | pour tester "isColliding"
    bool operator|(const Collider& body2) const;
    // Permet d'écrire > pour tester "isPointInside"
    bool operator>(const Vec2d& point) const;


private:
    Vec2d position_;
    double rayon_;

};

// Affiche les caractéristiques d'un Collider (Position et rayon)
std::ostream& operator<<(std::ostream& sortie, const Collider& col);

// Fonction posant le cadre torique au monde pour un Vec2d
void clamping(Vec2d& position);

#endif
