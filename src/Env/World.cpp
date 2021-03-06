#include <iostream>
#include <fstream>
#include <array>
#include "World.hpp"
#include "Random/Random.hpp"
#include "Utility/Utility.hpp"
#include "Env.hpp"


using namespace std;



/* ---------------------------------------------------------------------
 * BASE GRAPHIQUE POUR LA MISE EN PLACE DE TERRAINS
 */

const j::Value& World::getPath() const
{
    return getAppConfig()["simulation"]["world"];
}

void World::reloadConfig()
{
    // .toInt() nécessaire, car on transforme un j:Value en un nbCells_ (int).
    nbCells_ = getPath()["cells"].toInt() ;
    cellSize_ = (getPath()["size"].toDouble()) / nbCells_ ;
    cells_.assign(nbCells_* nbCells_ , Kind::roche);
    humCells_.assign(nbCells_*nbCells_, 0);

    nbWaterSeeds_ = getPath()["seeds"]["water"].toInt();
    nbGrassSeeds_ = getPath()["seeds"]["grass"].toInt();
    seeds_ = vector<Seed>(nbWaterSeeds_ + nbGrassSeeds_);

    humidityRange_ = 0;

    double seuil = getPath()["generation"]["humidity"]["threshold"].toDouble();	// SOLUTION : Double instead of int
    mu_ = getPath()["generation"]["humidity"]["initial level"].toInt();
    lambda_ = getPath()["generation"]["humidity"]["decay rate"].toInt();

    while (mu_*exp(-humidityRange_/lambda_) > seuil) {
        ++humidityRange_;
    }

}

World::~World()
{}

void World::drawOn(sf::RenderTarget& target) const
{

    if ((getPath()["show humidity"]).toBool() == true) {
        target.clear();
        target.draw(humidityVertexes_.data(), humidityVertexes_.size(), sf::Quads);
        if (isDebugOn()) {
            Vec2d curseur(getApp().getCursorPositionInView());

            double x = curseur.x / cellSize_;
            double y = curseur.y / cellSize_;
            int textsize(0);
            textResize(textsize);

            auto const text = buildText( to_nice_string(humCells_[changeRef(x,y)]), curseur, getAppFont(), textsize, sf::Color::White);
            target.draw(text);

        }
    } else {
        sf::Sprite cache(renderingCache_.getTexture());
        target.draw(cache);

        if (isDebugOn()) {
            int textsize(0);
            textResize(textsize);

            sf::Color color(0,0,0);

            array<sf::Color, 4> colors;
            colors[0] = sf::Color::Red;
            colors[1] = sf::Color(255,135,0);
            colors[2] = sf::Color::Green;
            colors[3] = sf::Color(255,215,0);

            for (auto hive : getAppEnv().getHives()) {
                if (hive->getNectar() <= 250) {
                    color = colors[0];
                }

                else if (hive->getNectar() <= 750) {
                    color = colors[1];
                }

                else if (hive->getNectar() < 2500) {
                    color = colors[2];
                } else if (hive->getNectar() >= 2500) {
                    color = colors[3];
                }

                auto const text = buildText( to_nice_string(hive->getNectar()), hive->getPosition() + Vec2d(0.0, -35.0), getAppFont(), textsize, color);
                target.draw(text);

                if (hive->getNectar() >= 2500) {
                    color = colors[uniform(0,3)];
                    auto const text2 = buildText( "WINNER", hive->getPosition(), getAppFont(), 1.7 * textsize, color);
                    target.draw(text2);
                }

            }

        }
    }

}


void World::textResize(int& textsize) const
{
    if (nbCells_ == 500) { // Faire avec les switch serait plus pratique pour place un default: (cas général)
        textsize = 0.5 * nbCells_;
    }
    if (nbCells_ == 200) {
        textsize = 0.125 * nbCells_;
    }
    if (nbCells_ == 40) {
        textsize = 2 * nbCells_;
    }
}


void World::reloadCacheStrucure()
{
    renderingCache_.create(nbCells_ * cellSize_, nbCells_ * cellSize_);
    // Creation de allVertexes pour ne pas duppliquer de codes
    auto textures = getPath()["textures"];
    vector<sf::Vertex> allVertexes = generateVertexes(textures, nbCells_, cellSize_);
    grassVertexes_ = allVertexes;
    waterVertexes_ = allVertexes;
    rockVertexes_ = allVertexes;
    humidityVertexes_ =	allVertexes;
}

