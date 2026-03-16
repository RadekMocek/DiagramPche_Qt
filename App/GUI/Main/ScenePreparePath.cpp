#include "Scene.hpp"
#include "../../Helper/AABR.hpp"
#include "../../Helper/Color.hpp"
#include "../../Model/Path.hpp"
#include "../SceneItem/SceneNode.hpp"
#include "../SceneItem/ScenePath.hpp"

void GUIScene::GUIScenePreparePath(const Path& path)
{
    // Prepare the start point
    QPointF start(path.start.x, path.start.y);
    bool do_start_shift = false;
    if (!path.start.parent_id.empty()) {
        if (const auto it = m_scene_nodes.find(path.start.parent_id); it != m_scene_nodes.end()) {
            // Move start point so it's relative to parent's pivot
            start += it->second->GetExactPointFromPivot(path.start.parent_pivot);
            // Path shift makes sense only when the start/end point is relative to some node
            if (path.shift_start != 0) {
                do_start_shift = true;
            }
        }
    }

    // QGraphicsItem needs to have everything drawn in its boundingRect, which we must set correctly, so we remember the boundaries
    AABRHelper aabr(start);

    // One path can have multiple ends defined by the user => one [[path]] can define multiple result_paths.
    // Every result_path will be defined as a list of points `QList<QPointF>` (start point, maybe some Pathpoints, end point).
    // List of these lists will be given to the draw command, it needs a list of pairs, so there will be duplicates.
    QList<QList<QPointF>> result_paths;

    // Each inner vector starts with the start point; or, if shift != 0 && start point is relative, with OG start point followed by a shifted start point
    const auto n_ends = static_cast<qsizetype>(path.ends.size());
    if (!do_start_shift) {
        result_paths = QList<QList<QPointF>>(n_ends, QList<QPointF>());
    }
    else {
        const auto shifted_start = start + path.GetShiftVector(path.start.parent_pivot, true);
        result_paths = QList<QList<QPointF>>(n_ends, QList({start, shifted_start}));
        start = shifted_start; // Do this so Pathpoints relative to start are relative to this
        aabr.Update(start);
    }

    // Foreach end point
    auto index = 0;
    for (const auto& path_end : path.ends) {
        auto& result_pathpoints = result_paths[index++];

        // Ready the current end point
        QPointF end(path_end.x, path_end.y);
        bool do_end_shift = false;
        if (!path_end.parent_id.empty()) {
            if (const auto it = m_scene_nodes.find(path_end.parent_id); it != m_scene_nodes.end()) {
                end += it->second->GetExactPointFromPivot(path_end.parent_pivot);
                if (path.shift_end != 0) {
                    do_end_shift = true;
                }
            }
        }

        // `shifted_end` is the end point, that all the Pathpoints relate to.
        // If there is no a shift, it is just the original end.
        // If there is a shift, we apply it; we still remember the original end and in this case it will be the last point added to current collection.
        const auto shifted_end = (!do_end_shift) ? end : end + path.GetShiftVector(path_end.parent_pivot, false);

        // Pathpoints (defined as a collection [[path]].points) are points between start and end.
        // They are not mandatory: if no Pathpoints are specified, then path is just a single line from start to end.
        // If there are some, we iterate them and add them to the result collection.
        auto prev(start); // This is for the "prev" to work (Pathpoint relative to previous Pathpoint)

        // ReSharper disable once CppUseStructuredBinding
        for (const auto& pathpoint : path.pathpoints) {
            // Currently processed Pathpoint
            QPointF curr(pathpoint.x, pathpoint.y);
            // Apply the Pathpoint type for both coordinates
            // X
            switch (pathpoint.x_type) {
            case PPTYPE_ABSOLUTE:
                break;
            case PPTYPE_REFERENCE:
                if (const auto it = m_scene_nodes.find(pathpoint.x_parent_id); it != m_scene_nodes.end()) {
                    curr.rx() += it->second->GetExactPointFromPivot(pathpoint.x_parent_pivot).x();
                }
                break;
            case PPTYPE_START:
                curr.rx() += start.x();
                break;
            case PPTYPE_END:
                curr.rx() += shifted_end.x();
                break;
            case PPTYPE_PREVIOUS:
                curr.rx() += prev.x();
                break;
            }
            // Y
            switch (pathpoint.y_type) {
            case PPTYPE_ABSOLUTE:
                break;
            case PPTYPE_REFERENCE:
                if (const auto it = m_scene_nodes.find(pathpoint.y_parent_id); it != m_scene_nodes.end()) {
                    curr.ry() += it->second->GetExactPointFromPivot(pathpoint.y_parent_pivot).y();
                }
                break;
            case PPTYPE_START:
                curr.ry() += start.y();
                break;
            case PPTYPE_END:
                curr.ry() += shifted_end.y();
                break;
            case PPTYPE_PREVIOUS:
                curr.ry() += prev.y();
                break;
            }

            // Pathpoint is ready now
            result_pathpoints.append(prev);
            result_pathpoints.append(curr);
            aabr.Update(curr);

            // Ready for the next iteration
            prev = curr;
        }

        // After the Pathpoints are ready add the endpoint(s)
        result_pathpoints.append(prev);
        result_pathpoints.append(shifted_end);
        aabr.Update(shifted_end);
        if (do_end_shift) {
            result_pathpoints.append(shifted_end);
            result_pathpoints.append(end);
            aabr.Update(end);
        }
    }

    //
    auto* item = new ScenePath({
        aabr.ToQRectF(), result_paths, GetQColorFromTuple(path.color), path.do_start_arrow, path.do_end_arrow
    });
    item->setZValue(DLUserChannelToRealChannel(path.z, false));
    addItem(item);

    //
    m_scene_aabr.UpdateWithQRectF(aabr.ToQRectF());
}
