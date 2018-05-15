#ifndef CFSM_HPP
#define CFSM_HPP
#include "../Utility/Utility.hpp"
#include "../Utility/Vec2d.hpp"

typedef Uid State;

class CFSM
{
public:
    CFSM(std::vector<State> etats);
    ~CFSM();
    State getState() const;
    std::vector<State> getStates() const;
    size_t getIndiceState() const;
    // Changement d'état
    void nextState();
    void setIndiceState(size_t indice);
    virtual void onState(State etat, sf::Time dt) = 0;
    void action(sf::Time dt);
    virtual void onEnterState(State etat) = 0;

private:
    std::vector<State> states_;
    size_t indiceState_;

};

#endif
