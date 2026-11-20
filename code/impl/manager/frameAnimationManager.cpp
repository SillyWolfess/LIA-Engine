#include "manager/frameAnimationManager.hpp"

#include "precompiled.hpp"

LIA::FrameAnimationData* LIA::FrameAnimationManager::get(std::string name) {
	LIA_TRY
		if (_data.find(name) == _data.end()) {
			return nullptr;
		}
		return &_data.at(name);
	LIA_CATCH(return nullptr;)
}

void LIA::FrameAnimationManager::pass(Scene* scene, Object* object, int nFrame, std::string name, Position& position, Rotation& rotation, Scale& scale) {
	FrameAnimationData* frames = get(name);
	if (frames == nullptr) {
		return;
	}
	if (frames->start > nFrame || frames->end < nFrame) {
		return;
	}
    int fIndx = nFrame - frames->start;
	Model* model = _modelManager.get(frames->frame.at(fIndx));
    if (model->isInGpu) {
        std::vector<int> offsets = model->data.offsets;
        for (int i = 0; i < offsets.size(); i++) {
            int oSize = model->size - offsets[i];
            if (i + 1 < offsets.size()) {
                oSize = offsets[i + 1] - offsets[i];
            }
            scene->add(
                object->_name,
                position, rotation, scale,
                model->vao, model->shader,
                model->hasIndices, oSize,
                object->_materialLib != "" ? object->_materialLib : model->materialLib,
                model->data.materialName.size() <= i ? "" : model->data.materialName[i],
                offsets[i]
            );
        }
    } else {
        LIA_error_f("Animation model {} was not loaded for frame {}", frames->name, nFrame);
    }
}