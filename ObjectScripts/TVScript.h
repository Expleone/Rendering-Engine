//
// Created by expleoene on 5/6/26.
//

#ifndef VIEWER_TVSCRIPT_H
#define VIEWER_TVSCRIPT_H
#include "FilmScript.h"
#include "ObjectScript.h"

namespace BiBuild {
    class TVScript : public ObjectScript {
        FilmScript* filmScript;
        bool isPaused = true;
        SceneObject* playButton;
        SceneObject* nextButton;
        SceneObject* previousButton;
        SceneObject* light;
        std::vector<std::string> content;
        int currentContent = 0;
    public:
        TVScript(SceneObject* owner) : ObjectScript(owner) {
            // owner->hasClickableParts = true;
        }
        void SetFilmScript(FilmScript* script) {
            filmScript = script;
            if (filmScript) {
                filmScript->Pause();
                if (!content.empty()) {
                    filmScript->SetVideo(content[0].c_str());
                    currentContent = 0;
                }
            }
        }
        void SetButtons(SceneObject* pButton,
        SceneObject* nButton,
        SceneObject* prevButton) {
            if (!pButton || !nButton || !prevButton) return;
            playButton = pButton;
            nextButton = nButton;
            previousButton = prevButton;
        }
            void SetLight(SceneObject* light) {
                this->light = light;
            }

        void SetContent(const std::vector<std::string>& cont) {
            content = cont;
            currentContent = -1;
        }
        void Update() override;


    };
} // BiBuild

#endif //VIEWER_TVSCRIPT_H