void World::updateCache()
{
    renderingCache_.clear();
    sf::RenderStates rs;
    auto textures = getPath()["textures"];

    double humMin(INFINITY);
    double humMax(-INFINITY);
    for (unsigned int i(0); i < humCells_.size(); ++i) {
        if (humCells_[i] < humMin ) {
            humMin = humCells_[i];
        }
        if (humCells_[i] > humMax ) {
            humMax = humCells_[i];
        }
    }


    for(int i(0); i < nbCells_; ++i) {
        for(int j(0); j < nbCells_; ++j) {
            vector<size_t> tmp;
            tmp = indexesForCellVertexes(i, j, nbCells_);

            double niveau_bleu = (humCells_[changeRef(i,j)] - humMin) / (humMax - humMin) * 255;

            for (int l(0) ; l < 4 ; ++l) {
                humidityVertexes_[tmp[l]].color = sf::Color(0, 0, niveau_bleu);
            }

            if (cells_[(nbCells_*j)+i] == Kind::roche) {
                for (int k(0) ; k < 4 ; ++k) {
                    rockVertexes_[tmp[k]].color.a = 255;
                    waterVertexes_[tmp[k]].color.a = 0;
                    grassVertexes_[tmp[k]].color.a = 0;
                }
            }
            if (cells_[(nbCells_*j)+i] == Kind::eau) {
                for (int k(0) ; k < 4 ; ++k) {
                    rockVertexes_[tmp[k]].color.a = 0;
                    waterVertexes_[tmp[k]].color.a = 255;
                    grassVertexes_[tmp[k]].color.a = 0;
                }

            }
            if(cells_[(nbCells_*j)+i] == Kind::herbe) {
                for (int k(0) ; k < 4 ; ++k) {
                    rockVertexes_[tmp[k]].color.a = 0;
                    waterVertexes_[tmp[k]].color.a = 0;
                    grassVertexes_[tmp[k]].color.a = 255;
                }

            }
        }
    }

    rs.texture = &getAppTexture(textures["rock"].toString());
    renderingCache_.draw(rockVertexes_.data(), rockVertexes_.size(), sf::Quads, rs);

    rs.texture = &getAppTexture(textures["water"].toString());
    renderingCache_.draw(waterVertexes_.data(), waterVertexes_.size(), sf::Quads, rs);

    rs.texture = &getAppTexture(textures["grass"].toString());
    renderingCache_.draw(grassVertexes_.data(), grassVertexes_.size(), sf::Quads, rs);

    renderingCache_.display();

}

void World::reset(bool regenarate)
{

    reloadConfig();

    for (int i(0) ; i < nbWaterSeeds_ ; ++i) {
        sf::Vector2i pos(uniform(0, nbCells_-1), uniform(0, nbCells_-1));

        seeds_[i].type = Kind::eau ;
        seeds_[i].position = pos;
        cells_[changeRef(pos.x, pos.y)] = Kind::eau;

    }

    for (unsigned int i(nbWaterSeeds_); i < seeds_.size()-1 ; ++i) {

        sf::Vector2i pos(uniform(0, nbCells_-1), uniform(0, nbCells_-1));

        seeds_[i].type = Kind::herbe ;
        seeds_[i].position = pos;
        if( cells_[changeRef(pos.x, pos.y)] != Kind::eau ) {
            cells_[changeRef(pos.x, pos.y)] = Kind::herbe;
        }

    }

    if (regenarate) {
        steps((getPath()["generation"]["steps"]).toInt(), false);
        smooths((getPath()["generation"]["smoothness"]["level"]).toInt(), false);
    }

    reloadCacheStrucure();
    updateCache();
}

int World::changeRef(int i, int j) const
{
    return (nbCells_*j) + i;
}

float World::getCellSize() const
{
    return cellSize_;
}
/* ---------------------------------------------------------------------
 * CHARGEMENT DE TERRAINS DEPUIS DES FICHIERS
 */

