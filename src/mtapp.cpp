#include "mtapp.h"

using namespace std;

namespace mt {

MegaTextureApp::MegaTextureApp(const wchar_t * Title, size_t W, size_t H) :
    App(Title, W, H),
    _pool(128, 49152),
    _pt(256, 8),
    _lru(1024),
    _overlay(W, H)
{
    _texture    = std::make_unique<MegaTexture>("G:\\MegaTextures\\Factory.megatexture");
    _scene      = std::make_unique<Scene>("G:\\MegaTextures\\Factory.scene");
    _feedback   = std::make_unique<FeedbackTask>(_scene->buffer(), W/8, H/8, _mvp);
    _analysis   = std::make_unique<AnalysisTask>(_feedback->framebuffer());
    _streamer   = std::make_unique<TextureStreamer>(*_texture,
        "G:\\MegaTextures\\Factory.megatexture", 4);
    _request    = std::make_unique<RequestTask>(*_streamer, _analysis->result(), _pool);
    _texture_cache  = std::make_unique<ogl::BufferedTexture>(4096, 4096, 1);
    _page_table     = std::make_unique<ogl::BufferedTexture>(
        _texture->VirtualSize() / _texture->PageSize(),
        _texture->VirtualSize() / _texture->PageSize(),
        1
    );

    _update = std::make_unique<CacheUpdateTask>(_pt, _lru, *_streamer, *_texture_cache, *_page_table, _pool);


    _render     = std::make_unique<RenderTask>(W, H, _scene->buffer(), _mvp, *_texture_cache, *_page_table);

    glm::mat4 projMat = glm::perspective(45.0f, float(W) / float(H), 1.0f, 1000.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5, 10), glm::vec3(0,0,0), glm::vec3(0.0f,1.0f, 0.0f));
    _mvp = projMat * view;
}

MegaTextureApp::~MegaTextureApp()
{
}

void MegaTextureApp::Frame()
{
    if (_feedback) {
        _feedback->Compute();
        _analysis->Compute();
        _request->Compute();
        _update->Compute();
        _render->Compute();

        //_overlay.ShowTexture(0, 0, 512, 512, _feedback->texture());
        _overlay.ShowTexture(0, 0, 512, 512, *_texture_cache);
    }
}

} // namespace mt
