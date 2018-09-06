#include "task_handlers.h"

// get_tx_handler
bool get_tx_handler::prepare_params()
{
    CHK_PRM(m_id, "id field not found")

    auto params = m_reader.get_params();
    CHK_PRM(params, "params field not found")

    std::string hash;
    CHK_PRM(m_reader.get_value(*params, "hash", hash) && !hash.empty(), "hash field not found")

    m_writer.add_param("hash", hash.c_str());

    return true;
}