#ifndef MASTER_RENDERER_H
#define MASTER_RENDERER_H

#include "Chunk_Renderer.h"
#include "Water_Renderer.h"
#include "Flora_Renderer.h"
#include "SFML_Renderer.h"

#include "Framebuffer_Object.h"

class Chunk;
class Camera;

class Master_Renderer
{
    public:
        void prepare ();

        void processChunk       (const Chunk& chunk);
        void processSfDrawable  (const sf::Drawable& sfDrawable);

        void render (const Camera& camera);

    private:
        Chunk_Renderer m_chunkRenderer;
        Water_Renderer m_waterRenderer;
        Flora_Renderer m_floraRenderer;
        SFML_Renderer  m_sfmlRenderer;

        Framebuffer_Object m_framebuffer;

        std::vector<const sf::Drawable*> m_sfDrawcalls;
};

#endif // MASTER_RENDERER_H
