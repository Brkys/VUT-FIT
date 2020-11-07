/**
 * Uchovava stav programu a kontroluje zmeny stavu.
 * @author Simeon Borko
 */

#include "ModelState.h"

namespace Model {

    ModelState::ModelState()
    {
        current = SCHEME;

        transitions[SCHEME] = {INITIALIZING};
        transitions[INITIALIZING] = {SCHEME, INITIALIZED};
        transitions[INITIALIZED] = {RUN, DEBUG};
        transitions[RUN] = {FINISH};
        transitions[DEBUG] = {SCHEME, STEP, RUN};
        transitions[STEP] = {DEBUG, FINISH};
        transitions[FINISH] = {SCHEME};
    }

    ModelState::~ModelState() {}

    void ModelState::setState(StateEnum state, bool force)
    {
        if (transitions[current].count(state) || force) {
            current = state;
            emit stateChanged();
        } else {
            throw std::exception();
        }
    }

    ModelState::StateEnum ModelState::getState()
    {
        return current;
    }

    bool ModelState::operator==(StateEnum tested)
    {
        return current == tested;
    }

    bool ModelState::operator!=(StateEnum tested)
    {
        return current != tested;
    }

}
