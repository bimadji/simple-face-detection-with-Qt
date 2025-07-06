#include "pch.h"
#include "framework.h"
#include "CameraDlg.h"
#include "afxdialogex.h"

BEGIN_MESSAGE_MAP(CCameraDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_START_CAMERA, &CCameraDlg::OnBnClickedStartCamera)
    ON_BN_CLICKED(IDC_BTN_STOP_CAMERA, &CCameraDlg::OnBnClickedStopCamera)
    ON_BN_CLICKED(IDC_BTN_START_FACE_DETECTION, &CCameraDlg::OnBnClickedStartFaceDetection)
    ON_BN_CLICKED(IDC_BTN_STOP_FACE_DETECTION, &CCameraDlg::OnBnClickedStopFaceDetection)
    ON_CBN_SELCHANGE(IDC_CMB_FILTER, &CCameraDlg::OnCbnSelchangeFilterCombo)
    ON_WM_TIMER()
END_MESSAGE_MAP()

CCameraDlg::CCameraDlg(CWnd* pParent)
    : CDialogEx(IDD_CAMERA_DIALOG, pParent)
    , m_isCameraRunning(false)
    , m_isFaceDetectionActive(false)
    , m_currentFilter(NoFilter)
    , m_timerId(0)
{
}

void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_START_CAMERA, m_btnStartCamera);
    DDX_Control(pDX, IDC_BTN_STOP_CAMERA, m_btnStopCamera);
    DDX_Control(pDX, IDC_BTN_START_FACE_DETECTION, m_btnStartFaceDetection);
    DDX_Control(pDX, IDC_BTN_STOP_FACE_DETECTION, m_btnStopFaceDetection);
    DDX_Control(pDX, IDC_CMB_FILTER, m_cmbFilter);
    DDX_Control(pDX, IDC_STATIC_IMAGE, m_staticImage);
}

BOOL CCameraDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Initialize filter combo box
    m_cmbFilter.AddString(_T("No Filter"));
    m_cmbFilter.AddString(_T("Grayscale"));
    m_cmbFilter.AddString(_T("Sepia"));
    m_cmbFilter.AddString(_T("Negative"));
    m_cmbFilter.AddString(_T("Blur"));
    m_cmbFilter.AddString(_T("Sharpen"));
    m_cmbFilter.SetCurSel(0);

    // Load face detection classifier
    if (!m_faceDetector.load("haarcascade_frontalface_default.xml")) {
        MessageBox(_T("Failed to load face detection classifier"), _T("Error"), MB_ICONERROR);
    }

    // Set initial button states
    m_btnStopCamera.EnableWindow(FALSE);
    m_btnStartFaceDetection.EnableWindow(FALSE);
    m_btnStopFaceDetection.EnableWindow(FALSE);
    m_cmbFilter.EnableWindow(FALSE);

    return TRUE;
}

void CCameraDlg::OnBnClickedStartCamera()
{
    if (!m_isCameraRunning) {
        m_camera.open(0);
        
        if (m_camera.isOpened()) {
            m_btnStartCamera.EnableWindow(FALSE);
            m_btnStopCamera.EnableWindow(TRUE);
            m_btnStartFaceDetection.EnableWindow(TRUE);
            m_cmbFilter.EnableWindow(TRUE);
            m_isCameraRunning = true;
            
            // Start timer for frame updates (30 FPS)
            m_timerId = SetTimer(1, 33, NULL);
        } else {
            MessageBox(_T("Could not open camera"), _T("Error"), MB_ICONERROR);
        }
    }
}

void CCameraDlg::OnBnClickedStopCamera()
{
    if (m_isCameraRunning) {
        KillTimer(m_timerId);
        m_camera.release();
        
        m_btnStartCamera.EnableWindow(TRUE);
        m_btnStopCamera.EnableWindow(FALSE);
        m_btnStartFaceDetection.EnableWindow(FALSE);
        m_btnStopFaceDetection.EnableWindow(FALSE);
        m_cmbFilter.EnableWindow(FALSE);
        m_isCameraRunning = false;
        m_isFaceDetectionActive = false;
        
        // Clear the image display
        m_staticImage.SetWindowText(_T("Camera Off"));
    }
}

