#include "CFSM.hpp"

using namespace std;

CFSM::CFSM(vector<State> etats)
    : states_(etats), indiceState_(0)
{}

CFSM::~CFSM()
{}

State CFSM::getState() const
{
    return states_[indiceState_];
}

vector<State> CFSM::getStates() const
{
    return states_;
}

size_t CFSM::getIndiceState() const
{
    return indiceState_;
}

void CFSM::nextState()
{
    ++indiceState_;

    if (indiceState_ == states_.size()) {
        // Permet de naviguer circulairement dans le tableau d'états
        indiceState_ = 0;
    }
    onEnterState(states_[indiceState_]);
}

void CFSM::setIndiceState(size_t indice)
{
    indiceState_ = indice;
}

void CFSM::action(sf::Time dt)
{
    onState(states_[indiceState_], dt);
}

