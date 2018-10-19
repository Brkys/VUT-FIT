/**
 * @brief Uchovava stav programu a kontroluje zmeny stavu.
 * @author Simeon Borko
 */

#ifndef MODELSTATE_H
#define MODELSTATE_H

#include <QObject>
#include <map>
#include <set>

namespace Model {

    class ModelState : public QObject {

        Q_OBJECT

    public:

        enum StateEnum {
            SCHEME,
            INITIALIZING,
            INITIALIZED,
            RUN,
            DEBUG,
            STEP,
            FINISH
        };

        ModelState();
        ~ModelState();

        /**
         * Zmeni stav. Ak prechod nie je povoleny, vyhodi vynimku.
         * @param state novy stav
         * @param force zmenit stav napriek tomu, ze prechod nie je povoleny
         */
        void setState(StateEnum state, bool force = false);

        StateEnum getState();

        bool operator==(StateEnum tested);

        bool operator!=(StateEnum tested);

    signals:
        void stateChanged();

    private:

        StateEnum current;

        std::map<StateEnum, std::set<StateEnum>> transitions;

    };
}

#endif // MODELSTATE_H
