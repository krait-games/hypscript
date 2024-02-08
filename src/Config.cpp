#include <Config.hpp>

#include <asset/ByteWriter.hpp>

namespace hyperion::v2 {

const FlatMap<OptionName, String> Configuration::option_name_strings = {
};

OptionName Configuration::StringToOptionName(const String &str)
{
    for (auto &it : option_name_strings) {
        if (it.second == str) {
            return it.first;
        }
    }

    return CONFIG_NONE;
}

String Configuration::OptionNameToString(OptionName option)
{
    const auto it = option_name_strings.Find(option);

    if (it == option_name_strings.End()) {
        return "Unknown";
    }

    return it->second;
}

Configuration::Configuration() = default;

bool Configuration::LoadFromDefinitionsFile()
{
    const DefinitionsFile definitions("config.def");

    if (!definitions.IsValid()) {
        return false;
    }

    for (const auto &it : definitions.GetSections()) {
        for (const auto &option_it : it.second) {
            const OptionName option_name = StringToOptionName(option_it.first);
            const DefinitionsFile::Value &option_value = option_it.second;

            if (option_name == CONFIG_NONE || option_name >= CONFIG_MAX) {
                DebugLog(
                    LogType::Warn,
                    "%s: Unknown config option\n",
                    option_it.first.Data()
                );

                continue;
            }

            Option value;

            union { Int i; Float f; } tmp_value;

            if (option_value.GetValue().name == "true") {
                value = true;
            } else if (option_value.GetValue().name == "false") {
                value = false;
            } else if (StringUtil::Parse(option_it.first.Data(), &tmp_value.i)) {
                value = tmp_value.i;
            } else if (StringUtil::Parse(option_it.first.Data(), &tmp_value.f)) {
                value = tmp_value.f;
            } else {
                value = false;
            }

            m_variables[option_name] = value;
        }
    }

    return true;
}

bool Configuration::SaveToDefinitionsFile()
{
    String str_result = "[Default]\n";

    for (UInt index = CONFIG_NONE + 1; index < CONFIG_MAX; index++) {
        const Option &option = m_variables[index];

        if (!option.GetIsSaved()) {
            continue;
        }

        String value_string = "false";

        if (option.IsValid()) {
            if (option.Is<bool>()) {
                value_string = option.GetBool() ? "true" : "false";
            } else if (option.Is<Int>()) {
                value_string = String::ToString(option.GetInt());
            } else if (option.Is<Float>()) {
                // TODO!
                //value_string = String::ToString(option.GetFloat());
            }
        }

        str_result += OptionNameToString(OptionName(index)) + " = " + value_string + "\n";
    }

    const String path = "config.def";

    FileByteWriter writer(path.Data());

    if (!writer.IsOpen()) {
        return false;
    }

    writer.Write(str_result.Data(), str_result.Size());
    writer.Close();

    return true;
}

void Configuration::SetToDefaultConfiguration()
{
    m_variables = { };
}

} // namespace hyperion::v2