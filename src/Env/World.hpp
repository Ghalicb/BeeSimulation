#ifndef WORLD_HPP
#define WORLD_HPP
#include <vector>
#include <SFML/Graphics.hpp>
#include "Application.hpp"
#include "../Utility/Vertex.hpp"


enum class Kind : short { herbe, eau, roche };

struct Seed {
    // Position de la graîne dans cells_
    sf::Vector2i position;
    Kind type;
};

class World final
{
public:
    const j::Value& getPath() const;
    /* Pour cette classe pas de constructeur explicite, on utilise le
    * constructeur par défaut par défaut.
    * => reloadConfig (ré)initialise les attributs.
    */
    void reloadConfig();
    World() = default;
    World(const World&) = delete;
    World& operator=(World&) = delete;
    ~World();
    void drawOn(sf::RenderTarget& target) const;
    // Adapte la taille du texte selon le "zoom" (appX.json) du monde
    void textResize(int& textsize) const;
    // Créé un renderingCache intermédiaire pour les textures du monde
    void reloadCacheStrucure();
    // S'occupe de répartir les textures (humidité comprise)
    void updateCache();
    /* Réinitialise le monde en posant de nouvelles graines
     * Si regenerate => Applique steps() et smooths()
     */
    void reset(bool regenarate = true);
    // Transforme une pos(x,y) en indice de cells_
    int changeRef(int i, int j) const;
    float getCellSize() const;

    // Charge une sauvegarde de terrain
    void loadFromFile();

    // Permet aux graînes de se déplacer (aléatoirement)
    void step();
    void moveRandom(int i);
    void steps (int nbStep, bool regenerate = false);
    // Permet de lisser le terrain (pas laisser de cellules individuelles)
    void smooth();
    void smooths (int nbSmooth, bool regenerate = false);
    // Enregistre les données d'une map dans un fichier
    void saveToFile() const;

    // Attribue une valeur d'humidité à chaque cellule (en fonction des autres)
    void setHumidity(const int& x, const int& y);
    // Détermine si on peut poser un élément sur une zone du monde
    bool isGrowable(const Vec2d& p) const;
    double getHumidity(const Vec2d& pos) const;
    // Détermine si on peut voler à la position pos
    bool isFlyable(const Vec2d& pos) const;
    // Détermine si on peut poser une ruche à la position "position"
    bool isHiveable(const Vec2d& position, double rayon) const;
    // Fonction utilise pour drawHiveAbleZone() de Env
    std::vector<int> indexesForRect(sf::Vector2i const& topLeft, sf::Vector2i const& bottomRight) const;
    void clampVector2i(sf::Vector2i& kase) const;



private:
    // Nb de lignes du tableau (et de colonnes => carré)
    int nbCells_;
    // Tableau contenant l'ensemble des cellules de notre terrain
    std::vector<Kind> cells_;
    // Taille graphique de la cellule
    float cellSize_;
    // RenderingCache_ est l'intermediaire de target
    sf::RenderTexture renderingCache_;

    std::vector<sf::Vertex> grassVertexes_;
    std::vector<sf::Vertex> waterVertexes_;
    std::vector<sf::Vertex> rockVertexes_;

    // Ensemble des graines init présentes dans la grille
    std::vector<Seed> seeds_;
    int nbWaterSeeds_;
    int nbGrassSeeds_;

    // Tableau contenant les valeurs d'humidité de chaque cellule
    std::vector<double> humCells_;
    double humidityRange_;
    int mu_;
    int lambda_;
    std::vector<sf::Vertex> humidityVertexes_;


};


#endif
