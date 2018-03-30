#include "app.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include "megatexture.h"
#include "scene.h"
#include "feedbacktask.h"
#include "analysistask.h"
#include "texturestreamer.h"
#include "requesttask.h"
#include "memorypool.h"
#include "oglbufferedtexture.h"
#include "rendertask.h"
#include "pagetable.h"
#include "lrucache.h"
#include "cacheupdatetask.h"
#include "ogloverlay.h"
#include <memory>

namespace mt {

///
/// \class  MegaTextureApp
/// \brief  MegaTextures demo application
///
class MegaTextureApp : public App
{
public:
    MegaTextureApp(const wchar_t * Title, size_t W, size_t H);
    virtual ~MegaTextureApp();

    virtual void Frame();

protected:
    MemoryPool                          _pool;
    glm::mat4                           _mvp;   //< Model View Projection
    std::unique_ptr<MegaTexture>        _texture;
    std::unique_ptr<Scene>              _scene;
    std::unique_ptr<FeedbackTask>       _feedback;
    std::unique_ptr<AnalysisTask>       _analysis;
    std::unique_ptr<RequestTask>        _request;
    std::unique_ptr<RenderTask>         _render;
    std::unique_ptr<CacheUpdateTask>    _update;
    std::unique_ptr<TextureStreamer>    _streamer;

    std::unique_ptr<ogl::BufferedTexture> _texture_cache;
    std::unique_ptr<ogl::BufferedTexture> _page_table;

    PageTable                           _pt;
    LRUCache<uint32_t>                  _lru;
    ogl::Overlay                        _overlay;

};

} // namespace mt