void World::loadFromFile()
{
    reloadConfig();
    ifstream input;
    input.open(getApp().getResPath()+getPath()["file"].toString());

    if (input.fail()) {
        throw std::runtime_error("Le fichier n'a pas pu s'ouvrir correctement");
    }

    input >> nbCells_ >> std::ws;
    input >> cellSize_ >> std::ws;

    humCells_.resize(nbCells_*nbCells_);
    cells_.resize(nbCells_*nbCells_);

    short var;

    for (int i(0) ; i < (nbCells_* nbCells_) ; ++i) {
        input >> var >> std::ws;
        // Convertie var de short à Kind
        cells_[i] = static_cast<Kind>(var);

    }

    for (unsigned int i(0) ; i < humCells_.size() ; ++i) {
        input >> humCells_[i] >> std::ws;
    }

    input.close();
    reloadCacheStrucure();
    updateCache();
}

/* ---------------------------------------------------------------------
 * GENERATION ALEATOIRE DE TERRAINS
 */

void World::step()
{
    for (unsigned int i(0) ; i < seeds_.size() ; ++i) {
        if (seeds_[i].type == Kind::herbe) {
            moveRandom(i);
        }

        if (seeds_[i].type == Kind::eau) {
            double probT(bernoulli((getPath()["seeds"]["water teleport probability"]).toDouble()));

            if (probT == 0) {
                moveRandom(i);
            }

            else {
                // Permet de téléporter la graîne à une pos aléatoire
                sf::Vector2i pos(uniform(0, nbCells_-1), uniform(0, nbCells_-1));
                seeds_[i].position = pos;
            }

        }

    }

}

void World::moveRandom(int i)
{
    Seed seedBack(seeds_[i]);

    // Si la coordonnée de x ne change pas rentrée dans do/while
    seeds_[i].position.x += uniform(-1, 1);
    if (seeds_[i].position.x == seedBack.position.x) {
        // Recalculer la coordonnée de y tant qu'elle est nulle
        // (on veut éviter un déplacement de +(0, 0))
        do {
            seeds_[i].position.y += uniform(-1, 1);
        } while (seeds_[i].position.y == seedBack.position.y);
    }

    // Si la graîne sort du cadre, faire l'équivalent de ctrl Z
    if ((seeds_[i].position.x > nbCells_ -1) or (seeds_[i].position.x < 0)
        or (seeds_[i].position.y > nbCells_ -1) or (seeds_[i].position.y < 0)) {
        seeds_[i] = seedBack;
    }

    int j;
    j = (seeds_[i].position.y * nbCells_) + seeds_[i].position.x;
    cells_[j] = seeds_[i].type;

    int y = seeds_[i].position.y;
    int x = seeds_[i].position.x;

    if (cells_[j] == Kind::eau) {
        setHumidity(x, y);
    }

}

void World::steps (int nbStep, bool regenerate)
{
    for (int i(0) ; i < nbStep ; ++i) {
        step();
    }

    if (regenerate) {
        updateCache();
    }
}

void World::smooth()
{
    auto copyCells = cells_;
    double ratiowat((getPath()["generation"]["smoothness"]["water neighbourhood ratio"]).toDouble());
    double ratiogra((getPath()["generation"]["smoothness"]["grass neighbourhood ratio"]).toDouble());

    // On néglige les bords de cells_
    for( int j(1); j < nbCells_-1; ++j) {
        for(int i(1); i < nbCells_-1; ++i) {

            array<int,8> tabVoisin = {
                changeRef(i-1,j-1),
                changeRef(i-1,j),
                changeRef(i-1,j+1),
                changeRef(i,j-1),
                changeRef(i,j+1),
                changeRef(i+1,j-1),
                changeRef(i+1,j),
                changeRef(i+1,j+1)
            };

            int indice(changeRef(i,j));
            int compteurE(0);
            int compteurH(0);

            for (unsigned int k(0) ; k < tabVoisin.size() ; ++k) {
                if (cells_[tabVoisin[k]] == Kind::eau) {
                    ++compteurE;
                }
                if (cells_[tabVoisin[k]] == Kind::herbe) {
                    ++compteurH;
                }
            }

            if((cells_[indice] != Kind::eau) and (((double)compteurE/8.0) > ratiowat)) {
                copyCells[indice] = Kind::eau;
                setHumidity(i,j);
            }

            if ((copyCells[indice] == Kind::roche) and (((double)compteurH/8.0) > ratiogra)) {
                copyCells[indice] = Kind::herbe;
            }

        }

    }

    std::swap(cells_,copyCells);

}



