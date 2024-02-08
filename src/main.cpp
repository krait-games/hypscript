#include <script/SourceFile.hpp>
#include <script/Script.hpp>
#include <asset/BufferedByteReader.hpp>
#include <util/ArgParse.hpp>
#include <util/fs/FsUtil.hpp>

using namespace hyperion;
using namespace hyperion::compiler;

int main(int argc, char *argv[])
{
    ArgParse arg_parse;
    arg_parse.Add("input", "i", ArgParse::ARG_FLAGS_REQUIRED, ArgParse::ARGUMENT_TYPE_STRING);

    auto parse_result = arg_parse.Parse(argc, argv);

    if (parse_result.ok) {
        AssertThrow(parse_result["input"].Is<String>());

        Reader reader;
        FilePath file_path(parse_result["input"].Get<String>());

        if (!file_path.Open(reader)) {
            printf("Failed to open file: %s\n", file_path.Data());

            return 1;
        }

        const ByteBuffer byte_buffer = reader.ReadBytes();

        SourceFile source_file(file_path.Data(), byte_buffer.Size());
        source_file.ReadIntoBuffer(byte_buffer);

        Script script(source_file);

        scriptapi2::Context context;

        if (script.Compile(context)) {
            script.Bake();
            script.Decompile(&utf::cout);
            script.Run(context); // Run the script once to initialize the global state

            return 0;
        } else {
            return 1;
        }
    }

    printf("Usage: %s -i <input file>\n", argv[0]);

    if (parse_result.message.HasValue()) {
        printf("Error: %s\n", parse_result.message.Get().Data());
    }

    return 1;
}