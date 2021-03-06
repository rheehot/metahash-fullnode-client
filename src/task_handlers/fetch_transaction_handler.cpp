#include "fetch_transaction_handler.h"
#include "settings/settings.h"
#include "cache/history_cache.h"
#include "common/string_utils.h"
#include "common/convertStrings.h"
#include "utils.h"

fetch_transaction_handler::fetch_transaction_handler(session_context_ptr ctx)
    : base_handler(ctx)
{
    m_duration.set_message(__func__);
    m_name = __func__;
}

bool fetch_transaction_handler::prepare_params()
{
    BGN_TRY
    {
        CHK_REQ(m_id, "id field not found")

        auto params = m_reader.get_params();
        CHK_REQ(params, "params field not found")

        CHK_PRM(settings::system::history_cache_enable, "History cache is disabled");

        CHK_PRM(m_reader.get_value(*params, "address", m_addr), "address field not found")
        CHK_PRM(!m_addr.empty(), "address is empty")
        CHK_PRM(utils::validate_address(m_addr), "address is invalid")

        CHK_PRM(m_reader.get_value(*params, "data", m_data), "data field not found")
        CHK_PRM(!m_data.empty(), "data is empty")

        return true;
    }
    END_TRY(return false)
}

void fetch_transaction_handler::execute()
{
    BGN_TRY
    {
        rapidjson::Document doc;
        m_writer.reset();
        if (!history_cache::get()->get_history(m_addr, doc)) {
            m_writer.set_error(-32301, doc.GetString());
            return;
        }

        m_data = common::toHex(m_data.begin(), m_data.end());

        rapidjson::Value arr(rapidjson::kArrayType);
        rapidjson::Value::ConstMemberIterator data;
        for (auto& v : doc.GetArray()) {
            if (!v.IsObject()) {
                continue;
            }
            data = v.FindMember("data");
            if (data == v.MemberEnd()) {
                // TODO Warning
                continue;
            }
            if (data->value.GetStringLength() != m_data.size()) {
                continue;
            }
            if (strncmp(m_data.c_str(), data->value.GetString(), m_data.size()) != 0) {
                continue;
            }
            arr.PushBack(v, m_writer.get_allocator());
        }
        m_writer.set_result(arr);
    }
    END_TRY();
}
