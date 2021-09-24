#include "qrenderer.h"
#include <QFileDialog>
#include <QTextDocument>

QRenderer::QRenderer()
{
    m_oPrinter = new QPrinter(QPrinter::PrinterResolution);
    m_oPainter = new QPainter();
    m_oPrinter->setOutputFormat(QPrinter::PdfFormat);
    m_oPrinter->setPaperSize(QPrinter::A4);
}

QRenderer::~QRenderer()
{
    if(m_oPainter && m_oPainter->isActive())
        m_oPainter->end();
    delete m_oPainter;
    delete m_oPrinter;
}



HRESULT QRenderer::get_Type(LONG *lType)
{/*TODO*/ return S_OK; }

//-------- Функции для работы со страницей --------------------------------------------------
HRESULT QRenderer::NewPage()
{
    m_oPrinter->newPage();
    return S_OK;
}

HRESULT QRenderer::get_Height(double *dHeight)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_Height(const double &dHeight)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_Width(double *dWidth)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_Width(const double &dWidth)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_DpiX(double *dDpiX)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_DpiY(double *dDpiY)
{/*TODO*/ return S_OK; }

// pen --------------------------------------------------------------------------------------
HRESULT QRenderer::get_PenColor(LONG *lColor)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenColor(const LONG &lColor)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenAlpha(LONG *lAlpha)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenAlpha(const LONG &lAlpha)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenSize(double *dSize)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenSize(const double &dSize)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenDashStyle(BYTE *val)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenDashStyle(const BYTE &val)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenLineStartCap(BYTE *val)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenLineStartCap(const BYTE &val)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenLineEndCap(BYTE *val)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenLineEndCap(const BYTE &val)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenLineJoin(BYTE *val)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenLineJoin(const BYTE &val)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenDashOffset(double *dOffset)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenDashOffset(const double &dOffset)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenAlign(LONG *lAlign)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenAlign(const LONG &lAlign)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_PenMiterLimit(double *dOffset)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_PenMiterLimit(const double &dOffset)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::PenDashPattern(double *pPattern, LONG lCount)
{/*TODO*/ return S_OK; }

// brush ------------------------------------------------------------------------------------
HRESULT QRenderer::get_BrushType(LONG *lType)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushType(const LONG &lType)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_BrushColor1(LONG *lColor)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushColor1(const LONG &lColor)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_BrushAlpha1(LONG *lAlpha)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushAlpha1(const LONG &lAlpha)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_BrushColor2(LONG *lColor)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushColor2(const LONG &lColor)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_BrushAlpha2(LONG *lAlpha)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushAlpha2(const LONG &lAlpha)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_BrushTexturePath(std::wstring *bsPath)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushTexturePath(const std::wstring &bsPath)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_BrushTextureMode(LONG *lMode)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushTextureMode(const LONG &lMode)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_BrushTextureAlpha(LONG *lTxAlpha)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushTextureAlpha(const LONG &lTxAlpha)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_BrushLinearAngle(double *dAngle)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushLinearAngle(const double &dAngle)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::BrushRect(const INT &val, const double &left, const double &top, const double &width, const double &height)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::BrushBounds(const double &left, const double &top, const double &width, const double &height)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_BrushGradientColors(LONG *lColors, double *pPositions, LONG nCount)
{/*TODO*/ return S_OK; }

// font -------------------------------------------------------------------------------------
HRESULT QRenderer::get_FontName(std::wstring *bsName)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_FontName(const std::wstring &bsName)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_FontPath(std::wstring *bsName)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_FontPath(const std::wstring &bsName)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_FontSize(double *dSize)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_FontSize(const double &dSize)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_FontStyle(LONG *lStyle)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_FontStyle(const LONG &lStyle)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_FontStringGID(INT *bGID)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_FontStringGID(const INT &bGID)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_FontCharSpace(double *dSpace)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_FontCharSpace(const double &dSpace)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::get_FontFaceIndex(int *lFaceIndex)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_FontFaceIndex(const int &lFaceIndex)
{/*TODO*/ return S_OK; }

