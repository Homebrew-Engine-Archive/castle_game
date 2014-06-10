#ifndef SCENE_P_H_
#define SCENE_P_H_

namespace Render
{
    std::vector<core::Vec2i> mTranslationStack;
    core::Vec2i mTranslation;
    std::vector<core::Vec2f> mScaleStack;
    core::Vec2f mScale;
    std::vector<float> mAlphaStack;
    int mAlpha;
    std::vector<core::Vec2i> mCropStack;
    core::Vec2i mCrop;
    std::vector<SceneItem> mSceneItems;
}

#endif // SCENE_P_H_
