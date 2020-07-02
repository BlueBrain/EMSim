
/* Copyright (c) 2014-2020, EPFL/Blue Brain Project
 *                          Ahmet.Bilgili@epfl.ch
 *                          Stefan.Eilemann@epfl.ch
 *                          grigori.chevtchenko@epfl.ch
 *
 * This file is part of Emsim <https://github.com/BlueBrain/EMSim>
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

#include <emSim/AttenuationCurve.h>

namespace ems
{

AttenuationCurve::AttenuationCurve()
    : _thickness(1.0f)
{}

AttenuationCurve::AttenuationCurve(const std::string& dyeCurveFile, const float thickness)
    : _thickness(thickness)
{
    if (dyeCurveFile.empty())
        return;

    std::ifstream ifs(dyeCurveFile);
    if (!ifs.is_open())
        return;

    std::string line;
    while (std::getline(ifs, line))
        _dyeCurve.push_back(atof(line.c_str()));

    if (_dyeCurve.empty())
        return;

    const float maxAttn = *std::max_element(_dyeCurve.begin(), _dyeCurve.end());
    for (float& dyeAttn : _dyeCurve)
        dyeAttn = dyeAttn / maxAttn;
}

float AttenuationCurve::getAttenuation(const float yCoord,
                                       const bool interpolate) const
{
    if (_dyeCurve.empty() || _thickness <= 0.f)
        return 1.0f;

    if (yCoord >= _thickness)
        return _dyeCurve[0];

    const float depth = _thickness - yCoord;
    if (_dyeCurve.size() == 1 || depth >= _thickness)
        return _dyeCurve[_dyeCurve.size() - 1];

    const float depthDelta = _thickness / (_dyeCurve.size() - 1);
    const size_t index = depth / depthDelta;

    const float attenuation = _dyeCurve[index + 1];
    if (!interpolate)
        return attenuation;

    // parameter used for performing linear interpolation
    const float t = (depth - depthDelta * index) / depthDelta;
    return t * attenuation + (1.f - t) * _dyeCurve[index];
}

}
