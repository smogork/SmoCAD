//
// Created by ksm on 6/14/22.
//

#ifndef SMOCAD_POLYNOMIALS_H
#define SMOCAD_POLYNOMIALS_H

#include <QVector3D>

class Polynomials
{
public:
    template <typename T>
    static T deCasteljau(float t, T *points, int size)
    {
        T* data = new T[size];
    
        for (int i = 0; i < size; ++i)
            data[i] = points[i];
    
        for (int h = 1; h < size ; ++h)
            for (int i = 0; i < size - h ;  i++)
                data[i] = (1.0f - t) * data[i] + t * data[i + 1];
    
        T res = data[0];
        delete [] data;
    
        return res;
    }
    
    template <typename T>
    static T deCasteljau(float t, std::vector<T>& points)
    {
        return deCasteljau(t, points.data(), (int)points.size());
    }
    
    template <typename T>
    static void deCasteljauSteps(float t, T *points, T** steps, int size)
    {
        if (steps == nullptr)
            throw std::runtime_error("steps matrix must be passed to function");
    
        for (int i = 0; i < size; ++i)
            steps[0][i] = points[i];
    
        for (int h = 1; h < size ; ++h)
            for (int i = 0; i < size - h ;  i++)
                steps[h][i] = (1.0f - t) * steps[h - 1][i] + t * steps[h - 1][i + 1];
    }
    
    template <typename T>
    static std::pair<std::vector<T>, std::vector<T>> deCasteljauDouble(float t, std::vector<T>& points)
    {
        return deCasteljauDouble(t, points.data(), points.size());
    }
    
    template <typename T>
    static std::pair<std::vector<T>, std::vector<T>> deCasteljauDouble(float t, T *points, int size)
    {
        T** steps = new T*[size];
        for (size_t i = 0; i < size; ++i) steps[i] = new T[size];
    
        deCasteljauSteps(t, points, steps, size);
    
        std::vector<T> res1, res2;
    
        for (int i = 0; i < size; ++i)
        {
            res1.push_back(steps[i][0]);
            res2.push_back(steps[size - 1 - i][i]);
        }
    
        for (size_t i = 0; i < size; ++i) delete [] steps[i];
        delete [] steps;
    
        return std::make_pair(res1, res2);
    }
    
    template <typename T>
    static std::vector<T> deCasteljauDerK(int k, std::vector<T>& points)
    {
        return deCasteljauDerK(k, points.data(), points.size());
    }
    
    template <typename T>
    static std::vector<T> deCasteljauDerK(int k, T* points, int size)
    {
        T* data = new T[size];
    
        for (int i = 0; i < size; ++i)
            data[i] = points[i];
    
        for (int h = 1; h <= k ; ++h)
            for (int i = 0; i < size - h ;  i++)
                data[i] = -3.0f * data[i] +  3.0f * data[i + 1];
        
        std::vector<T> res(size - k);
        for (int i = 0; i < size - k; ++i)
            res[i] = data[i];
        
        return res;
    }
    
    template <typename T>
    static void LoadControlPointsRow(int rowId, std::vector<T>& inArray, std::vector<T>& outArray)
    {
        LoadControlPointsRow(rowId, inArray.data(), outArray.data(), outArray.size());
    }
    
    template <typename T>
    static void LoadControlPointsRow(int rowId, T* inArray, T* outArray, int size)
    {
        for (int i = 0; i < size; ++i)
            outArray[i] = inArray[rowId * size + i];
    }
    
    template <typename T>
    static void LoadControlPointsCol(int colId, std::vector<T>& inArray, std::vector<T>& outArray)
    {
        LoadControlPointsCol(colId, inArray.data(), outArray.data(), outArray.size());
    }
    
    template <typename T>
    static void LoadControlPointsCol(int colId, T* inArray, T* outArray, int size)
    {
        for (int i = 0; i < size; ++i)
            outArray[i] = inArray[i * size + colId];
    }

};

#endif //SMOCAD_POLYNOMIALS_H
