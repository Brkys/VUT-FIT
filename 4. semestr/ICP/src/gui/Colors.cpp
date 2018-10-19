/**
 * @author Simeon Borko
 * Nastavuje barvy v projektu
 */

#include "Colors.h"

namespace Gui {

    const QColor Colors::Block = Qt::white;
    const QColor Colors::BlockPressed = QColor("#f5f5dc");
    const QColor Colors::BlockComputed = QColor("#87cefa");
    const QColor Colors::BlockRun = QColor("#4169e1");
    const QColor Colors::Port = Colors::Block;
    const QColor Colors::PortHover = QColor("#87cefa");
    const QColor Colors::PortActive = QColor("#4169e1");
    const QColor Colors::PortConnected = QColor("#f5f5dc");

}
