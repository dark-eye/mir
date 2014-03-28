/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Daniel van Vugt <daniel.van.vugt@canonical.com>
 */

#include "mir/geometry/rectangle.h"
#include "mir/graphics/renderable.h"
#include "occlusion.h"

namespace mc = mir::compositor;
namespace geom = mir::geometry;
namespace mg = mir::graphics;

namespace
{
bool renderable_is_occluded(
    mg::Renderable const& renderable, 
    geom::Rectangle const& area,
    std::vector<geom::Rectangle>& coverage)
{
    static const glm::mat4 identity;
    if (renderable.transformation() != identity)
        return false;  // Weirdly transformed. Assume never occluded.

    if (!renderable.should_be_rendered_in(area))
        return true;  // Not on the display, or invisible; definitely occluded.

    bool occluded = false;
    geom::Rectangle const& window = renderable.screen_position();
    for (const auto &r : coverage)
    {
        if (r.contains(window))
        {
            occluded = true;
            break;
        }
    }

    if (!occluded && renderable.alpha() == 1.0f && !renderable.shaped())
        coverage.push_back(window);

    return occluded;
}
}

void mc::filter_occlusions_from(
    mg::RenderableList& list,
    geom::Rectangle const& area)
{
    mg::RenderableList non_occluded_renderables;
    std::vector<geom::Rectangle> coverage;
    auto it = list.crbegin();
    while (it != list.crend())
    {
        if (!renderable_is_occluded(**it, area, coverage))
            non_occluded_renderables.push_front(*it);

        it++;
    }
    std::swap(list, non_occluded_renderables);
}
