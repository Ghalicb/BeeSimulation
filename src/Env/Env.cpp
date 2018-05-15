#include "Env.hpp"
#include <array>
#include "Random/Random.hpp"
#include "../Utility/Utility.hpp"
#include "Collider.hpp"


using namespace std;


sf::Vector2i Env::posToIndex(const Vec2d& pos) const
{
    int x(pos.x/field_.getCellSize());
    int y(pos.y/field_.getCellSize());

    sf::Vector2i index(x,y);
    return index;
}


void Env::update(sf::Time dt)
{
    if (finish_ == false) {
        auto flowersInit = flowers_;

        for(auto& flower : flowersInit) {
            flower->update(dt);
        }

        for(auto& flower : flowers_) {
            if((*flower).getPollen() <= 0) {
                delete flower;
                flower = nullptr;
            }
        }
        flowers_.erase(std::remove(flowers_.begin(), flowers_.end(), nullptr),flowers_.end());


        for(auto& hive : hives_) {
            if (hive->getNectar() != 0) {
                hive->update(dt);
            } else {
                delete hive;
                hive = nullptr;
            }
            hives_.erase(std::remove(hives_.begin(), hives_.end(), nullptr),hives_.end());
        }

        FlowGen_.update(dt);
    }

    else {
        musicVictory(dt);
    }
}

void Env::drawOn(sf::RenderTarget& target)
{

    if (!(musicV1.getStatus() == sf::Music::Playing)) {
        changeMusic();
    }

    field_.drawOn(target);

    for (size_t i(0) ; i < flowers_.size() ; ++i) {
        flowers_[i]->drawOn(target);
    }

    for (auto& hive : hives_) {
        // Faire varier l'ordre (avec le paramètre mPaused) pour cacher les abeilles au début

        if ((getApp().getMPaused() == false) or (coco2_ != 1)) {
            hive->drawOn(target);
        }

        for (auto& bee : hive->getBees()) {
            bee->drawOn(target);
        }

        if ((getApp().getMPaused() == true) and (coco2_ == 1)) {
            hive->drawOn(target);
        }
    }
}

void Env::reset()
{
    reloadMusicConfig(1);

    field_.reset(true);
    destroyFlowers();
    FlowGen_.reset();
    destroyHives();

    musicL1.stop();
}

void Env::loadWorldFromFile()
{
    destroyFlowers();
    destroyHives();

    reloadMusicConfig(2);

    try {
        field_.loadFromFile();
    } catch (std::runtime_error) {
        reset();
    }
}

void Env::saveWorldToFile() const
{
    field_.saveToFile();
}

Env::Env()
{
    coco_ = 1;
    coco2_ = 1;
    coco3_ = 1;
    coco4_ = 1;
    finish_ = false;
    debugOnRepresseur_ = false;
    loadWorldFromFile();
}

Env::~Env()
{
    destroyFlowers();
    destroyHives();
}

/* ---------------------------------------------------------------------
 * GESTION DES FLEURS
 */

void Env::drawFlowerZone(sf::RenderTarget& target, Vec2d const& position) const
{
    double thickness = 5.0;
    double size = (getAppConfig()["simulation"]["env"]["initial"]["flower"]["size"]["manual"]).toDouble();

    sf::Color color(0,0,0);
    Collider pointer(position, size);

    // Vert quand on est sur de l'herbe + pas en "collision" avec une ruche !
    if ( (field_.isGrowable(position)) and (getCollidingHive(pointer) == nullptr) ) {
        color = sf::Color::Green;
    } else {
        color = sf::Color::Red;
    }

    auto shape = buildAnnulus(position, size, color, thickness);
    target.draw(shape);

}

bool Env::addFlowerAt(const Vec2d& pos)
{
    int Maxflower((getAppConfig()["simulation"]["env"]["max flowers"]).toInt());
    double rayon((getAppConfig()["simulation"]["env"]["initial"]["flower"]["size"]["manual"]).toDouble());
    Collider bodyFleur(pos, rayon);
    if((field_.isGrowable(pos)) and (flowers_.size() < Maxflower) and (getCollidingHive(bodyFleur) == nullptr)) {
        array<double,2> tabPollen = {
            (getAppConfig()["simulation"]["env"]["initial"]["flower"]["nectar"]["min"]).toDouble(),
            (getAppConfig()["simulation"]["env"]["initial"]["flower"]["nectar"]["max"]).toDouble()
        };

        double qtpollen(tabPollen[uniform(0,1)]);

        Flower* fleur = new Flower(pos, rayon, qtpollen);
        flowers_.push_back(fleur);

        return true;
    }

    return false;
}

