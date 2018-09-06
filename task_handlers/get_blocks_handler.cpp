#include "task_handlers.h"

// get_blocks_handler
bool get_blocks_handler::prepare_params()
{
    CHK_PRM(m_id, "id field not found")

    auto params = m_reader.get_params();
    CHK_PRM(params, "params field not found")

    mh_count_t countBlocks(0);
    if (m_reader.get_value(*params, "countBlocks", countBlocks))
        m_writer.add_param("countBlocks", countBlocks);

    mh_count_t beginBlock(0);
    if (m_reader.get_value(*params, "beginBlock", beginBlock))
        m_writer.add_param("beginBlock", beginBlock);

    return true;
}