/**
 * @brief Reprezentuje vsetky vytvorene bloky.
 * @author Simeon Borko
 */

#ifndef SRC_RUNNING_BLOCKARRAY_H
#define SRC_RUNNING_BLOCKARRAY_H

#include <iostream>
#include <vector>
#include <QObject>
#include <QThread>

#include "../Configuration/ConfigBlockMap.h"
#include "../Configuration/ConfigBlock.h"
#include "ModelBlock.h"
#include "ModelState.h"

using namespace std;

namespace Model {

    class ModelBlockArray : public QObject {

        Q_OBJECT

        /**
         * `blockArray` obsahuje pary <id, ukazatel na blok>.
         */
        map<int, ModelBlock *> blockArray;
        Configuration::ConfigBlockMap &blockMap;
        int idCounter;
        ModelState &mState;
        bool cyclesAlreadyChecked;

        /**
         * Ziska iterator mapy na blok zadany id. Ak taky blok neexistuje, vyhodi vynimku.
         * @param id id bloku
         * @return iterator najdeneho bloku
         */
        map<int, ModelBlock *>::iterator getIter(int id);

    private slots:
        /**
         * Ak je stav SCHEME, obnovi hodoty portov vsetkych blokov
         * a nastavi cyclesAlreadyChecked na false.
         */
        void reinit();

    public slots:
        /**
         * Pokusi sa spustit dalsi blok.
         * Ak nie je dalsi blok, zmeni stav na FINISH.
         */
        void runNextBlock();

    signals:
        /**
         * Informuje, ze sa pokusil spustit dalsi blok.
         * @param canRunNext je mozne spustit dalsi blok?
         */
        void runResult(bool canRunNext);

    public:

        explicit ModelBlockArray(Configuration::ConfigBlockMap &blockMap, ModelState &mState);

        ~ModelBlockArray();

        /**
         * Ziska pocet blokov.
         * @return pocet blokov
         */
        int size();

        /**
         * Vytvori blok podla predlohy.
         * Ak stav nie je SCHEME, vyhodi vynimku.
         * @param cBlock blok konfiguracie
         * @return vytvoreny blok
         */
        ModelBlock& create(Configuration::ConfigBlock &cBlock);

        /**
         * Vytvori blok podla nazvu.
         * Ak stav nie je SCHEME, vyhodi vynimku.
         * @param blockName nazov bloku
         * @return vytvoreny blok
         */
        ModelBlock& create(string blockName);

        /**
         * Ziska blok podla id. Ak taky blok nie je vytvoreny, vyhodi vynimku.
         * @param id id vytvoreneho bloku
         * @return vytvoreny blok
         */
        ModelBlock& get(int id);

        /**
         * Odstrani blok podla id. Ak taky blok nie je vytvoreny, vyhodi vynimku.
         * Ak stav nie je SCHEME, vyhodi vynimku.
         * @param id id vytvoreneho bloku
         */
        void remove(int id);

        /**
         * Metoda detekuje cykly.
         * @return ma zapojenie blokov cykly?
         */
        bool hasCycles();

        /**
         * Ziska bloky, ktore maju aspon jeden vstupny port,
         * ktory ma aspon jednu premennu, ktora nie je vyplnena.
         * Vyzaduje stav INITIALIZE.
         * Vyzaduje, aby bola najprv spustena funkcia hasCycles.
         * @return vektor blokov
         */
        vector<ModelBlock *> emptyInputBlocks();

        /**
         * Vrati nejaky blok, ktory je mozne spustit alebo nullptr.
         * Vyzaduje, aby bola najprv spustena funkcia hasCycles.
         * @return ukazatel na spustitelny blok alebo nullptr v pripade, ze ziadny blok nejde spustit
         */
        ModelBlock* runnableBlock();

        /**
         * Tuto funkciu zavola destruktor ModelBlock.
         * @param block ukazatel na mazany blok
         */
        void removeBlockCallback(ModelBlock *block);

        vector<string> blockNames();

    };

}


#endif //SRC_RUNNING_BLOCKARRAY_H