//-------- Функции для вывода текста --------------------------------------------------------
HRESULT
QRenderer::CommandDrawTextCHAR(const LONG &c, const double &x, const double &y, const double &w, const double &h)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::CommandDrawText(const std::wstring &bsText, const double &x, const double &y, const double &w, const double &h)
{
    m_oPainter->drawText(x, y, QString::fromStdWString(bsText));
    return S_OK;
}

HRESULT
QRenderer::CommandDrawTextExCHAR(const LONG &c, const LONG &gid, const double &x, const double &y, const double &w,
                      const double &h)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::CommandDrawTextEx(const std::wstring &bsUnicodeText, const unsigned int *pGids, const unsigned int nGidsCount,
                  const double &x, const double &y, const double &w, const double &h)
{/*TODO*/ return S_OK; }

//-------- Маркеры для команд ---------------------------------------------------------------
HRESULT QRenderer::BeginCommand(const DWORD &lType)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::EndCommand(const DWORD &lType)
{/*TODO*/ return S_OK; }

//-------- Функции для работы с Graphics Path -----------------------------------------------
HRESULT QRenderer::PathCommandMoveTo(const double &x, const double &y)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::PathCommandLineTo(const double &x, const double &y)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::PathCommandLinesTo(double *points, const int &count)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::PathCommandCurveTo(const double &x1, const double &y1, const double &x2, const double &y2, const double &x3,
                   const double &y3)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::PathCommandCurvesTo(double *points, const int &count)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::PathCommandArcTo(const double &x, const double &y, const double &w, const double &h, const double &startAngle,
                 const double &sweepAngle)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::PathCommandClose()
{/*TODO*/ return S_OK; }

HRESULT QRenderer::PathCommandEnd()
{/*TODO*/ return S_OK; }

HRESULT QRenderer::DrawPath(const LONG &nType)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::PathCommandStart()
{/*TODO*/ return S_OK; }

HRESULT QRenderer::PathCommandGetCurrentPoint(double *x, double *y)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::PathCommandTextCHAR(const LONG &c, const double &x, const double &y, const double &w, const double &h)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::PathCommandText(const std::wstring &bsText, const double &x, const double &y, const double &w, const double &h)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::PathCommandTextExCHAR(const LONG &c, const LONG &gid, const double &x, const double &y, const double &w,
                      const double &h)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::PathCommandTextEx(const std::wstring &sText, const unsigned int *pGids, const unsigned int nGidsCount,
                  const double &x, const double &y, const double &w, const double &h)
{/*TODO*/ return S_OK; }

//-------- Функции для вывода изображений ---------------------------------------------------
HRESULT QRenderer::DrawImage(IGrObject *pImage, const double &x, const double &y, const double &w, const double &h)
{/*TODO*/ return S_OK; }

HRESULT
QRenderer::DrawImageFromFile(const std::wstring &, const double &x, const double &y, const double &w, const double &h,
                  const BYTE &lAlpha)
{/*TODO*/ return S_OK; }

// transform --------------------------------------------------------------------------------
HRESULT
QRenderer::SetTransform(const double &m1, const double &m2, const double &m3, const double &m4, const double &m5,
             const double &m6)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::GetTransform(double *pdA, double *pdB, double *pdC, double *pdD, double *pdE, double *pdF)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::ResetTransform()
{/*TODO*/ return S_OK; }

// -----------------------------------------------------------------------------------------
HRESULT QRenderer::get_ClipMode(LONG *plMode)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::put_ClipMode(const LONG &lMode)
{/*TODO*/ return S_OK; }

// additiaonal params ----------------------------------------------------------------------
HRESULT QRenderer::CommandLong(const LONG &lType, const LONG &lCommand)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::CommandDouble(const LONG &lType, const double &dCommand)
{/*TODO*/ return S_OK; }

HRESULT QRenderer::CommandString(const LONG &lType, const std::wstring &sCommand)
{/*TODO*/ return S_OK; }
