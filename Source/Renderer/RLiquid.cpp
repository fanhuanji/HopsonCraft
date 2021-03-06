#include "RLiquid.h"

#include <iostream>

#include "../World/Chunk/Chunklet.h"
#include "../Maths/Matrix_Maths.h"

namespace Renderer
{
    void RLiquid::draw(const Chunklet& chunklet)
    {
        m_chunks.push_back(&chunklet);
    }

    void RLiquid::update(const Matrix4& viewMatrix)
    {
        glEnable    (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable   (GL_CULL_FACE);

        m_shader.bind();

        m_shader.setViewMatrix(viewMatrix);

        m_shader.loadWaveTimer(m_wavesTimer.getElapsedTime().asSeconds());

        for (const Chunklet* chunklet : m_chunks)
        {
            prepare(*chunklet);
            glDrawElements(GL_TRIANGLES,
                           chunklet->getMesh().getLiquidMesh().getModel().getIndicesCount(),
                           GL_UNSIGNED_INT,
                           nullptr);
        }
        m_chunks.clear();
    }

    void RLiquid::prepare(const Chunklet& chunklet)
    {
        chunklet.getMesh().getLiquidMesh().getModel().bind();
        m_shader.setModelMatrix(chunklet.getMat());

    }
}
