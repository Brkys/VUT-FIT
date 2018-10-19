/**
 * @author Simeon Borko
 * @brief Nastavuje barvy v projektu
 */

#ifndef COLORS_H
#define COLORS_H

#include <QColor>

namespace Gui {

    /**
     * @brief Určuje barvy ve schématu
     */
    struct Colors {

        static const QColor Block;
        static const QColor BlockPressed;
        static const QColor BlockComputed;
        static const QColor BlockRun;
        static const QColor Port;
        static const QColor PortHover;
        static const QColor PortActive;
        static const QColor PortConnected;

    };

}

#endif // COLORS_H
