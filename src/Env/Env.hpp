#ifndef ENV_HPP
#define ENV_HPP
#include "Application.hpp"
#include "World.hpp"
#include "Flower.hpp"
#include "FlowerGenerator.hpp"
#include "Bee.hpp"
#include <SFML/Audio.hpp>


class Env
{
public:
    sf::Vector2i posToIndex(const Vec2d& pos) const;

    // Evolution/Dessin de tout ce que contient l'environnement
    void update(sf::Time dt);
    void drawOn(sf::RenderTarget& target);
    void reset();
    // Charge/Sauvegarde un monde
    void loadWorldFromFile();
    void saveWorldToFile() const;
    Env();
    // On ne veut pas copier ou affecter un Env (trop lourd !)
    Env(const Env&) = delete;
    Env& operator=(Env&) = delete;
    ~Env();

    // Affiche les zones où l'on peut dessiner une fleur
    void drawFlowerZone(sf::RenderTarget& target, Vec2d const& position) const;
    // Ajoute une fleur au tableau de flowers_ (si possible)
    bool addFlowerAt(const Vec2d& pos);
    // Détruit l'adresse des fleurs avant d'effacer le tableau flowers_
    void destroyFlowers();
    // Donne l'humidité d'une cellule (en passant par getHumidity de World)
    double getHumidity(const Vec2d& pos) const;
    // Donne le nombre de fleur présente dans flowers_
    int getNbFlower() const;

    // Ajoute une ruche au tableau de hive_ (si possible)
    bool addHiveAt(const Vec2d& position);
    void destroyHives();
    std::vector<Hive*> getHives() const;
    // Renvoie la ruche/fleur en collision avec le Collider donné
    Hive* getCollidingHive(const Collider& body) const;
    Flower* getCollidingFlower(const Collider& body) const;
    bool isFlyable(const Vec2d& pos) const;
    // Affiche si l'on peut poser une ruche à l'endroit donné
    void drawHiveableZone(sf::RenderTarget& target, Vec2d const& position) const;

    // Methode nécessaire à l'application de musique
    void setFinish(bool fin);
    bool stopDebugOn() const;
    void musicStatic();
    void musicUpdateOn();
    void musicVictory(sf::Time dt);
    void musicLoading();
    void changeMusic();
    void reloadMusicConfig(int whichMusic);


private:
    World field_;
    std::vector<Flower*> flowers_;
    FlowerGenerator FlowGen_;
    std::vector<Hive*> hives_;

    bool finish_;
    // Attributs contenant les différentes musiques :
    // Musique Statique
    sf::Music musicS1;
    // Musique Update
    sf::Music musicU1;
    // Musique de Victoire
    sf::Music musicV1;
    // Mudique de loading
    sf::Music musicL1;
    /* Compteurs permettant de ne pas ouvrir une même musique dans
     * différents canaux (provocant si abus une saturation)
     */
    int coco_;
    int coco2_;
    int coco3_;
    int coco4_;
    // Défini si on veut stopper l'action usuel de debugOn()
    bool debugOnRepresseur_;


};

#endif
