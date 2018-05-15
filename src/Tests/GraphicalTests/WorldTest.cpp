/*
 * prjsv 2015
 * 2014
 * Marco Antognini
 */

#include <Application.hpp>
#include <Env/World.hpp>

/*!
 * Test world procedural generation.
 *
 * Initially the world is *not* initialised. One can load a world from a file,
 * or reset the world to initialised the seed and then manually generate the world,
 * or generate a new world using the application settings.
 *
 * Actions:
 *
 *  - I: load world from file
 *
 *  - O: save world to file
 *
 *  - R: reset world -- initialise seeds
 *
 *  - Shift+R: reset world -- generate a new world using cfg settings
 *
 *  - Space: after seeds are initialised, apply one step of the p.g. algorithm
 *
 *  - Shift+Space: idem, but apply 100 steps
 *
 *  - S: after seeds are initialised, apply one smoothing pass
 *
 *  - Shift+S: idem, but apply 10 smoothing passes
 *
 */
class WorldTest : public Application
{
public:
    using Application::Application;

    // Override a few methods
    virtual void onEvent(sf::Event event, sf::RenderWindow& window) override final;
    virtual void onDraw(sf::RenderTarget& target) override final;

private:
    World mWorld;
};

IMPLEMENT_MAIN(WorldTest);

void WorldTest::onEvent(sf::Event event, sf::RenderWindow&)
{
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        default:
            break;

        case sf::Keyboard::I:
            mWorld.loadFromFile();
            break;

        case sf::Keyboard::O:
            mWorld.saveToFile();
            break;

        case sf::Keyboard::R:
            mWorld.reset(event.key.shift ? true : false);
            break;

        case sf::Keyboard::S:
            mWorld.smooths(event.key.shift ? 10 : 1, true);
            break;

        case sf::Keyboard::Space:
            mWorld.steps(event.key.shift ? 100 : 1, true);
            break;
        }
    }
}

void WorldTest::onDraw(sf::RenderTarget& target)
{
    mWorld.drawOn(target);
}
