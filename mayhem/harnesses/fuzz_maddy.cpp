// Mirrors Utility::md2html, which renders package/description markdown to HTML.
#include "maddy/parser.h"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>

// The image parser may create its cache dir; keep that in the scratch dir, not the image or $HOME.
extern "C" int LLVMFuzzerInitialize(int *, char ***)
{
    const char *tmp = getenv("TMPDIR");
    setenv("XDG_CACHE_HOME", tmp ? tmp : "/tmp", 1);
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 1) {
        return -1;
    }

    std::shared_ptr<maddy::ParserConfig> config = std::make_shared<maddy::ParserConfig>();
    config->enabledParsers = maddy::types::DEFAULT;
    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>(config);

    std::stringstream markdownInput(std::string(reinterpret_cast<const char *>(data), size));
    parser->Parse(markdownInput);
    return 0;
}
