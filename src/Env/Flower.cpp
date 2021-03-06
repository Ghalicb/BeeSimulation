#include "Flower.hpp"
#include <Application.hpp>
#include "Random/Random.hpp"
#include "../Utility/Utility.hpp"
#include "Env.hpp"



Flower::Flower(Vec2d pos, double rayon, double pollen)
    : Collider(pos, rayon), nbPollen_(pollen)
{
    // Attribue UNE texture à une fleur nouvellement créée
    int indiceMax = (getAppConfig()["simulation"]["flower"]["textures"]).size() - 1;
    indice_ = uniform(0, indiceMax);
}

double Flower::takePollen(double polTaken)
{
    if (polTaken > nbPollen_) {
        double PollenI(nbPollen_);
        nbPollen_ = 0;
        return PollenI;
    }

    else {
        nbPollen_ -= polTaken;
        return polTaken;
    }
}

void Flower::drawOn(sf::RenderTarget& target) const
{

    auto const& texture = getAppTexture(((getAppConfig()["simulation"]["flower"]["textures"])[indice_]).toString());

    /* On choisit ici  exprès de recourir aux getters (ce qui est
     * négligeablement couteux) à la place de faire un protected
     * (néfaste à l'encapsulation).
    */
    auto flowerSprite = buildSprite(getPosition(), getRadius(), texture);
    target.draw(flowerSprite);

}

void Flower::update(sf::Time dt)
{
    double humidity = getAppEnv().getHumidity(getPosition());
    double seuilHum((getAppConfig()["simulation"]["flower"]["growth"]["threshold"]).toDouble());
    nbPollen_ += dt.asSeconds() * log(humidity/seuilHum);

    double seuilSplit((getAppConfig()["simulation"]["flower"]["growth"]["split"]).toDouble());
    // S'il y a assez de pollen, la plante se reproduit
    if (nbPollen_ >= seuilSplit) {
        int compteTry(0);
        bool stop(false);
        double distanceF;
        Vec2d posNewFlo;

        // Tente de se poser aléatoirement tant que cela n'a pas été possible
        do {
            distanceF = uniform(0.5 * getRadius(), 2 * getRadius());
            posNewFlo = getPosition() + Vec2d::fromRandomAngle() * distanceF;
            stop = getAppEnv().addFlowerAt(posNewFlo);
            ++compteTry;

            if (compteTry == 100) {
                stop = true;
            }

        } while(stop == false);

        nbPollen_ /= 2;
    }

}

double Flower::getPollen() const
{
    return nbPollen_;
}
