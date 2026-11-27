/* Composition root: the only module that knows every component and the order they start in. */
#pragma once

/* Boot checks, watchdog, services, board, controller link and UI, then the alive loop.
 * Returns only if the board's memory configuration is wrong (nothing else is started then). */
void app_run(void);
