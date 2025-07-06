#pragma once
#include "afxdialogex.h"
#include <opencv2/opencv.hpp>

class CCameraDlg : public CDialogEx
{
public:
    CCameraDlg(CWnd* pParent = nullptr);
    enum { IDD = IDD_CAMERA_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedStartCamera();
    afx_msg void OnBnClickedStopCamera();
    afx_msg void OnBnClickedStartFaceDetection();
    afx_msg void OnBnClickedStopFaceDetection();
    afx_msg void OnCbnSelchangeFilterCombo();
    DECLARE_MESSAGE_MAP()

private:
    enum FilterType {
        NoFilter,
        Grayscale,
        Sepia,
        Negative,
        Blur,
        Sharpen
    };

    cv::Mat applyFilter(const cv::Mat& frame);
    void updateFrame();

    CButton m_btnStartCamera;
    CButton m_btnStopCamera;
    CButton m_btnStartFaceDetection;
    CButton m_btnStopFaceDetection;
    CComboBox m_cmbFilter;
    CStatic m_staticImage;
    CImage m_displayImage;

    cv::VideoCapture m_camera;
    cv::CascadeClassifier m_faceDetector;
    bool m_isCameraRunning;
    bool m_isFaceDetectionActive;
    FilterType m_currentFilter;
    UINT_PTR m_timerId;
}; 