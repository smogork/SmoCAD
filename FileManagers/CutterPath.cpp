//
// Created by ksm on 10/20/22.
//

#include "CutterPath.h"

CutterPath::CutterPath(CutterParameters params): Cutter(params)
{ }

CutterPath::CutterPath(CutterPath &&other) noexcept : Cutter(other.Cutter), Points(std::move(other.Points))
{ }

CutterPath &CutterPath::operator=(CutterPath &&right) noexcept
{
    Cutter = right.Cutter;
    Points = std::move(right.Points);
    return *this;
}

