//
// Created by expleoene on 5/6/26.
//

#include "TVScript.h"

namespace holubiho {
    void TVScript::Update() {
        if (isPaused && currentContent == -1 && filmScript && !content.empty()) {
            filmScript->SetVideo(content[0].c_str());
            currentContent = 0;
        }
        if (playButton && filmScript && playButton->hasBeenInteracted) {
            isPaused = !isPaused;
            if (isPaused) filmScript->Pause();
            else filmScript->Play();
            playButton->hasBeenInteracted = false;
        }

        if (nextButton && filmScript && nextButton->hasBeenInteracted) {
            if (currentContent + 1 < content.size()) {
                currentContent+=1;
                filmScript->SetVideo(content[currentContent].c_str());
                filmScript->Play();
                isPaused = false;
            }
            nextButton->hasBeenInteracted = false;
        }

        if (previousButton && filmScript && previousButton->hasBeenInteracted) {
            if (currentContent - 1 >= 0) {
                currentContent-=1;
                filmScript->SetVideo(content[currentContent].c_str());
                filmScript->Play();
                isPaused = false;
            }
            previousButton->hasBeenInteracted = false;
        }

        if (filmScript) {
            if (filmScript->PlayerStatus() == FilmScript::FilmScript_Ended) {
                isPaused = true;
                if (currentContent + 1 < content.size()) {
                    currentContent+=1;
                    filmScript->SetVideo(content[currentContent].c_str());
                    filmScript->Play();
                    isPaused = false;
                }
            }
        }

        if (light) {
        if (filmScript ) {
                if (auto light_component = light->GetComponent<LightComponent>()) {
                    light_component->diffuse = filmScript->GetAverageColor()*0.01f;
                    light_component->specular = filmScript->GetAverageColor()*0.01f;

                }
            }
            else {
                if (auto light_component = light->GetComponent<LightComponent>()) {
                    light_component->diffuse = glm::vec3(0.0f);
                    light_component->specular = glm::vec3(0.0f);
                }
            }
        }
    }
} // BiBuild