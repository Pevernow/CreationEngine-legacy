#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <bgfx/bgfx.h>
#include <map>
#include <string>

using namespace std;

class Block
{
public:
    int x;
    int y;
    int z;
    string type = "air";
};

struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
    int16_t u;
    int16_t v;
};

bool Gen_block_model();

void destroy();

bool is_type_registed(string type);

void Draw_blocks();

void register_node(const char* name, const char* texture_path);

#endif