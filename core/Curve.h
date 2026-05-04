//
// Created by expleoene on 5/2/26.
//

#ifndef VIEWER_CURVE_H
#define VIEWER_CURVE_H
#include <utility>
#include <vector>
#define CURVE_DEGREE 3

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace BiBuild {
    class Curve {
    public:
        std::vector<glm::vec3> controlPoints;
        std::vector<double> weights;
        std::vector<float> knots;


        Curve(std::vector<glm::vec3>& control_points) : controlPoints(std::move(control_points)) {
            int knotsNum = controlPoints.size() + CURVE_DEGREE + 1;
            weights.assign(controlPoints.size(), 1.0);
            knots.assign(knotsNum, 0.0);
            for (int i = 0; i < CURVE_DEGREE + 1; i++) {
                knots[knotsNum - i - 1] = 1;
            }
            int internalKnotCount = knotsNum - 2 * (CURVE_DEGREE + 1);
            if (internalKnotCount > 0) {
                float step = 1.0f / (internalKnotCount + 1);
                for (int i = 1; i <= internalKnotCount; i++) {
                    knots[CURVE_DEGREE + i] = i * step;
                }
            }
        }

        size_t findStartKnot(float t) {
            for (int i = 3;  i < knots.size() - 1; i++) {
                if (t >= knots[i] && t < knots[i+1]) return i;
            }
            return knots.size() - CURVE_DEGREE - 2;
        }

        std::vector<double> weightFunc(size_t i, double t) {
            double level1[4];
            double level2[3];
            double level3[2];
            double level4;

            //Ni,1
            level1[0] = 1;

            //Ni-1, 2
            double dividerRight = knots[i+1] - knots[i];
            level1[1] = dividerRight == 0 ? 0 : (knots[i+1] - t)/dividerRight * level1[0];

            //Ni-2,3
            dividerRight = knots[i+1] - knots[i-1];
            level1[2] = dividerRight == 0 ? 0 : (knots[i+1] - t)/dividerRight * level1[1];

            //Ni-3,4
            dividerRight = knots[i+1] - knots[i-2];
            level1[3] = dividerRight == 0 ? 0 : (knots[i+1] - t)/dividerRight * level1[2];

            //Ni,2
            double dividerLeft = knots[i+1] - knots[i];
            level2[0] = dividerLeft == 0 ? 0 : (t - knots[i])/dividerLeft * level1[0];

            //Ni-1,3
            dividerLeft = knots[i+1] - knots[i-1];
            level2[1] = dividerLeft == 0 ? 0 : (t - knots[i-1])/dividerLeft * level1[1];
            dividerRight = knots[i+2] - knots[i];
            level2[1] = dividerRight == 0 ? level2[1] : level2[1] + (knots[i+2] - t)/dividerRight * level2[0];

            //Ni-2,4
            dividerLeft = knots[i+1] - knots[i-2];
            level2[2] = dividerLeft == 0 ? 0 : (t - knots[i-2])/dividerLeft * level1[2];
            dividerRight = knots[i+2] - knots[i-1];
            level2[2] = dividerRight == 0 ? level2[2] : level2[2] + (knots[i+2] - t)/dividerRight * level2[1];

            //Ni,3
            dividerLeft = knots[i+2] - knots[i];
            level3[0] = dividerLeft == 0 ? 0 : (t - knots[i])/dividerLeft * level2[0];

            //Ni-1,4
            dividerLeft = knots[i+2] - knots[i-1];
            level3[1] = dividerLeft == 0 ? 0 : (t - knots[i-1])/dividerLeft * level2[1];
            dividerRight = knots[i+3] - knots[i];
            level3[1] = dividerRight == 0 ? level3[1] : level3[1] + (knots[i+3] - t)/dividerRight * level3[0];

            //Ni,4
            dividerLeft = knots[i+3] - knots[i];
            level4 = dividerLeft == 0 ? 0 : (t - knots[i])/dividerLeft * level3[0];

            return std::vector<double>{level1[3], level2[2], level3[1], level4};
        }

        glm::vec3 getPoint(float t) {
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            int first_knot = findStartKnot(t);
            auto N = weightFunc(first_knot, t);

            glm::vec3 C(0);
            double denominator = 0.0;

            for (int j = 0; j <= CURVE_DEGREE; ++j) {
                int index = first_knot - CURVE_DEGREE + j;
                double weight = N[j] * weights[index];
                C.x += controlPoints[index].x * weight;
                C.y += controlPoints[index].y * weight;
                C.z += controlPoints[index].z * weight;
                denominator += weight;
            }

            C.x /= denominator;
            C.y /= denominator;
            C.z /= denominator;

            return C;
        }
    };
} // BiBuild

#endif //VIEWER_CURVE_H
