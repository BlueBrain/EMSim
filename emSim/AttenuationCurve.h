
/* Copyright (c) 2014-2016, EPFL/Blue Brain Project
 *                          Ahmet.Bilgili@epfl.ch
 *                          Stefan.Eilemann@epfl.ch
 *                          grigori.chevtchenko@epfl.ch
 *
 * This file is part of Fivox <https://github.com/BlueBrain/Fivox>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _AttenuationCurve_h_
#define _AttenuationCurve_h_

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

namespace ems
{
/** Attentuation curve access for depth-based interpolation for VSD */
class AttenuationCurve
{
public:
    /** Construct an empty attenuation curve */
    AttenuationCurve();

    /**
     * @param dyeCurveFile Dye attenuation file.
     * @param thickness Thickness of the circuit.
     */
    AttenuationCurve(const std::string& dyeCurveFile,
                     const float thickness);

    /**
     * Get the attenuation for the given Y-coordinate.
     *
     * If no curve file was loaded or the thickness has a negative value,
     * returns 1.0f for any attenuation depth so it does not modify the value of
     * event. If yCoord is larger than the thickness it returns the first
     * element of the curve (generally 0.f)
     *
     * @param yCoord Absolute position (Y axis) of the point.
     * @param interpolate If true, interpolate the attenuation between the two
     * closest indices in the attenuation curve.
     * @return the interpolated attenuation value according to depth; 1 if there
     * is no attenuation curve or it is empty.
     */
    float getAttenuation(const float yCoord, const bool interpolate = false) const;

private:
    std::vector<float> _dyeCurve;
    float _thickness;
};
}
#endif // _AttenuationCurve_h_

