#include "TextureAsset.h"

namespace alag
{

TextureAsset::TextureAsset()
{
    //ctor
}

TextureAsset::~TextureAsset()
{
    //dtor
}

bool TextureAsset::LoadFromFile(const std::string &filePath)
{
    return m_texture.loadFromFile(filePath);
}

}
