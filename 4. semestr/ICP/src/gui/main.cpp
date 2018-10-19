/**
 * @author Zdeněk Brhel, Simeon Borko
 * Spouští program
 */


#include "mainwindow.h"
#include <QApplication>
#include "../Model/ModelState.h"

using namespace Configuration;
using namespace Model;
using namespace Gui;

void loadTypes(ConfigTypeMap &tm)
{
    tm.create("color", {"red", "green", "blue"});
    tm.create("logical", {"bit"});
    tm.create("real", {"number"});
}

void loadBlocks(ConfigBlockMap &bm)
{
    ConfigBlock &b1 = bm.create("rgbToGray");
    b1.addInPort("rgb", "color");
    b1.addOutPort("gray", "real");

    ConfigBlock &b2 = bm.create("add");
    b2.addInPort("a", "real");
    b2.addInPort("b", "real");
    b2.addOutPort("c", "real");

    ConfigBlock &b3 = bm.create("flip");
    b3.addInPort("a", "real");
    b3.addOutPort("b", "real");

    ConfigBlock &b4 = bm.create("multiply");
    b4.addInPort("a", "real");
    b4.addInPort("b", "real");
    b4.addOutPort("c", "real");

    ConfigBlock &b5 = bm.create("median5");
    b5.addInPort("a", "real");
    b5.addInPort("b", "real");
    b5.addInPort("c", "real");
    b5.addInPort("d", "real");
    b5.addInPort("e", "real");
    b5.addOutPort("median", "real");

    ConfigBlock &b6 = bm.create("and");
    b6.addInPort("a", "logical");
    b6.addInPort("b", "logical");
    b6.addOutPort("c", "logical");

    ConfigBlock &b7 = bm.create("or");
    b7.addInPort("a", "logical");
    b7.addInPort("b", "logical");
    b7.addOutPort("c", "logical");

    ConfigBlock &b8 = bm.create("xor");
    b8.addInPort("a", "logical");
    b8.addInPort("b", "logical");
    b8.addOutPort("c", "logical");

    ConfigBlock &b9 = bm.create("not");
    b9.addInPort("a", "logical");
    b9.addOutPort("b", "logical");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ModelState mState;

    ConfigTypeMap tm;
    loadTypes(tm);
    ConfigBlockMap bm (tm);
    loadBlocks(bm);
    ModelBlockArray ba (bm, mState);

    QThread mThread;
    ba.moveToThread(&mThread);
    mThread.start();

    MainWindow w (mState, ba);

    w.show();
    int code = app.exec();

    mThread.quit();
    mThread.wait();

    return code;
}
