#include "map3d.h"

#include "error.h"
#include "file.h"
#include "mem.h"
#include "material.h"
#include "main.h"

lpMap3D Map3D_Create(lpContainer root, const char* filename, F32 blockSize, F32 roughLevel)
{
    lpMapFileInfo mapFileInfo = File_LoadBinary(filename, NULL);
    if (mapFileInfo == NULL)
        return NULL;

    lpMap3D map = Mem_Alloc(sizeof(Map3D));
    if (map == NULL)
    {
        Mem_Free(mapFileInfo);
        return NULL;
    }

    map->blockSize = blockSize;
    map->roughLevel = roughLevel;
    Map3D_MapFileGetSpecs(mapFileInfo, &map->gridWidth, &map->gridHeight);
    map->blockWidth = map->gridWidth - 1;
    map->blockHeight = map->gridHeight - 1;
    map->worldDimensions_fnegx.width = -((F32)(U64)(map->blockWidth) * blockSize * 0.5f);
    map->worldDimensions_fnegx.height = ((F32)(U64)(map->blockHeight) * blockSize * 0.5f);
    map->textureSet = NULL;

    memset(map->texsNum, 0, MAP3D_MAXTEXTURES);

    map->flagsMap = MAP3D_FLAG_NONE;
    map->visibleBlocksNum = 0;
    map->uvBlocksNum = 0;
    map->float_20 = 0.0f;

    // TODO: THIS MATERIAL IS NEVER USED, AND IS NEVER RELEASED!!!
    map->material = Material_Create(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    memset(map->transBlocks, 0, sizeof(Map3D_TransitionBlock) * 10);
    memset(map->texsNoFade, 0, sizeof(B32) * MAP3D_MAXTEXTURES);

    for (U32 i = 0; i < 10; i++)
        map->transBlocks[i].groupID = D3DRMGROUP_ALLGROUPS; // (not created yet)

    map->transMesh = Container_MakeMeshTrans(root);
    map->mesh = Container_MakeMeshImmediate(root);

    if (map->mesh != NULL)
    {
        map->blocks3D = Mem_Alloc(sizeof(Map3D_Block) * map->gridWidth * map->gridHeight);
        if (map->blocks3D != NULL)
        {
            U32 faceData[6];
            faceData[0] = 0;
            faceData[1] = 1;
            faceData[2] = 3;
            faceData[3] = 1;
            faceData[4] = 2;
            faceData[5] = 3;

            if (map->blockWidth * map->blockHeight != 0)
            {
                U32 iter = 0;
                do
                {
                    U32 group = Container_Mesh_AddGroup(map->mesh, 4, 2, 3, faceData);
                    Container_Mesh_HideGroup(map->mesh, group, TRUE);
                    Container_Mesh_SetQuality(map->mesh, group, Gouraud);

                    iter++;
                } while (iter < map->blockWidth * map->blockHeight);
            }

            if (map->gridHeight != 0)
            {
                U32 yIter = 0;
                do
                {
                    if (map->gridWidth != 0)
                    {
                        U32 xIter = 0;
                        do
                        {
                            map->blocks3D[yIter * map->gridWidth + xIter].flags3D = MAP3DBLOCK_FLAG_UNK_20 | MAP3DBLOCK_FLAG_UNK_40;
                            map->blocks3D[yIter * map->gridWidth + xIter].texture = TEXTURE_FLOOR_STD;
                            map->blocks3D[yIter * map->gridWidth + xIter].heightValue = Map3D_MapFileBlockValue(mapFileInfo, xIter, yIter, map->gridWidth);

                            F32 fVar8 = Maths_RandRange(0.0f, 1.0f);
                            F32 fVar8_2 = Maths_RandRange(0.0f, 1.0f);
                            F32 fVar9 = 1.0f / (F32)sqrt(fVar8 * fVar8 + fVar8_2 * fVar8_2);
                            map->blocks3D[yIter * map->gridWidth + xIter].uvCoord.x = fVar9 * fVar8 * 0.3f;
                            map->blocks3D[yIter * map->gridWidth + xIter].uvCoord.y = fVar9 * fVar8_2 * 0.3f;

                            map->blocks3D[yIter * map->gridWidth + xIter].highlight = 0;

                            xIter++;
                        } while (xIter < map->gridWidth);
                    }
                    yIter++;
                } while(yIter < map->gridHeight);
            }

            Map3D_InitRoughness(map);

            if (map->blockHeight != 0)
            {
                U32 yIter = 0;
                do
                {
                    U32 xIter = 0;
                    if (map->blockWidth != 0)
                    {
                        do
                        {
                            U32 iter = yIter * map->gridWidth + xIter;
                            U32 iterNext = (yIter + 1) * map->gridWidth + xIter;

                            // Compare crossed height differences
                            // Condition is true when abs(x0y0 - x1y1) < diff(x1y0 - x0y1).
                            S32 diffxy00_11 = map->blocks3D[iter].heightValue - map->blocks3D[iterNext + 1].heightValue;
                            S32 diffxy10_01 = map->blocks3D[iter + 1].heightValue - map->blocks3D[iterNext].heightValue;
                            // diffxy00_11 = (blocks3D[y][x].heightValue - blocks3D[y+1][x+1].heightValue)
                            // diffxy10_01 = (blocks3D[y][x+1].heightValue - blocks3D[y+1][x].heightValue)

                            if (abs(diffxy00_11) < abs(diffxy10_01))
                                Map3D_SetBlockRotated(map, xIter, yIter, TRUE);
                            else
                                Map3D_GenerateBlockPlaneNormals(map, xIter, yIter);

                            xIter++;
                        } while (xIter < map->blockWidth);
                    }

                    yIter++;
                } while(yIter < map->blockHeight);
            }

            Mem_Free(mapFileInfo);
            return map;
        }
        Container_Remove(map->mesh);
    }
    Mem_Free(map);
    Mem_Free(mapFileInfo);

    return NULL;
}

void Map3D_MapFileGetSpecs(lpMapFileInfo mapFileInfo, U32* width, U32* height)
{
    *width = mapFileInfo->dimensions.width;
    *height = mapFileInfo->dimensions.height;
}

U16 Map3D_MapFileBlockValue(lpMapFileInfo mapFile, U32 bx, U32 by, U32 gridWidth)
{
    return mapFile->blocks[gridWidth * by + bx];
}

B32 Map3D_BlockToWorldPos(lpMap3D map, U32 bx, U32 by, F32* outX, F32* outY)
{
    if (bx < map->blockWidth && by < map->blockHeight)
    {
        *outX = map->blockSize * 0.5f + (F32)bx * map->blockSize + map->worldDimensions_fnegx.width;
        *outY = map->worldDimensions_fnegx.height - (map->blockSize * 0.5f + (F32)by * map->blockSize);
        return TRUE;
    }
    return FALSE;
}

B32 Map3D_WorldToBlockPos(lpMap3D map, F32 x, F32 y, S32* outBx, S32* outBy, F32* outZ)
{
    // decompiled code
    /*
        longlong lVar4 = __ftol(((float10)in_x - (float10)(map->worldDimensions_fnegx).width) / (float10)map->blockSize);
        int xPos = (int)lVar4;
        lVar4 = __ftol(((float10)(map->worldDimensions_fnegx).height - (float10)in_y) / (float10)map->blockSize);
        int yPos = (int)lVar4;
    */

    // TODO: Verify that this is correct
    S32 xPos = (S32)((x - map->worldDimensions_fnegx.width) / map->blockSize);
    S32 yPos = (S32)((map->worldDimensions_fnegx.height - y) / map->blockSize);

    if (outBx != NULL)
        *outBx = xPos;
    if (outBy != NULL)
        *outBy = yPos;

    if (-1 < xPos && xPos < (S32)map->blockWidth && -1 < yPos && yPos < (S32)map->blockHeight)
    {
        if (outZ != NULL)
        {
            F32 fVar1 = map->blockSize * 0.5f;
            F32 tmp = (F32)xPos * map->blockSize;
            tmp = tmp + map->worldDimensions_fnegx.width;
            x = (x - tmp);
            x = x / fVar1;
            tmp = (F32)yPos * map->blockSize;
            tmp = map->worldDimensions_fnegx.height - tmp;
            tmp = y - tmp;
            tmp = tmp / fVar1;
            fVar1 = -tmp;
            if (1.0f < x)
                x = 2.0f - x;
            if (1.0f < fVar1)
                fVar1 = 2.0f - fVar1;
            if (x < fVar1)
                fVar1 = x;

            *outZ = fVar1;
        }
        return TRUE;
    }

    return FALSE;
}

B32 Map3D_WorldToBlockPos_NoZ(lpMap3D map, F32 x, F32 y, S32* outBx, S32* outBy)
{
    return Map3D_WorldToBlockPos(map, x, y, outBx, outBy, (S32)&outBy);
}

F32 Map3D_GetWorldZ(lpMap3D map, F32 xPos, F32 yPos)
{
    U32 bx;
    U32 by;
    if (Map3D_WorldToBlockPos_NoZ(map, xPos, yPos, &bx, &by))
    {
        Point3F positions[4];
        if (!Map3D_GetBlockVertexPositions(map, bx, by, &positions))
        {
            if (map->blockSize <= (xPos - positions[0].x) + (positions[0].y - yPos))
                positions[0].z = (positions[3].z - positions[2].z) + (positions[1].z - positions[2].z) + positions[2].z;
            else
                positions[2].z = (positions[3].z - positions[0].z) + (positions[1].z - positions[0].z) + positions[0].z;
        }
        else if (map->blockSize <= (positions[1].x - xPos) + (positions[1].y - yPos))
        {
            positions[1].z = (positions[2].z - positions[3].z) + (positions[0].z - positions[3].z) + positions[3].z;
        }
        else
        {
            positions[3].z = (positions[2].z - positions[1].z) + (positions[0].z - positions[1].z) + positions[1].z;
        }

        F32 f = (positions[2].z - positions[3].z) * ((positions[2].x - positions[3].x) / (xPos - positions[3].x)) + positions[3].z;

        return ((positions[0].y - positions[3].y) / (yPos - positions[3].y)) *
            (((positions[1].z - positions[0].z) * ((xPos - positions[0].x) / (positions[1].x - positions[0].x)) + positions[0].z) - f) + f;
    }

    return 0.0f;
}

F32 Map3D_UnkCameraXYFunc_RetZunk(lpMap3D map, F32 xPos, F32 yPos)
{
    S32 bx, by;
    if (!Map3D_WorldToBlockPos_NoZ(map, xPos, yPos, &bx, &by))
        return 0.0f;

    Point2F OFFSETS[4];

    OFFSETS[0].x = 0.0f;
    OFFSETS[0].y = 0.0f;
    OFFSETS[1].x = 1.0f;
    OFFSETS[1].y = 0.0f;
    OFFSETS[2].x = 1.0f;
    OFFSETS[2].y = 1.0f;
    OFFSETS[3].x = 0.0f;
    OFFSETS[3].y = 1.0f;

    Point3F offsets2[4];

    S32 i = 4;
    do
    {
        i--;

        offsets2[i].x = (OFFSETS[i].x + (F32) bx) * map->blockSize + map->worldDimensions_fnegx.width;
        offsets2[i].y = map->worldDimensions_fnegx.height - ((OFFSETS[i].y + (F32) by) * map->blockSize);

        // TODO: Verify this is correct
        S32 iVar = (by + (S32) OFFSETS[i].y) * map->gridWidth + bx;
        F32 heightValue = map->blocks3D[iVar + (S32) OFFSETS[i].x].heightValue;
        offsets2[i].z = -(heightValue * map->roughLevel);

    } while (i != 0);

    F32 f;
    if (map->blockSize <= (offsets2[1].x - xPos) + (offsets2[1].y - yPos))
    {
        f = offsets2[3].z;
        offsets2[1].z = (offsets2[2].z - offsets2[3].z) + (offsets2[0].z - offsets2[3].z) + offsets2[3].z;
    }
    else
    {
        f = (offsets2[2].z - offsets2[1].z) + (offsets2[0].z - offsets2[1].z) + offsets2[1].z;
    }

    f = (offsets2[2].z - f) * ((offsets2[2].x - offsets2[3].x) / (xPos - offsets2[3].x)) + f;

    // TODO: Verify this is correct
    return ((offsets2[0].y - offsets2[3].y) / (yPos - offsets2[3].y)) *
        (((offsets2[1].z - offsets2[0].z) * ((xPos - offsets2[0].x) / (offsets2[1].x - offsets2[0].x)) + offsets2[0].z) - f) + f;
}

B32 Map3D_GetIntersections(lpMap3D map, lpViewport view, U32 mouseX, U32 mouseY, U32 *outBx, U32 *outBy, Point3F *outVector)
{
    Point2F mousePos;
    Point2I blockPos;

    mousePos.x = (F32)(U64)mouseX;
    blockPos.x = mouseY;
    blockPos.y = 0;
    mousePos.y = (F32)(U64)mouseY;

    Point4F xformMouseZ0;
    Point4F xformMouseZ1;

    // xformMouseZ0 = Point4F{mouseX, mouseY, 0.0, 1.0};
    xformMouseZ0.x = mousePos.x;
    xformMouseZ0.y = mousePos.y;
    xformMouseZ0.z = 0.0f;
    xformMouseZ0.w = 1.0f;

    // xformMouseZ1 = Point4F{mouseX, mouseY, 1.0, 1.0};
    xformMouseZ1.x = mousePos.x;
    xformMouseZ1.y = mousePos.y;
    xformMouseZ1.z = 1.0f;
    xformMouseZ1.w = 1.0f;

    Point3F rayOrigin;
    Point3F ray;

    Viewport_InverseTransform(view, &rayOrigin, &xformMouseZ0);
    Viewport_InverseTransform(view, &ray, &xformMouseZ1);

    F32 x = ray.x - rayOrigin.x;
    F32 y = ray.y - rayOrigin.y;
    F32 z = ray.z - rayOrigin.z;
    F32 length = 1.0f / sqrtf(x * x + y * y + z * z);

    ray.x = x * length;
    ray.y = y * length;
    ray.z = z * length;

    if (!Map3D_Intersections_Sub1_FUN_00450820(map, &rayOrigin, &ray, outVector, &blockPos, 20))
        return FALSE;

    // Do a 2D loop in range: (-2, -2) -> (2, 2)
    Point3F vertPoses[4];
    Point2I loopOffset;
    loopOffset.y = -2;
    do
    {
        loopOffset.x = -2;
        do
        {
            if ((U32)(loopOffset.x + blockPos.x) < map->blockWidth && (U32)(blockPos.y + loopOffset.y) < map->blockHeight)
            {
                Map3D_GetBlockVertexPositions(map, loopOffset.x + blockPos.x, loopOffset.y + blockPos.y, vertPoses);

                Point3F* vector = vertPoses;
                S32 i = 4;

                Point4F xformPoly;
                Point2F polyPoints[5];

                do
                {
                    Viewport_Transform(view, &xformPoly, vector);

                    vector++;
                    i--;

                    // TODO: Verify that this is correctly decompiled
                    polyPoints[i].x = xformPoly.x / xformPoly.w;
                    polyPoints[i].y = xformPoly.y / xformPoly.w;
                } while (i != 0);

                polyPoints[4].x = polyPoints[0].x;
                polyPoints[4].y = polyPoints[0].y;

                if (Maths_PointInsidePoly(&mousePos, polyPoints, &polyPoints[1], 4))
                {
                    *outBx = blockPos.x + loopOffset.x;
                    *outBy = blockPos.y + loopOffset.y;
                    Map3D_Intersections_Sub2_FUN_004518a0(map, *outBx, *outBy, &rayOrigin, &ray, outVector);
                    return TRUE;
                }
            }
            loopOffset.x++;
        } while (loopOffset.x < 3);
        loopOffset.y++;
        if (2 < loopOffset.y)
            return FALSE;
    } while (TRUE);
}

B32 Map3D_Intersections_Sub1_FUN_00450820(lpMap3D map, Point3F *rayOrigin, Point3F *ray, Point3F *outEndPoint, Point2I *outBlockPos, S32 unkCount)
{
    Point3F planePoint;
    Point3F planeNormal;

    planePoint.x = 0.0f;
    planePoint.y = 0.0f;
    planePoint.z = -50.0f;

    planeNormal.x = 0.0f;
    planeNormal.y = 0.0f;
    planeNormal.z = -1.0f;

    if (unkCount != 0)
    {
        do
        {
            Maths_RayPlaneIntersection(outEndPoint, rayOrigin, ray, &planePoint, &planeNormal);
            planePoint.z = Map3D_GetWorldZ(map, outEndPoint->x, outEndPoint->y);

            unkCount--;
        } while (unkCount != 0);
    }

    return Map3D_WorldToBlockPos_NoZ(map, outEndPoint->x, outEndPoint->y, &outBlockPos->x, &outBlockPos->y);
}

B32 Map3D_Intersections_Sub2_FUN_004518a0(lpMap3D map, U32 bx, U32 by, Point3F *rayOrigin, Point3F *ray, Point3F *outVector)
{
    Point2F points[4];

    Point3F positions[4];
    B32 rotated = Map3D_GetBlockVertexPositions(map, bx, by, positions);

    B32 flag1 = FALSE;
    B32 flag2 = FALSE;

    Point3F normal;
    normal.x = map->blocks3D[by * map->gridWidth + bx].normalA.x;
    normal.y = map->blocks3D[by * map->gridWidth + bx].normalA.y;
    normal.z = map->blocks3D[by * map->gridWidth + bx].normalA.z;

    Point3F endPoint;
    if (Maths_RayPlaneIntersection(&endPoint, rayOrigin, ray, &positions[0], &normal))
    {
        if (rotated)
        {
            points[1].x = positions[2].x;
            points[1].y = positions[2].y;
        }
        else
        {
            points[1].x = positions[1].x;
            points[1].y = positions[1].y;
        }

        Point2F endPointCopy;
        endPointCopy.x = endPoint.x;
        endPointCopy.y = endPoint.y;

        if (Maths_PointInsidePoly(&endPointCopy, points, &points[1], 3))
        {
            flag1 = TRUE;
        }
    }

    normal.x = map->blocks3D[by * map->gridWidth + bx].normalB.x;
    normal.y = map->blocks3D[by * map->gridWidth + bx].normalB.y;
    normal.z = map->blocks3D[by * map->gridWidth + bx].normalB.z;

    Point3F endPoint2;
    if (Maths_RayPlaneIntersection(&endPoint2, rayOrigin, ray, &positions[2], &normal))
    {
        if (rotated)
        {
            points[1].x = positions[1].x;
            points[1].y = positions[1].y;
            points[2].x = positions[2].x;
            points[2].y = positions[2].y;
            points[3].x = positions[0].x;
            points[3].y = positions[0].y;
        }
        else
        {
            points[1].x = positions[2].x;
            points[1].y = positions[3].y;
            points[2].x = positions[3].x;
            points[2].y = positions[3].y;
            points[3].x = positions[1].x;
            points[3].y = positions[1].y;
        }

        Point2F endPointCopy;
        endPointCopy.x = endPoint2.x;
        endPointCopy.y = endPoint2.y;

        points[0].x = points[3].x;
        points[0].y = points[3].y;

        if (Maths_PointInsidePoly(&endPointCopy, points, &points[1], 3))
        {
            flag2 = TRUE;
        }
    }

    if (flag1)
    {
        if (flag2)
        {
            F32 x1 = endPoint.x - rayOrigin->x;
            F32 x2 = endPoint2.x - rayOrigin->x;
            F32 y1 = endPoint.y - rayOrigin->y;
            F32 y2 = endPoint2.y - rayOrigin->y;
            F32 z1 = endPoint.z - rayOrigin->z;
            F32 z2 = endPoint2.z - rayOrigin->z;
            if (sqrtf(z1 * z1 + y1 * y1 + x1 * x1) < sqrtf(z2 * z2 + y2 * y2 + x2 * x2))
            {
                outVector->x = endPoint.x;
                outVector->y = endPoint.y;
                outVector->z = endPoint.z;

                return TRUE;
            }
        }
        else
        {
            if (flag1)
            {
                outVector->x = endPoint.x;
                outVector->y = endPoint.y;
                outVector->z = endPoint.z;

                return TRUE;
            }
        }
    }
    else if (!flag2)
    {
        return FALSE;
    }

    outVector->x = endPoint2.x;
    outVector->y = endPoint2.y;
    outVector->z = endPoint2.z;

    return TRUE;
}

B32 Map3D_GetBlockVertexPositions(lpMap3D map, U32 bx, U32 by, Point3F* outVertPositions)
{
    Vertex vertices[4];

    if (bx < map->blockWidth && by < map->blockHeight)
    {
        Container_Mesh_GetVertices(map->mesh, by * map->blockWidth + bx, 0, 4, vertices);

        if ((map->blocks3D[map->gridWidth * by + bx].flags3D & MAP3DBLOCK_FLAG_ROTATED) != MAP3DBLOCK_FLAG_NONE)
        {
            // Return vertices shifted down once.
            outVertPositions[0].x = vertices[1].position.x;
            outVertPositions[0].y = vertices[1].position.y;
            outVertPositions[0].z = vertices[1].position.z;
            outVertPositions[1].x = vertices[2].position.x;
            outVertPositions[1].y = vertices[2].position.y;
            outVertPositions[1].z = vertices[2].position.z;
            outVertPositions[2].x = vertices[3].position.x;
            outVertPositions[2].y = vertices[3].position.y;
            outVertPositions[2].z = vertices[3].position.z;
            outVertPositions[3].x = vertices[0].position.x;
            outVertPositions[3].y = vertices[0].position.y;
            outVertPositions[3].z = vertices[0].position.z;

            return TRUE;
        } else
        {
            // Return vertices in original position.
            outVertPositions[0].x = vertices[0].position.x;
            outVertPositions[0].y = vertices[0].position.y;
            outVertPositions[0].z = vertices[0].position.z;
            outVertPositions[1].x = vertices[1].position.x;
            outVertPositions[1].y = vertices[1].position.y;
            outVertPositions[1].z = vertices[1].position.z;
            outVertPositions[2].x = vertices[2].position.x;
            outVertPositions[2].y = vertices[2].position.y;
            outVertPositions[2].z = vertices[2].position.z;
            outVertPositions[3].x = vertices[3].position.x;
            outVertPositions[3].y = vertices[3].position.y;
            outVertPositions[3].z = vertices[3].position.z;

            return FALSE;
        }

    }

    // TODO: Originally this returned map->blockWidth which is really weird. Potential bug?
    return FALSE;
}

B32 Map3D_GetBlockDirectionNormal(lpMap3D map, U32 bx, U32 by, Direction direction, Point3F* outNormal)
{
    if (bx < map->blockWidth && by < map->blockHeight)
    {
        Map3D_Block block3D = map->blocks3D[by * map->gridWidth + bx];

        if ((block3D.flags3D & MAP3DBLOCK_FLAG_ROTATED) != MAP3DBLOCK_FLAG_NONE)
            direction = (direction + 1) % DIRECTION_COUNT;

        if (direction == DIRECTION_UP)
            *outNormal = block3D.normalA;

        if (direction == DIRECTION_DOWN)
            *outNormal = block3D.normalB;

        if (direction == DIRECTION_RIGHT || direction == DIRECTION_LEFT)
        {
            Maths_Vector3DAdd(outNormal, &block3D.normalB, &block3D.normalA);
            Maths_Vector3DNormalize(outNormal);
        }

        return TRUE;
    }

    outNormal->x = 0.0f;
    outNormal->y = 0.0f;
    outNormal->z = 0.0f;
    return FALSE;
}

void Map3D_SetBlockDirectionNormal(lpMap3D map, U32 bx, U32 by, Direction direction, Point3F* normal)
{
    D3DRMGROUPINDEX groupID = by * map->blockWidth + bx;
    Vertex vertices[1];

    if (bx < map->blockWidth && by < map->blockHeight)
    {
        if ((map->blocks3D[map->gridWidth * by + bx].flags3D & MAP3DBLOCK_FLAG_ROTATED) != MAP3DBLOCK_FLAG_NONE)
            direction = (direction + 1) % DIRECTION_COUNT;

        Container_Mesh_GetVertices(map->mesh, groupID, direction, 1, vertices);

        vertices[0].normal.x = normal->x;
        vertices[0].normal.y = normal->y;
        vertices[0].normal.z = normal->z;

        Container_Mesh_SetVertices(map->mesh, groupID, direction, 1, vertices);
    }
}

B32 Map3D_BlockPairHasTextureMatch(lpMap3D map, U32 bx1, U32 by1, U32 bx2, U32 by2)
{
    if ((((by1 != by2 || bx1 != bx2) && bx1 < map->blockWidth) && ((by1 < map->blockHeight && bx2 < map->blockWidth))) && by2 < map->blockHeight)
    {
        SurfaceTexture tex1 = map->blocks3D[by1 * map->gridWidth + bx1].texture;
        SurfaceTexture tex2 = map->blocks3D[by2 * map->gridWidth + bx2].texture;

        // Find a texture coord match for the 2 block positions
        if (tex1 != TEXTURE__INVALID && tex2 != TEXTURE__INVALID)
        {
            S32 i = 0;
            if (map->texsNum[tex1] != 0)
            {
                do
                {
                    if (map->texsGrid[tex1][i] == tex2)
                        return TRUE;

                    i++;
                } while (i < map->texsNum[tex1]);

                return FALSE;
            }
        }

    }

    return FALSE;
}

void Map3D_InitRoughness(lpMap3D map)
{
    Vertex vertices[4];

    Point2F OFFSETS[4];

    OFFSETS[0].x = 0.0f;
    OFFSETS[0].y = 0.0f;
    OFFSETS[1].x = 1.0f;
    OFFSETS[1].y = 0.0f;
    OFFSETS[2].x = 1.0f;
    OFFSETS[2].y = 1.0f;
    OFFSETS[3].x = 0.0f;
    OFFSETS[3].y = 1.0f;

    U32 y = 0;
    if (map->blockHeight != 0)
    {
        do
        {
            U32 x = 0;
            if (map->blockWidth != 0)
            {
                do
                {
                    Point2F* pOffset = OFFSETS;

                    S32 i = 3;
                    do
                    {
                        vertices[i].position.x = ((F32)x + pOffset->x) * map->blockSize + map->worldDimensions_fnegx.width;
                        vertices[i].position.y = map->worldDimensions_fnegx.height - (pOffset->y + (F32)y) * map->blockSize;

                        vertices[i].position.z = -((F32)(U32)map->blocks3D[(y + (S32)pOffset->y) * map->gridWidth + x + (S32)pOffset->x].heightValue * map->roughLevel);
                        if (vertices[i].position.z < map->float_20)
                            map->float_20 = vertices[i].position.z;

                        pOffset++;
                        i--;
                    } while (i >= 0);

                    Container_Mesh_SetVertices(map->mesh, y * map->blockWidth + x, 0, 4, vertices);

                    x++;
                } while (x < map->blockWidth);
            }
            y++;
        } while (y < map->blockHeight);
    }
}

// This seems to be used when the height difference of diagonal vertex points differs.
//
// diffxy00_11 = (blocks3D[y][x].heightValue - blocks3D[y+1][x+1].heightValue)
// diffxy10_01 = (blocks3D[y][x+1].heightValue - blocks3D[y+1][x].heightValue)
// rotated = (abs(diffxy00_11) < std::abs(diffxy10_01));
void Map3D_SetBlockRotated(lpMap3D map, U32 bx, U32 by, B32 on)
{
    if ((on != 0) != Map3D_IsBlockRotated(map, bx, by))
    {
        Vertex vertices[4];
        Vertex vertexSwap;

        U32 groupID = map->blockWidth * by + bx;
        Container_Mesh_GetVertices(map->mesh, groupID, 0, 4, vertices);

        if ((map->blocks3D[by * map->gridWidth + bx].flags3D & MAP3DBLOCK_FLAG_ROTATED) == MAP3DBLOCK_FLAG_NONE)
        {
            map->blocks3D[by * map->gridWidth + bx].flags3D |= MAP3DBLOCK_FLAG_ROTATED;

            // Shift vertices up one
            memcpy(&vertexSwap,  &vertices[3], sizeof(D3DRMVERTEX));
            memcpy(&vertices[3], &vertices[2], sizeof(D3DRMVERTEX));
            memcpy(&vertices[2], &vertices[1], sizeof(D3DRMVERTEX));
            memcpy(&vertices[1], &vertices[0], sizeof(D3DRMVERTEX));
            memcpy(&vertices[0], &vertexSwap,  sizeof(D3DRMVERTEX));
        }
        else
        {
            map->blocks3D[by * map->gridWidth + bx].flags3D &= ~MAP3DBLOCK_FLAG_ROTATED;

            // Shift vertices down one
            memcpy(&vertexSwap,  &vertices[0], sizeof(D3DRMVERTEX));
            memcpy(&vertices[0], &vertices[1], sizeof(D3DRMVERTEX));
            memcpy(&vertices[1], &vertices[2], sizeof(D3DRMVERTEX));
            memcpy(&vertices[2], &vertices[3], sizeof(D3DRMVERTEX));
            memcpy(&vertices[3], &vertexSwap,  sizeof(D3DRMVERTEX));
        }

        Container_Mesh_SetVertices(map->mesh, groupID, 0, 4, vertices);

        Map3D_GenerateBlockPlaneNormals(map, bx, by);
        // Perform action in a square: { { 0, 0 }, { 1, 0 },
        //                               { 0, 1 }, { 1, 1 } }
        Map3D_SetBlockVertexModified(map, bx, by);
        Map3D_SetBlockVertexModified(map, bx + 1, by);
        Map3D_SetBlockVertexModified(map, bx + 1, by + 1);
        Map3D_SetBlockVertexModified(map, bx, by + 1);
    }
}

B32 Map3D_IsBlockRotated(lpMap3D map, U32 bx, U32 by)
{
    return (map->blocks3D[by * map->gridWidth + bx].flags3D & MAP3DBLOCK_FLAG_ROTATED) != MAP3DBLOCK_FLAG_NONE;
}

void Map3D_SetBlockVertexModified(lpMap3D map, U32 vx, U32 vy)
{
    map->blocks3D[map->gridWidth * vy + vx].flags3D |= MAP3DBLOCK_FLAG_VERTEXMODIFIED;
    map->flagsMap |= MAP3D_FLAG_VERTEXMODIFIED;
}

void Map3D_MoveBlockDirectionVertex(lpMap3D map, U32 bx, U32 by, Direction direction, Point3F* vertDist)
{
    D3DRMGROUPINDEX groupID = by * map->blockWidth + bx;
    Vertex vertices[1];

    if (bx < map->blockWidth && by < map->blockHeight)
    {
        if ((map->blocks3D[map->gridWidth * by + bx].flags3D & MAP3DBLOCK_FLAG_ROTATED) != 0)
            direction = direction + DIRECTION_RIGHT & DIRECTION_LEFT;
        Container_Mesh_GetVertices(map->mesh, groupID, direction, 1, vertices);

        vertices[0].position.x += vertDist[0].x;
        vertices[0].position.y += vertDist[0].y;
        vertices[0].position.z += vertDist[0].z;

        Container_Mesh_SetVertices(map->mesh, groupID, direction, 1, vertices);
    }
}

void Map3D_MoveBlockVertices(lpMap3D map, U32 bx, U32 by, F32 zDist)
{
    Point3F vertDists[1];
    vertDists[0].x = 0.0f;
    vertDists[0].y = 0.0f;
    vertDists[0].z = zDist;

    Point2F OFFSETS_M1[4];
    OFFSETS_M1[0].x = 0.0f;
    OFFSETS_M1[0].y = 0.0f;
    OFFSETS_M1[1].x = -1.0f;
    OFFSETS_M1[1].y = 0.0f;
    OFFSETS_M1[2].x = -1.0f;
    OFFSETS_M1[2].y = -1.0f;
    OFFSETS_M1[3].x = 0.0f;
    OFFSETS_M1[3].y = -1.0f;

    for (U32 i = 0; i < 4; i++)
    {
        OFFSETS_M1[i].x += (F32)bx;
        OFFSETS_M1[i].y += (F32)by;
    }

    for (U32 i = 0; i < DIRECTION_COUNT; i++)
    {
        Map3D_MoveBlockDirectionVertex(map, OFFSETS_M1[i].x, OFFSETS_M1[i].y, (Direction)i, vertDists);
    }

    for (U32 y = by - 1; y <= by; y++)
    {
        for (U32 x = bx - 1; x <= bx; x++)
        {
            Map3D_GenerateBlockPlaneNormals(map, x, y);
        }
    }
}

void Map3D_GenerateBlockPlaneNormals(lpMap3D map, U32 bx, U32 by)
{
    Vertex vertices[4];

    U32 width = map->blockWidth;

    if (bx < width && by < map->blockHeight)
    {
        Container_Mesh_GetVertices(map->mesh, by * width + bx, 0, 4, vertices);
        // (&surfMap->blocks3D[by * surfMap->gridWidth + bx].vector_0,
        //   &vertices[0].position, &vertices[1].position, &vertices[2].position)

        Maths_PlaneNormal(&map->blocks3D[by * map->gridWidth + bx].normalA, &vertices[0].position, &vertices[1].position, &vertices[3].position);
        // (&surfMap->blocks3D[by * surfMap-gridWidth + bx].vector_c,
        //   &vertices[1].position, &vertices[2].position, &vertices[3].position)

        // NOTE: index is shifted one after the previous call
        Maths_PlaneNormal(&map->blocks3D[by * map->gridWidth + bx].normalB, &vertices[1].position, &vertices[2].position, &vertices[3].position);
    }
}

void Map3D_SetPerspectiveCorrectionAll(lpMap3D map, B32 on)
{
    for (U32 y = 0; y < map->blockHeight; y++)
    {
        for (U32 x = 0; x < map->blockWidth; x++)
        {
            Container_Mesh_SetPerspectiveCorrection(map->mesh, y * map->blockWidth + x, on);
        }
    }
}

void Map3D_SetEmissive(lpMap3D map, B32 on)
{
    if (on && (map->flagsMap & MAP3D_FLAG_EMISSIVE_4) == MAP3D_FLAG_NONE)
    {
        for (U32 i = 0; i < map->uvBlocksNum; i++)
        {
            Coord2U coords = map->uvBlocksTable[i];
            Container_Mesh_SetEmissive(map->mesh, coords.y * map->blockWidth + coords.x, 0.3f, 0.3f, 0.3f);
        }
        map->flagsMap |= MAP3D_FLAG_EMISSIVE_4;
    }
    else if (!on && (map->flagsMap & MAP3D_FLAG_EMISSIVE_4) != MAP3D_FLAG_NONE)
    {
        for (U32 i = 0; i < map->uvBlocksNum; i++)
        {
            Coord2U coords = map->uvBlocksTable[i];
            Container_Mesh_SetEmissive(map->mesh, coords.y * map->blockWidth + coords.x, 0.0f, 0.0f, 0.0f);
        }
        map->flagsMap &= ~MAP3D_FLAG_EMISSIVE_4;
    }
}

void Map3D_SetTextureSet(lpMap3D map, Detail_TextureSet* textureSet)
{
    map->textureSet = textureSet;
}

void Map3D_Update(lpMap3D map, F32 elapsedGame)
{
    if ((map->flagsMap & MAP3D_FLAG_VERTEXMODIFIED) != MAP3D_FLAG_NONE)
    {
        U32 by = 0;
        if (map->gridHeight != 0)
        {
            do
            {
                U32 bx = 0;
                if (map->gridWidth != 0)
                {
                    do
                    {
                        if ((map->blocks3D[by * map->gridWidth + bx].flags3D & MAP3DBLOCK_FLAG_VERTEXMODIFIED) != MAP3DBLOCK_FLAG_NONE)
                        {
                            Map3D_UpdateBlockNormals(map, bx, by);
                            map->blocks3D[by * map->gridWidth + bx].flags3D &= ~MAP3DBLOCK_FLAG_VERTEXMODIFIED;
                        }

                        bx++;
                    } while (bx < map->gridWidth);
                }

                by++;
            } while (by < map->gridHeight);
        }
        map->flagsMap &= ~MAP3D_FLAG_VERTEXMODIFIED;
    }

    Map3D_UpdateTextureUVs(map, elapsedGame);
    Map3D_UpdateFadeInTransitions(map, elapsedGame);
}

void Map3D_UpdateAllBlockNormals(lpMap3D map)
{
    for (U32 vy = 0; vy < map->gridHeight; vy++)
    {
        for (U32 vx = 0; vx < map->gridWidth; vx++)
        {
            Map3D_UpdateBlockNormals(map, vx, vy);
        }
    }
}

void Map3D_UpdateBlockNormals(lpMap3D map, U32 bx, U32 by)
{
    Point2F blockOffsets[4];
    blockOffsets[0].x = 0.0f;
    blockOffsets[0].y = 0.0f;
    blockOffsets[1].x = -1.0f;
    blockOffsets[1].y = 0.0f;
    blockOffsets[2].x = -1.0f;
    blockOffsets[2].y = -1.0f;
    blockOffsets[3].x = 0.0f;
    blockOffsets[3].y = -1.0f;

    Point3F vertNormalsOut[4];
    vertNormalsOut[0].x = 0.0f;
    vertNormalsOut[0].y = 0.0f;
    vertNormalsOut[0].z = 0.0f;
    vertNormalsOut[1].x = 0.0f;
    vertNormalsOut[1].y = 0.0f;
    vertNormalsOut[1].z = 0.0f;
    vertNormalsOut[2].x = 0.0f;
    vertNormalsOut[2].y = 0.0f;
    vertNormalsOut[2].z = 0.0f;
    vertNormalsOut[3].x = 0.0f;
    vertNormalsOut[3].y = 0.0f;
    vertNormalsOut[3].z = 0.0f;

    Point3F vertNormalsIn[4];

    S32 i = 4;
    do
    {
        i--;

        blockOffsets[i].x += (F32)bx;
        blockOffsets[i].y += (F32)by;
    } while (i != 0);


    Direction idxGetIn = DIRECTION_UP;
    do
    {
        Map3D_GetBlockDirectionNormal(map, (U32)(U64)blockOffsets[idxGetIn].x, (U32)(U64)blockOffsets[idxGetIn].y, idxGetIn, &vertNormalsIn[idxGetIn]);
        idxGetIn++;
    } while (idxGetIn < DIRECTION_COUNT);

    S32 idxOut = 4;
    do
    {

        idxOut--;
        S32 idxIn = 4;
        do
        {
            idxIn--;
            if (!Map3D_BlockPairHasTextureMatch(map, blockOffsets[idxOut].x, blockOffsets[idxOut].y, blockOffsets[idxIn].x, blockOffsets[idxIn].y))
            {
                vertNormalsOut[idxOut].x += vertNormalsIn[idxIn].x;
                vertNormalsOut[idxOut].y += vertNormalsIn[idxIn].y;
                vertNormalsOut[idxOut].z += vertNormalsIn[idxIn].z;
            }

        } while (idxIn != 0);

        F32 f = 1.0f / sqrtf(vertNormalsOut[idxOut].x * vertNormalsOut[idxOut].x + vertNormalsOut[idxOut].y * vertNormalsOut[idxOut].y + vertNormalsOut[idxOut].z * vertNormalsOut[idxOut].z);
        vertNormalsOut[idxOut].x *= f;
        vertNormalsOut[idxOut].y *= f;
        vertNormalsOut[idxOut].z *= f;

    } while (idxOut != 0);

    Direction idxSetOut = 0;
    do
    {
        Map3D_SetBlockDirectionNormal(map, (U32)(U64)blockOffsets[idxSetOut].x, (U32)(U64)blockOffsets[idxSetOut].y, idxSetOut, &vertNormalsOut[idxSetOut]);
        idxSetOut++;
    } while (idxSetOut < DIRECTION_COUNT);
}

void Map3D_UpdateTextureUVs(lpMap3D map, F32 elapsedGame)
{
    // TODO: Implement Map3D_UpdateTextureUVs
}

void Map3D_UpdateFadeInTransitions(lpMap3D map, F32 elapsedGame)
{
    // TODO: Implement Map3D_UpdateFadeInTransitions
}

void Map3D_AddTextureMapping(lpMap3D map, SurfaceTexture texA, SurfaceTexture texB)
{
    U32 texANum = map->texsNum[texA];

    // check if mapping has already been created. If so, end operation
    for (U32 i = 0; i < texANum; i++)
    {
        // mapping already exists at index?
        if (map->texsGrid[texA][i] == texB)
            return;
    }

    // mapping not found, add texB to end of texA[] list
    map->texsGrid[texA][texANum] = texB;
    map->texsNum[texA]++;

    // now create the reverse mapping, add texA to end of texB[] list
    map->texsGrid[texB][map->texsNum[texB]] = texA;
    map->texsNum[texB]++;
}

// Initialisation function only.
// States a textureNo. cannot use fade transitions.
void Map3D_SetTextureNoFade(lpMap3D map, SurfaceTexture texture)
{
    map->texsNoFade[texture] = TRUE;
}

void Map3D_SetBlockTexture(lpMap3D map, U32 bx, U32 by, SurfaceTexture newTexture, Direction direction)
{
    if (bx < map->blockWidth && by < map->blockHeight)
    {
        U32 groupID = by * map->blockWidth + bx;
        Direction DIRECTIONS[4] = { DIRECTION_RIGHT, DIRECTION_LEFT, DIRECTION_DOWN, DIRECTION_UP };

        lpContainer_Texture texture = Detail_GetTexture(map->textureSet, newTexture);
        map->blocks3D[by * map->gridWidth + bx].texture = newTexture;
        if ((map->blocks3D[by * map->gridWidth + bx].flags3D & MAP3DBLOCK_FLAG_ROTATED) != MAP3DBLOCK_FLAG_NONE)
            direction++;
        Container_Mesh_SetTexture(map->mesh, groupID, texture);

        if ((map->blocks3D[by * map->gridWidth + bx].flags3D & MAP3DBLOCK_FLAG_UVWOBBLES) == MAP3DBLOCK_FLAG_NONE)
        {
            Vertex vertices[4];
            Container_Mesh_GetVertices(map->mesh, groupID, 0, 4, vertices);
            for (U32 i = 0; i < 4; i++)
            {
                Direction dir = DIRECTIONS[i - direction & 3];
                vertices[i].tu = (F32)(U32)(dir >> 1); // dir / 2
                vertices[i].tv = (F32)(U32)(dir & 1); // dir % 2 == 1
            }
            Container_Mesh_SetVertices(map->mesh, groupID, 0, 4, vertices);
        }
    }
}

void Map3D_SetBlockFadeInTexture(lpMap3D map, U32 bx, U32 by, SurfaceTexture newTexture, Direction direction)
{
    // TODO: Implement Map3D_SetBlockFadeInTexture

    Map3D_SetBlockTexture(map, bx, by, newTexture, direction);
}

void Map3D_ClearBlockHighlight(lpMap3D map, S32 bx, S32 by)
{
    // TODO: Implement Map3D_ClearBlockHighlight
}

F32 Map3D_BlockSize(lpMap3D map)
{
    return map->blockSize;
}

void Map3D_HideBlock(lpMap3D map, U32 bx, U32 by, BOOL hide)
{
    Container_Mesh_HideGroup(map->mesh, by * map->blockWidth + bx, hide);
}

void Map3D_HideVisibleBlocksList(lpMap3D map)
{
    for (U32 i = 0; i < map->visibleBlocksNum; i++)
    {
        Map3D_HideBlock(map, map->visibleBlocksTable[i].x, map->visibleBlocksTable[i].y, TRUE);
    }
    map->visibleBlocksNum = 0;
}

void Map3D_AddVisibleBlocksInRadius_AndDoCallbacks(lpMap3D map, S32 bx, S32 by, S32 radius, XYCallback callback)
{
    if (-radius > radius)
        return;

    S32 abs_y = -radius + by;
    S32 rel_y = -radius;
    do
    {
        if (-radius <= radius)
        {
            S32 abs_x = -radius + bx;
            S32 rel_x = -radius;
            do
            {
                if ((((rel_x * rel_x + rel_y * rel_y <= radius * radius) && -1 < abs_x) && (abs_x < (S32)map->blockWidth)) && ((-1 < abs_y && (abs_y < (S32)map->blockHeight))))
                {
                    Map3D_HideBlock(map, abs_x, abs_y, FALSE);
                    if (callback != NULL)
                        callback(abs_x, abs_y);

                    map->visibleBlocksTable[map->visibleBlocksNum].x = (S16)rel_x + (S16)bx;
                    map->visibleBlocksTable[map->visibleBlocksNum].y = (S16)rel_y + (S16)by;
                    map->visibleBlocksNum++;
                }
                rel_x++;
                abs_x++;
            } while (rel_x <= radius);
        }

        rel_y++;
        abs_y++;
    } while (rel_y <= radius);
}
