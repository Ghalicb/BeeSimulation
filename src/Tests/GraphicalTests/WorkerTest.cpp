/*
 * prjsv 2015
 * 2014
 * Marco Antognini
 */

#include <Application.hpp>
#include <Env/Env.hpp>
#include <Env/Hive.hpp>
#include <Env/WorkerBee.hpp>

#include <cassert>

/*!
 * This tests the behaviour of workers. When a hive is created
 * (using `H`), a worker is created as well. Note that hives can
 * be created only after a flower was planted.
 */
class WorkerTest : public Application
{
public:
    using Application::Application;

    // Override a few methods
    virtual void onSimulationStart() override final;
    virtual void onEvent(sf::Event event, sf::RenderWindow& window) override final;
    virtual void onDraw(sf::RenderTarget& target) override final;

private:
    bool  mShowFlowerZone   = false;   ///< Enable/disable flower zone visualisation
    bool  mShowHiveableZone = false;   ///< Enable/disable hiveable zone visualisation
    Hive* mLastHive         = nullptr; ///< Keep track of the last hive that was created
    bool  mFlowerCreated    = false;
    Vec2d mFlowerPosition;
};

IMPLEMENT_MAIN(WorkerTest);

void WorkerTest::onSimulationStart()
{
    // Override a few settings
    auto& cfg = getConfig()["simulation"];

    auto& initial = cfg["env"]["initial"];
    initial["flower"]["count"] = j::number(0);
    initial["hive"]["count"] = j::number(0);

    auto& hive = cfg["hive"];
    hive["reproduction"]["nectar threshold"] = j::number(1e9); // a big number

    auto& generator = cfg["flower generator"];
    generator["active"] = j::boolean(false);

    initial["flower"]["nectar"]["min"] = j::number(1e8);
    initial["flower"]["nectar"]["max"] = j::number(1e9);
    cfg["flower"]["growth"]["split"] = j::number(1e10);
}

void WorkerTest::onEvent(sf::Event event, sf::RenderWindow&)
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

        case sf::Keyboard::I:
        case sf::Keyboard::R:
            mFlowerCreated = false;
            break;

        // Add flower: try to create a new one and disable visualisation zone
        case sf::Keyboard::F:
            mShowFlowerZone = false;
            mFlowerPosition = getCursorPositionInView();
            if (getEnv().addFlowerAt(mFlowerPosition)) {
                mFlowerCreated  = true;
                std::cout << "New flower created\n";
            } else {
                std::cout << "Couldn't create new flower\n";
            }
            break;

        case sf::Keyboard::H: {
            // Add hive: try to create a new one and disable "hiveable" zone
            mShowHiveableZone = false;
            auto position = getCursorPositionInView();
            if (mFlowerCreated && getEnv().addHiveAt(position)) {
                auto dummyRadius = 1;
                mLastHive = getEnv().getCollidingHive(Collider(position, dummyRadius));
                assert(mLastHive != nullptr);
                std::cout << "New hive created\n";
                WorkerBee* newWorker = mLastHive->addWorker();

                // uncomment me when learnFlowerLocation is coded
                if (newWorker != nullptr) newWorker->learnFlowerLocation(mFlowerPosition);
            } else {
                std::cout << "Couldn't create new hive, add flowers first\n";
            }

            break;
        }
        }
    }
}

void WorkerTest::onDraw(sf::RenderTarget& target)
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
