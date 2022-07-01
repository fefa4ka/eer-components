#include "Chip.h"
#include <eers.h>


/**
 * \brief
 */
WILL_MOUNT(Chip)
{
}

WILL_UPDATE_SKIP(Chip);

/**
 * \brief
 */
SHOULD_UPDATE(Chip)
{

    return false;
}

/**
 * \brief
 */
RELEASE(Chip)
{
}

DID_MOUNT_SKIP(Chip);

DID_UPDATE_SKIP(Chip);
