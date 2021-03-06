/**
 * @file solvePnPRansac.cpp
 * @brief mex interface for solvePnPRansac
 * @author Kota Yamaguchi
 * @date 2011
 */
#include "mexopencv.hpp"
using namespace std;
using namespace cv;

/// Conversion to vector<Point_<T> >
template <typename T>
vector<Point_<T> > MxArrayToVecPt(MxArray& arr)
{
    vector<MxArray> va = arr.toVector<MxArray>();
    vector<Point_<T> > vp;
    vp.reserve(va.size());
    for (vector<MxArray>::iterator it=va.begin(); it<va.end(); ++it)
        vp.push_back((*it).toPoint_<T>());
    return vp;
}

/// Conversion to vector<Point_<T> >
template <typename T>
vector<Point3_<T> > MxArrayToVecPt3(MxArray& arr)
{
    vector<MxArray> va = arr.toVector<MxArray>();
    vector<Point3_<T> > vp;
    vp.reserve(va.size());
    for (vector<MxArray>::iterator it=va.begin(); it<va.end(); ++it)
        vp.push_back((*it).toPoint3_<T>());
    return vp;
}


/**
 * Main entry called from Matlab
 * @param nlhs number of left-hand-side arguments
 * @param plhs pointers to mxArrays in the left-hand-side
 * @param nrhs number of right-hand-side arguments
 * @param prhs pointers to mxArrays in the right-hand-side
 */
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
    // Check the number of arguments
    if (nrhs<3 || ((nrhs%2)!=0) || nlhs>2)
        mexErrMsgIdAndTxt("mexopencv:error","Wrong number of arguments");
    
    // Argument vector
    vector<MxArray> rhs(prhs,prhs+nrhs);
    
    Mat cameraMatrix(rhs[2].toMat(CV_32F));
    Mat distCoeffs((nrhs>3) ? rhs[3].toMat(CV_32F) : Mat());
    
    // Option processing
    bool useExtrinsicGuess=false;
    int iterationsCount=100;
    float reprojectionError=8.0;
    int minInliersCount=100;
    Mat rvec, tvec;
    for (int i=4; i<nrhs; i+=2) {
        string key = rhs[i].toString();
        if (key=="UseExtrinsicGuess")
            useExtrinsicGuess = rhs[i+1].toBool();
        else if (key=="IterationsCount")
            iterationsCount = rhs[i+1].toInt();
        else if (key=="ReprojectionError")
            reprojectionError = rhs[i+1].toDouble();
        else if (key=="MinInliersCount")
            minInliersCount = rhs[i+1].toInt();
        else if (key=="Rvec")
            rvec = rhs[i+1].toMat(CV_32F);
        else if (key=="Tvec")
            tvec = rhs[i+1].toMat(CV_32F);
        else
            mexErrMsgIdAndTxt("mexopencv:error","Unrecognized option");
    }
    
    // Process
    Mat inliers;
    if (rhs[0].isNumeric() && rhs[1].isNumeric()) {
        Mat objectPoints(rhs[0].toMat(CV_32F)), imagePoints(rhs[1].toMat(CV_32F));
        solvePnPRansac(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec,
            tvec, useExtrinsicGuess, iterationsCount, reprojectionError,
            minInliersCount, inliers);
    }
    else if (rhs[0].isCell() && rhs[1].isCell()) {
        vector<Point3f> objectPoints(rhs[0].toVector<Point3f>());
        vector<Point2f> imagePoints(rhs[1].toVector<Point2f>());
        solvePnPRansac(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec,
            tvec, useExtrinsicGuess, iterationsCount, reprojectionError,
            minInliersCount, inliers);
    }
    else
        mexErrMsgIdAndTxt("mexopencv:error","Invalid argument");

    plhs[0] = MxArray(rvec);
    if (nlhs>1)
        plhs[1] = MxArray(tvec);
    if (nlhs>2)
        plhs[2] = MxArray(inliers);
}