void CCameraDlg::OnBnClickedStartFaceDetection()
{
    if (m_isCameraRunning && !m_isFaceDetectionActive) {
        m_isFaceDetectionActive = true;
        m_btnStartFaceDetection.EnableWindow(FALSE);
        m_btnStopFaceDetection.EnableWindow(TRUE);
    }
}

void CCameraDlg::OnBnClickedStopFaceDetection()
{
    if (m_isFaceDetectionActive) {
        m_isFaceDetectionActive = false;
        m_btnStartFaceDetection.EnableWindow(TRUE);
        m_btnStopFaceDetection.EnableWindow(FALSE);
    }
}

void CCameraDlg::OnCbnSelchangeFilterCombo()
{
    m_currentFilter = static_cast<FilterType>(m_cmbFilter.GetCurSel());
}

void CCameraDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == m_timerId) {
        updateFrame();
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CCameraDlg::updateFrame()
{
    cv::Mat frame;
    if (m_camera.read(frame)) {
        cv::Mat displayFrame = frame.clone();
        
        // Apply selected filter
        displayFrame = applyFilter(displayFrame);
        
        // If face detection is active, detect and draw faces
        if (m_isFaceDetectionActive) {
            std::vector<cv::Rect> faces;
            cv::Mat grayFrame;
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            
            m_faceDetector.detectMultiScale(grayFrame, faces, 1.3, 5);
            
            for (size_t i = 0; i < faces.size(); i++) {
                cv::rectangle(displayFrame, faces[i].tl(), faces[i].br(), cv::Scalar(50, 50, 255), 3);
                cv::rectangle(displayFrame, cv::Point(0, 0), cv::Point(250, 70), cv::Scalar(50, 50, 255), cv::FILLED);
                cv::putText(displayFrame, std::to_string(faces.size()) + " Monkey Found!!", cv::Point(300, 600), 
                           cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 255, 255), 1);
            }
        }
        
        // Convert OpenCV Mat to MFC CImage
        cv::cvtColor(displayFrame, displayFrame, cv::COLOR_BGR2RGB);
        m_displayImage.Destroy();
        m_displayImage.Create(displayFrame.cols, displayFrame.rows, 24);
        
        // Copy the image data
        memcpy(m_displayImage.GetBits(), displayFrame.data, displayFrame.total() * displayFrame.elemSize());
        
        // Update the static control
        CDC* pDC = m_staticImage.GetDC();
        m_displayImage.Draw(pDC->GetSafeHdc(), 0, 0);
        m_staticImage.ReleaseDC(pDC);
    }
}

cv::Mat CCameraDlg::applyFilter(const cv::Mat& frame)
{
    cv::Mat result;
    
    switch (m_currentFilter) {
        case NoFilter:
            return frame.clone();
            
        case Grayscale:
            cv::cvtColor(frame, result, cv::COLOR_BGR2GRAY);
            cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
            return result;
            
        case Sepia:
            result = frame.clone();
            cv::transform(frame, result, cv::Matx33f(
                0.272, 0.534, 0.131,
                0.349, 0.686, 0.168,
                0.393, 0.769, 0.189
            ));
            return result;
            
        case Negative:
            result = frame.clone();
            cv::bitwise_not(frame, result);
            return result;
            
        case Blur:
            cv::GaussianBlur(frame, result, cv::Size(15, 15), 0);
            return result;
            
        case Sharpen:
            {
                cv::Mat kernel = (cv::Mat_<float>(3, 3) << 
                    0, -1, 0,
                    -1, 5, -1,
                    0, -1, 0);
                cv::filter2D(frame, result, -1, kernel);
                return result;
            }
            
        default:
            return frame.clone();
    }
} 