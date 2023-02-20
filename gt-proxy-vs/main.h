#pragma once

#define MODIFY_LOGIN_PACKET_HELPER(name, func) \
    {\
        int pos_##name = bruh.find(#name "|");\
        int newln_pos = bruh.find("\n", pos_##name);\
        std::string gen_num = func;\
\
        if (pos_##name == std::string::npos || pos_##name == std::string::npos)\
            goto skip;\
\
        bruh.replace(pos_##name + strlen(#name) + 1, gen_num.length(), gen_num);\
    }