void World::smooths (int nbSmooth, bool regenerate)
{
    for (int i(0) ; i < nbSmooth ; ++i) {
        smooth();
    }

    if (regenerate) {
        updateCache();
    }

}

void World::saveToFile() const
{
    ofstream sortie;
    sortie.open(getApp().getResPath()+getPath()["file"].toString());

    if (sortie.fail()) {
        throw std::runtime_error("Le fichier n'a pas pu s'ouvrir correctement");
    }

    sortie << nbCells_ << endl;
    sortie << cellSize_ << endl;

    short var;

    for (int i(0) ; i < (nbCells_* nbCells_) ; ++i) {
        var = static_cast<short>(cells_[i]);
        sortie << var << " " << flush;
    }

    for (unsigned int i(0) ; i < humCells_.size() ; ++i) {
        sortie << humCells_[i] << " " << flush;
    }

    sortie.close();

}

/* ---------------------------------------------------------------------
 * COORDINATION ENTRE MONDE ET "HABITAT"
 */

void World::setHumidity(const int& x, const int& y)
{
    // Trace la zone autour de c1 (selon le rayon)
    for (int i(-humidityRange_); i<= humidityRange_ ; ++i) {
        for( int k(-humidityRange_); k<= humidityRange_ ; ++k) {
            // Pour prendre en compte que des c2 dans le tableau cells_
            if( (x+i >= 0 and x+i < nbCells_) and (y+k >= 0 and y+k < nbCells_) ) {
                double distance(std::hypot(x-(x+i),y-(y+k)) );
                // Ajoute une part d'humidité de c1 à c2
                humCells_[changeRef(x+i,y+k)] += mu_*exp(-distance/lambda_);
            }
        }
    }
}

bool World::isGrowable(const Vec2d& pos) const
{
    sf::Vector2i posCoord(getAppEnv().posToIndex(pos));
    // Empêche la plantation de fleur en dehors du monde
    if( posCoord.x < nbCells_ and posCoord.y < nbCells_) {
        int posInd = changeRef(posCoord.x, posCoord.y);

        if(cells_[posInd] == Kind::herbe) {
            return true;
        }
    }

    return false;
}

double World::getHumidity(const Vec2d& pos) const
{
    sf::Vector2i posHumCoord = getAppEnv().posToIndex(pos);
    int posHumInd = changeRef(posHumCoord.x, posHumCoord.y);

    return humCells_[posHumInd];
}

bool World::isHiveable(const Vec2d& position, double rayon) const
{
    // carre dans lequel on travaille (topleft et bottomright)
    Vec2d topLeft(position.x - rayon, position.y - rayon);
    Vec2d bottomRight(position.x + rayon, position.y + rayon);
    sf::Vector2i topLeft_I = getAppEnv().posToIndex(topLeft);
    sf::Vector2i bottomRight_I = getAppEnv().posToIndex(bottomRight);

    vector<int> IndicesCells = indexesForRect(topLeft_I, bottomRight_I);
    for(auto& indice : IndicesCells) {
        if(cells_[indice] != Kind::herbe) {
            return false;
        }
    }
    return true;
}

vector<int> World::indexesForRect(sf::Vector2i const& topLeft, sf::Vector2i const& bottomRight) const
{
    vector<int> tab;

    for(int x = topLeft.x; x <= bottomRight.x; ++x) {
        for(int y = topLeft.y; y <= bottomRight.y; ++y) {
            sf::Vector2i kase(x,y);
            clampVector2i(kase);
            tab.push_back(changeRef(kase.x,kase.y));  // pk const dans changeref? et size-t ?
        }
    }
    return tab;
}

void World::clampVector2i(sf::Vector2i& kase) const
{
    if(kase.x < 0) {
        kase.x += nbCells_;
    }
    if(kase.x > nbCells_-1) {
        kase.x -= nbCells_;
    }
    if(kase.y < 0) {
        kase.y += nbCells_;
    }
    if(kase.y > nbCells_-1) {
        kase.y -= nbCells_;
    }
}


bool World::isFlyable(const Vec2d& pos) const
{
    sf::Vector2i posCoord(getAppEnv().posToIndex(pos));
    int posInd = changeRef(posCoord.x, posCoord.y);

    if(cells_[posInd] == Kind::roche) {
        return false;
    }

    return true;

}
