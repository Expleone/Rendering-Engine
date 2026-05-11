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

namespace holubiho {
    class Curve {
    public:
        std::vector<glm::vec3> controlPoints;
        std::vector<double> weights;
        std::vector<float> knots;


        /// @brief Constructs a NURBS curve with the given control points. Initializes the knot vector and weights for a cubic B-spline.
        /// The Created curve will be a clamped cubic B-spline, meaning the first and last control points will have full influence at the start and end of the curve, respectively. The knot vector is initialized with 0s for the first 4 knots, 1s for the last 4 knots, and evenly spaced values in between for any internal knots if there are more than 7 control points.
        Curve(std::vector<glm::vec3>& control_points);

        /// @brief Finds the index of the knot span that contains the parameter t.
        size_t findStartKnot(float t);

        /// @brief Computes the basis function values for the given knot span index i and parameter t. This function implements the Cox-de Boor recursion formula to calculate the basis functions for a cubic B-spline. It calculates the values for the first level (degree 1), then uses those to compute the second level (degree 2), and so on until it computes the final basis function values for degree 3. The resulting vector contains the basis function values corresponding to the control points that influence the curve at parameter t.
        /// @param i The index of the knot span that contains t.
        /// @param t The parameter value at which to evaluate the basis functions in the range [0, 1].
        std::vector<double> weightFunc(size_t i, double t);

        /// @brief Returns point on the curve corresponding to parameter t.
        /// @param t The parameter value in the range [0, 1] that specifies the position along the curve. 0 corresponds to the start of the curve, and 1 corresponds to the end.
        glm::vec3 getPoint(float t);

        /// @brief Creates a closed loop curve by appending the first few control points to the end of the control point list. This ensures that the curve will loop back to the starting point smoothly.
        /// @param control_points The control points that define the shape of the curve.
        static Curve createClosedLoop(const std::vector<glm::vec3>& control_points);
    };

} // BiBuild

#endif //VIEWER_CURVE_H