void Env::destroyFlowers()
{
    for (auto& flower : flowers_) {
        delete flower;
    }

    flowers_.clear();

}

double Env::getHumidity(const Vec2d& pos) const
{
    return field_.getHumidity(pos);
}

int Env::getNbFlower() const
{
    return flowers_.size();
}

/* ---------------------------------------------------------------------
 * GESTION DES RUCHES & ABEILLES
 */

std::vector<Hive*> Env::getHives() const
{
    return hives_;
}

bool Env::addHiveAt(const Vec2d& position)
{
    double rayon(getAppConfig()["simulation"]["env"]["initial"]["hive"]["size"]["manual"].toDouble());
    double factor(getAppConfig()["simulation"]["env"]["initial"]["hive"]["hiveable factor"].toDouble());
    Collider bodyHive(position, rayon * factor);
    if(field_.isHiveable(position, rayon * factor)) {
        if( (getCollidingHive(bodyHive) == nullptr) and (getCollidingFlower(bodyHive) == nullptr) ) {
            hives_.push_back(new Hive(position, rayon));
            return true;
        }
    }
    return false;
}

void Env::destroyHives()
{
    for (auto& hive : hives_) {
        delete hive;
    }

    hives_.clear();

}

Hive* Env::getCollidingHive(const Collider& body) const
{
    double rayon((getAppConfig()["simulation"]["env"]["initial"]["hive"]["size"]["manual"]).toDouble());
    double factor((getAppConfig()["simulation"]["env"]["initial"]["hive"]["hiveable factor"]).toDouble());

    for (auto& hive: hives_) {
        Collider bodytest((*hive).getPosition(), rayon * factor);
        if(bodytest.isColliding(body)) {
            return hive;
        }
    }

    return nullptr;
}

Flower* Env::getCollidingFlower(const Collider& body) const
{
    for (auto& flower: flowers_) {
        if((*flower).isColliding(body)) {
            return flower;
        }
    }

    return nullptr;
}

bool Env::isFlyable(const Vec2d& pos) const
{
    return field_.isFlyable(pos);
}

void Env::drawHiveableZone(sf::RenderTarget& target, Vec2d const& position) const
{
    auto worldSize = getApp().getWorldSize();
    auto width  = worldSize.x; // largeur
    auto height = worldSize.y; // hauteur
    double rayon((getAppConfig()["simulation"]["env"]["initial"]["hive"]["size"]["manual"]).toDouble());
    double factor((getAppConfig()["simulation"]["env"]["initial"]["hive"]["hiveable factor"]).toDouble());
    double Rayon = rayon*factor;
    Collider bodyHive(position, Rayon);

    double thickness = 5.0;
    sf::Color color(0,0,0);
    if ((getCollidingHive(bodyHive) != nullptr) or (getCollidingFlower(bodyHive) != nullptr)) {
        color = sf::Color::Blue;

    } else if(field_.isHiveable(position, Rayon)) {
        color = sf::Color::Green;
    } else {
        color = sf::Color::Red;
    }

    Vec2d topLeft(position.x - Rayon, position.y - Rayon);
    clamping(topLeft);
    Vec2d bottomRight(position.x + Rayon, position.y + Rayon);
    clamping(bottomRight);
    if(topLeft.x < bottomRight.x and topLeft.y < bottomRight.y) {
        sf::RectangleShape shape0(buildRectangle(topLeft, bottomRight, color, thickness));
        target.draw(shape0);
    }
    if(topLeft.x < bottomRight.x and topLeft.y > bottomRight.y) {
        Vec2d p1(topLeft.x,0.0);
        Vec2d p2(bottomRight.x, height);
        sf::RectangleShape shape1(buildRectangle(p1, bottomRight, color, thickness));
        target.draw(shape1);
        sf::RectangleShape shape2(buildRectangle(topLeft,p2 , color, thickness));
        target.draw(shape2);
    }

    if(topLeft.x > bottomRight.x and topLeft.y < bottomRight.y) {
        Vec2d p3(width, bottomRight.y);
        Vec2d p4(0.0, topLeft.y);
        sf::RectangleShape shape3(buildRectangle(topLeft, p3, color, thickness));
        target.draw(shape3);
        sf::RectangleShape shape4(buildRectangle(p4, bottomRight, color, thickness));
        target.draw(shape4);
    }

    if(topLeft.x > bottomRight.x and topLeft.y > bottomRight.y) {
        Vec2d p5(0.0,0.0);
        sf::RectangleShape shape5(buildRectangle(p5, bottomRight, color, thickness));
        target.draw(shape5);

        Vec2d p6(topLeft.x, 0.0);
        Vec2d p7(width, bottomRight.y);
        sf::RectangleShape shape6(buildRectangle(p6, p7, color, thickness));
        target.draw(shape6);

        Vec2d p8(0.0, topLeft.y);
        Vec2d p9(bottomRight.x, height);
        sf::RectangleShape shape7(buildRectangle(p8, p9, color, thickness));
        target.draw(shape7);

        Vec2d p10(width, height);
        sf::RectangleShape shape8(buildRectangle(topLeft, p10, color, thickness));
        target.draw(shape8);
    }
}

