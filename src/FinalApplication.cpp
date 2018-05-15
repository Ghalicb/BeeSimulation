/*
 * prjsv 2016
 * 2013, 2014, 2016
 * Marco Antognini
 */

#include <Config.hpp>
#include <Env/Env.hpp>
#include <FinalApplication.hpp>

#include <cassert>

IMPLEMENT_MAIN(FinalApplication);

void FinalApplication::onRun()
{
    // Setup stats
    // TODO step5 uncomment me
    //addGraph(s::GENERAL, { s::FLOWERS, s::HIVES, s::SCOUTS, s::WORKERS, }, 0, 300);
}

void FinalApplication::onEvent(sf::Event event, sf::RenderWindow&)
{
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
        default:
            break;

        // Add flower: show the area covered by the flower
        case sf::Keyboard::F:
            mShowFlowerZone = true;
            break;

        case sf::Keyboard::H:
            mShowHiveableZone = true;
            break;
        }
    } else if (event.type == sf::Event::KeyReleased) {
        switch (event.key.code) {
        default:
            break;

        // Add flower: try to create a new one and disable visualisation zone
        case sf::Keyboard::F:
            mShowFlowerZone = false;
            if (getEnv().addFlowerAt(getCursorPositionInView())) {
                std::cout << "New flower created\n";
            } else {
                std::cout << "Couldn't create new flower\n";
            }
            break;

        // Add hive: try to create a new one and disable "hiveable" zone
        case sf::Keyboard::H:
            mShowHiveableZone = false;
            if (getEnv().addHiveAt(getCursorPositionInView())) {
                std::cout << "New hive created\n";
            } else {
                std::cout << "Couldn't create new hive\n";
            }
            break;
        }
    }
}

void FinalApplication::onDraw(sf::RenderTarget& target)
{
    if (mShowFlowerZone) {
        auto pos = getCursorPositionInView();
        getEnv().drawFlowerZone(target, pos);
    }

    if (mShowHiveableZone) {
        auto pos = getCursorPositionInView();
        getEnv().drawHiveableZone(target, pos);
    }
}

