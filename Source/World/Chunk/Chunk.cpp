#include "Chunk.h"

#include <iostream>
#include <fstream>

#include "../Block/D_Blocks.h"
#include "../Block/Block.h"

#include "../../Loader.h"
#include "../../Input/Function_Toggle_Key.h"
#include "../../Util/Noise_Generator.h"

#include "Chunk_Map.h"

Chunk::Chunk(const Chunk_Location& position, Chunk_Map& chunkMap, const Texture_Atlas& blockAtlas)
//:   m_layers        (WATER_LEVEL + 1)
:   m_location      (position)
,   m_position      (position.x * SIZE, position.z * SIZE)
,   m_p_chunkMap    (&chunkMap)
,   m_p_atlas       (&blockAtlas)
,   m_mesh          (*this)
,   m_blocks        (*this, m_location, *m_p_chunkMap)
,   m_worldGenerator(*this)
{
    m_worldGenerator.generate();
    loadBlockData ();

    m_hasBlockData = true;
}

bool Chunk::hasBlockData () const
{
    return m_hasBlockData;
}

bool Chunk::hasMesh () const
{
    return m_hasMesh;
}

bool Chunk::hasBuffered () const
{
    return m_hasBuffered;
}

void Chunk::generateMesh ()
{
    m_p_chunkMap->addChunk({m_location.x + 1, m_location.z});
    m_p_chunkMap->addChunk({m_location.x, m_location.z + 1});
    m_p_chunkMap->addChunk({m_location.x - 1, m_location.z});
    m_p_chunkMap->addChunk({m_location.x, m_location.z - 1});

    m_mesh.generateMesh(m_blocks.getLayerCount());

    m_hasMesh       = true;
    m_hasBuffered   = false;
}

void Chunk::bufferMesh ()
{
    m_mesh.bufferMesh();
    m_hasBuffered = true;
}

const Texture_Atlas& Chunk::getAtlas() const
{
    return *m_p_atlas;
}


const Chunk_Location& Chunk::getLocation () const
{
    return m_location;
}

const Vector2& Chunk::getPosition () const
{
    return m_position;
}

const Chunk_Mesh& Chunk::getMesh() const
{
    return m_mesh;
}


void Chunk::giveDeleteFlag ()
{
    saveToFile();
    m_hasDeleteFlag = true;
}

bool Chunk::hasDeleteFlag () const
{
    return m_hasDeleteFlag;
}

void Chunk::update()
{
    generateMesh();
    bufferMesh();
    m_hasUpdateFlag = false;
}

void Chunk::giveUpdateFlag()
{
    m_hasUpdateFlag = true;
}

bool Chunk::hasUpdateFlag() const
{
    return m_hasUpdateFlag;
}

void Chunk::saveToFile()
{
    if(!m_blocks.getAddedBlocks().empty())
    {
        std::ofstream outFile (getFileString());

        for(auto& block : m_blocks.getAddedBlocks())
        {
            const Block_Location& l = block.first;

            outFile << l.x << " " << l.y << " " << l.z << " ";
            outFile << block.second << std::endl;
        }
    }

}


std::string Chunk::getFileString()
{
    return "Worlds/" +
            std::to_string(Noise::getSeed()) +
            "/" +
            std::to_string(m_location.x)
            + " "
            + std::to_string(m_location.z);
}

Chunk_Blocks& Chunk::getBlocks()
{
    return m_blocks;
}

const Chunk_Blocks& Chunk::getBlocks() const
{
    return m_blocks;
}

void Chunk::loadBlockData ()
{
    std::ifstream inFile(getFileString());

    if(!inFile.is_open())
        return;

    char x, z;
    int y, id;

    while(inFile.peek() != EOF)
    {
        inFile >> x >> y >> z >> id;
        m_blocks.m_addedBlocks[{x, y, z}] = id;
    }

    for (auto& block : m_blocks.m_addedBlocks)
    {
        int idNum = block.second;
        Block::ID id = static_cast<Block::ID>(idNum);

        m_blocks.qSetBlock(block.first, Block::getBlockFromId(id));
    }
}