/* ---------------------------------------------------------------------
 * GESTION DE LA MUSIQUE
 */

void Env::setFinish(bool fin)
{
    finish_ = fin;
}

void Env::musicStatic()
{
    if (musicU1.getStatus() == sf::Music::Playing) {
        musicU1.stop();
    }

    if (coco_ == 1) {
        if (!musicS1.openFromFile("res/"+getAppConfig()["simulation"]["audio"]["static"]["1"].toString())) {
            cerr << "Le fichier audio n'a pas pu s'ouvrir correctement" << endl;
        }
        ++coco_;
    }

    if (!(musicS1.getStatus() == sf::Music::Playing)) {
        musicS1.play();
        musicS1.setLoop(true);
    }
}

void Env::musicUpdateOn()
{
    if (musicS1.getStatus() == sf::Music::Playing) {
        musicS1.stop();
    }

    if (coco2_ == 1) {
        if (!musicU1.openFromFile("res/"+getAppConfig()["simulation"]["audio"]["update"]["1"].toString())) {
            cerr << "Le fichier audio n'a pas pu s'ouvrir correctement" << endl;
        }
        ++coco2_;
    }

    if ( !(musicU1.getStatus() == sf::Music::Playing) ) {
        musicU1.play();
    }
}

void Env::musicVictory(sf::Time dt)
{
    if (musicU1.getStatus() == sf::Music::Playing) {
        musicU1.stop();
    }

    if (musicS1.getStatus() == sf::Music::Playing) {
        musicS1.stop();
    }

    if (coco3_ == 1) {
        if (!musicV1.openFromFile("res/"+getAppConfig()["simulation"]["audio"]["victory"]["1"].toString())) {
            cerr << "Le fichier audio n'a pas pu s'ouvrir correctement" << endl;
        }

        if (!(musicV1.getStatus() == sf::Music::Playing)) {
            musicV1.play();
        }

        ++coco3_;
    }

    if (musicV1.getStatus() == sf::Music::Playing) {
        for (auto& hive : hives_) {
            for (auto& bee : hive->getBees()) {
                bee->changeAngle(dt);
            }
        }

        debugOnRepresseur_ = true;
    } else {
        getApp().setMPaused(true);
        reset();
    }

}

void Env::musicLoading()
{
    if (musicS1.getStatus() == sf::Music::Playing) {
        musicS1.stop();
    }

    if (musicU1.getStatus() == sf::Music::Playing) {
        musicU1.stop();
    }

    if (musicV1.getStatus() == sf::Music::Playing) {
        musicV1.stop();
    }

    if (coco4_ == 1) {
        if (!musicL1.openFromFile("res/"+getAppConfig()["simulation"]["audio"]["loading"]["1"].toString())) {
            cerr << "Le fichier audio n'a pas pu s'ouvrir correctement" << endl;
        }
        ++coco4_;
    }

    if (!(musicL1.getStatus() == sf::Music::Playing)) {
        musicL1.play();
        musicL1.setLoop(true);
    }
}

void Env::changeMusic()
{
    if (getApp().getMPaused() == true) {
        musicStatic();
    }

    if (getApp().getMPaused() == false) {
        musicUpdateOn();
    }
}

void Env::reloadMusicConfig(int whichMusic)
{
    if (musicV1.getStatus() == sf::Music::Playing) {
        musicV1.stop();
    }

    finish_ = false;
    debugOnRepresseur_ = false;
    coco2_ = 1;
    coco3_ = 1;
    getApp().setMPaused(true);

    if (whichMusic == 1) {
        musicLoading();
    }

    else {
        changeMusic();
    }


}
bool Env::stopDebugOn() const
{
    return debugOnRepresseur_;
}
