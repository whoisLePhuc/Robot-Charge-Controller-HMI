# HMI documentation

The current target is the Elecrow DLE06235B / ES3C35P ESP32-S3 display. The
firmware builds this target by default. Its five-page HMI, LCD and touch have
been integrated; the controller UART backend remains disabled until the board
pins are confirmed.

## Reading order

| Document | Content |
|---|---|
| [elecrow-s3-bringup.md](elecrow-s3-bringup.md) | Build configuration, LCD, touch, HMI integration and verification status |
| [controller-interface.md](controller-interface.md) | Controller protocol, command semantics and authority constraints |
| [ui-pages-and-information.md](ui-pages-and-information.md) | Five HMI pages, state labels and operator feedback |
| [controller-simulator.md](controller-simulator.md) | PC-side controller simulator; requires an enabled HMI transport |
| [controller-change-requests.md](controller-change-requests.md) | Interface dependencies and proposed controller changes |
| [mockups/](mockups/) | HMI visual reference |

## Current verification status

- The Elecrow ST77922 LCD and integrated capacitive touch work on the physical
  board connected through COM15 during bring-up.
- The shared five-page UI has been built and flashed to the Elecrow board.
  Visual and navigation acceptance of that image remains pending.
- The Elecrow firmware binds the link facade to an offline implementation.
  START/STOP controls cannot send controller commands in this build.
- No controller connection, charging session or relay operation has been
  verified by this HMI repository.
