/*
 * (c) Copyright Ascensio System SIA 2010-2019
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at 20A-12 Ernesta Birznieka-Upisha
 * street, Riga, Latvia, EU, LV-1050.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */
#pragma once

#include "./MetafileToRendererCheck.h"
#include "agg_math.h"
#include "../fontengine/FontManager.h"

// tmp
#include <fstream>

#if !defined(_WIN32) && !defined(_WIN64)
#include "../common/StringExt.h"
#endif

// этот класс нужно переписать. должно работать как и в js
// а не просто на каждом символе переключаться, если нужно
class CMetafileFontPicker
{
private:
    NSFonts::IApplicationFonts* m_pApplication;
    NSFonts::IFontManager* m_pManager;
    IRenderer* m_pRenderer;

public:
    CMetafileFontPicker(std::wstring sFolder)
    {
        m_pApplication = NSFonts::NSApplication::Create();
        m_pApplication->InitializeFromFolder(sFolder);
        m_pManager = m_pApplication->GenerateFontManager();

        NSFonts::IFontsCache* pCache = NSFonts::NSFontCache::Create();
        pCache->SetStreams(m_pApplication->GetStreams());
        m_pManager->SetOwnerCache(pCache);

        m_pRenderer = NULL;
    }
    CMetafileFontPicker(NSFonts::IApplicationFonts* pFonts)
    {
        m_pApplication = pFonts;
        m_pApplication->AddRef();
        m_pManager = m_pApplication->GenerateFontManager();

        NSFonts::IFontsCache* pCache = NSFonts::NSFontCache::Create();
        pCache->SetStreams(m_pApplication->GetStreams());
        m_pManager->SetOwnerCache(pCache);

        m_pRenderer = NULL;
    }
    ~CMetafileFontPicker()
    {
        m_pManager->Release();
        m_pApplication->Release();
    }

    void SetFont(const std::wstring& sName, const bool& bBold, const bool& bItalic)
    {
        int nStyle = 0;
        if (bBold) nStyle |= 0x01;
        if (bItalic) nStyle |= 0x02;
        SetFont(sName, nStyle);
    }
    void SetFont(const std::wstring& sName, const int& nStyle)
    {
        m_pManager->LoadFontByName(sName, 10, nStyle, 72, 72);
    }

    void FillText(const std::wstring& bsText, const double& x, const double& y, const double& w, const double& h)
    {
        int bGID = 0;
        m_pRenderer->get_FontStringGID(&bGID);

        CGlyphString oString(bsText, 0, 0);
        if (bGID || oString.GetLength() != 1)
        {
            m_pRenderer->CommandDrawText(bsText, x, y, w, h);
            return;
        }

        std::wstring sName;
        LONG nStyle = 0;
        m_pRenderer->get_FontName(&sName);
        m_pRenderer->get_FontStyle(&nStyle);
        SetFont(sName, (int)nStyle);

        NSFonts::IFontFile* pFont = m_pManager->GetFile();
        if (!pFont)
        {
            m_pRenderer->CommandDrawText(bsText, x, y, w, h);
            return;
        }

        int code = oString.GetAt(0)->lUnicode;
        if (pFont->GetGIDByUnicode(code))
        {
            m_pRenderer->CommandDrawText(bsText, x, y, w, h);
            return;
        }

        CFontFile* pFileNew = ((CFontManager*)m_pManager)->GetFontFileBySymbol((CFontFile*)m_pManager->GetFile(), code);
        if (!pFileNew)
        {
            m_pRenderer->CommandDrawText(bsText, x, y, w, h);
            return;
        }

        m_pRenderer->put_FontName(pFileNew->m_sName);
        int nNewStyle = 0;
        if (pFileNew->IsBold()) nNewStyle |= 0x01;
        if (pFileNew->IsItalic()) nNewStyle |= 0x02;
        m_pRenderer->put_FontStyle(nNewStyle);
        m_pRenderer->CommandDrawText(bsText, x, y, w, h);
        m_pRenderer->put_FontName(sName);
        m_pRenderer->put_FontStyle(nStyle);
    }

    void SetRenderer(IRenderer* pRenderer)
    {
        m_pRenderer = pRenderer;
    }
};

IMetafileToRenderter::IMetafileToRenderter(IRenderer* pRenderer)
{
    m_pRenderer = pRenderer;
    m_pPicker = NULL;
}
IMetafileToRenderter::~IMetafileToRenderter()
{
    if (!m_pPicker)
        return;

    CMetafileFontPicker* pPicker = (CMetafileFontPicker*)m_pPicker;
    RELEASEOBJECT(pPicker);
    m_pPicker = NULL;
}
void IMetafileToRenderter::EnableBrushRect(bool bValue)
{
}
void IMetafileToRenderter::SetLinearGradiant(const double& x0, const double& y0, const double& x1, const double& y1)
{
    double dAngle = 0;
    if (fabs(x1 - x0) >= FLT_EPSILON || fabs(y1 - y0) >= FLT_EPSILON)
        dAngle = 180 * atan2(y1 - y0, x1 - x0) / agg::pi;

    m_pRenderer->put_BrushType(/*c_BrushTypePathGradient1*/2006);
    m_pRenderer->put_BrushLinearAngle(dAngle);
}

void IMetafileToRenderter::SetRadialGradiant(const double& dX0, const double& dY0, const double& dR0, const double& dX1, const double& dY1, const double& dR1)
{
    // TODO:
    m_pRenderer->put_BrushType(/*c_BrushTypePathGradient2*/2007);
}

void IMetafileToRenderter::InitPicker(const std::wstring& sFontsFolder)
{
    CMetafileFontPicker* pPicker = new CMetafileFontPicker(sFontsFolder);
    m_pPicker = (void*)pPicker;
}
void IMetafileToRenderter::InitPicker(NSFonts::IApplicationFonts* pFonts)
{
    CMetafileFontPicker* pPicker = new CMetafileFontPicker(pFonts);
    m_pPicker = (void*)pPicker;
}

namespace NSOnlineOfficeBinToPdf
{
    inline BYTE ReadByte(BYTE*& pData, int& nOffset, std::wfstream &file)
	{

		BYTE ret = *(pData);
        file << "read 1 byte: " << (int)ret << std::endl;
		pData++;
		nOffset++;
		return ret;
	}
    inline bool ReadBool(BYTE*& pData, int& nOffset, std::wfstream &file)
	{
        return ReadByte(pData, nOffset, file);
	}
    inline INT32 ReadInt(BYTE*& pData, int& nOffset, std::wfstream &file)
	{

	#ifdef _ARM_ALIGN_
		INT32 ret = 0;
		memcpy(&ret, pData, sizeof(INT32));
		pData   += 4;
		nOffset += 4;
		return ret;
	#else
		INT32 ret = *((INT32*)pData);
        file << "read 4 bytes: " << ret << std::endl;
		pData   += 4;
		nOffset += 4;
		return ret;
	#endif
	}
    inline double ReadDouble(BYTE*& pData, int& nOffset, std::wfstream &file)
	{
        return ReadInt(pData, nOffset, file) / 100000.0;
	}
    inline void SkipInt(BYTE*& pData, int& nOffset, std::wfstream &file, int nCount = 1)
	{
        file << "skip " << nCount << " ints, 4 bytes each" << std::endl;
		pData   += (nCount << 2);
		nOffset += (nCount << 2);
	}
    inline void SkipDouble(BYTE*& pData, int& nOffset, std::wfstream &file, int nCount = 1)
	{
        SkipInt(pData, nOffset, file, nCount);
	}

    inline USHORT ReadUSHORT(BYTE*& pData, int& nOffset, std::wfstream &file)
	{

	#ifdef _ARM_ALIGN_
		USHORT ret = 0;
		memcpy(&ret, pData, sizeof(USHORT));
		pData   += 2;
		nOffset += 2;
		return ret;
	#else
		USHORT ret = *((USHORT*)pData);
        file << "read 2 bytes: " << ret << std::endl;
		pData   += 2;
		nOffset += 2;
		return ret;
	#endif
	}
    inline void SkipUSHORT(BYTE*& pData, int& nOffset, std::wfstream &file)
	{
        file << "skip 2 bytes" << std::endl;
		pData   += 2;
		nOffset += 2;
	}

    inline std::wstring ReadString16(BYTE*& pData, int& nOffset, int nLen, std::wfstream &file)
	{
        file << "read string len = " << nLen << std::endl;
		std::wstring wsTempString;
	#ifdef _ARM_ALIGN_

		if (sizeof(wchar_t) == 4)
		{
	#if !defined(_WIN32) && !defined(_WIN64)
			int len = nLen / 2;
			unsigned short* buf = new unsigned short[len];
			memcpy(buf, pData, nLen);
			wsTempString = NSStringExt::CConverter::GetUnicodeFromUTF16((unsigned short*)buf, len);
			RELEASEARRAYOBJECTS(buf);
	#endif
		}
		else
		{
			int len = nLen / 2;
			wchar_t* buf = new wchar_t[len];
			memcpy(buf, pData, nLen);
			wsTempString = std::wstring(buf, len);
			RELEASEARRAYOBJECTS(buf);
		}

	#else

		if (sizeof(wchar_t) == 4)
		{
	#if !defined(_WIN32) && !defined(_WIN64)
			wsTempString = NSStringExt::CConverter::GetUnicodeFromUTF16((unsigned short*)pData, nLen / 2);
	#endif
		}
		else
		{
			wsTempString = std::wstring((wchar_t*)pData, nLen / 2);
		}

	#endif
		pData += nLen;
		nOffset += nLen;
		return wsTempString;
	}
    inline void SkipString16(BYTE*& pData, int& nOffset, int nLen, std::wfstream &file)
	{
        file << "skip string len = " << nLen << std::endl;
		pData += nLen;
		nOffset += nLen;
	}
    inline std::wstring ReadString(BYTE*& pData, int& nOffset, std::wfstream &file)
	{
        int nLen = 2 * ReadUSHORT(pData, nOffset, file);
        return ReadString16(pData, nOffset, nLen, file);
	}
    inline void SkipString(BYTE*& pData, int& nOffset, std::wfstream &file)
	{
        int nLen = 2 * ReadUSHORT(pData, nOffset, file);
        SkipString16(pData, nOffset, nLen, file);
	}

    bool ConvertBufferToRenderer(BYTE* pBuffer, LONG lBufferLen, IMetafileToRenderter* pCorrector)
	{
        std::cout << lBufferLen << std::endl;
		IRenderer* pRenderer = pCorrector->m_pRenderer;
        CMetafileFontPicker* pPicker = NULL;
        if (pCorrector->m_pPicker)
        {
            pPicker = (CMetafileFontPicker*)pCorrector->m_pPicker;
            pPicker->SetRenderer(pRenderer);
        }

		LONG lRendererType = 0;
		pRenderer->get_Type(&lRendererType);

        // tmp
        std::wfstream readable{
//#ifdef TMP_TEST_SAVE_READABLE_DATA
            "./../../../../../../desktop-sdk/ChromiumBasedEditors/lib/qt_wrapper/test_project/tests/source_readable"
//#endif
        };
        readable.clear();
        for (LONG i = 0; i < lBufferLen; ++i) {
            readable << (int)pBuffer[i] << ' ';
            if (i && i % 10 == 0) {
                readable << std::endl;
            }
        }

		CommandType eCommand = ctError;

		bool bIsPathOpened = false;
		int curindex = 0;

		BYTE* current = pBuffer;
		while (curindex < lBufferLen)
		{
			eCommand = (CommandType)(*current);
			current++;
			curindex++;
            readable << "current++" << std::endl;
			switch (eCommand)
			{
			case ctPageWidth:
			{
                INT32 val = ReadInt(current, curindex, readable) / 100000.0;
                readable << "ctPageWidth " << val << std::endl;
                pRenderer->put_Width(val);
				break;
			}
			case ctPageHeight:
			{
                INT32 val = ReadInt(current, curindex, readable) / 100000.0;
                readable << "ctPageHeight " << val << std::endl;
                pRenderer->put_Height(val);
				break;
			}
			case ctPageStart:
			{
                readable << "ctPageStart " << std::endl;
				pRenderer->NewPage();
				pRenderer->BeginCommand(c_nPageType);

				// TODO:
				pRenderer->put_PenLineStartCap(Aggplus::LineCapFlat);
				pRenderer->put_PenLineEndCap(Aggplus::LineCapFlat);
				break;
			}
			case ctPageEnd:
			{
                readable << "ctPageEnd " << std::endl;
				if (bIsPathOpened)
				{
					pRenderer->PathCommandEnd();
					pRenderer->EndCommand(c_nPathType);
				}
				bIsPathOpened = false;

				pRenderer->EndCommand(c_nPageType);

				if (lRendererType == c_nGrRenderer)
					return true;

				break;
			}
			case ctPenColor:
			{
                INT32 val = ReadInt(current, curindex, readable);
                readable << "ctPenColor " << val << std::endl;
                pRenderer->put_PenColor(val);
				break;
			}
			case ctPenAlpha:
			{
                readable << "ctPenAlpha " << int(*current) << std::endl;
				pRenderer->put_PenAlpha(*current);
				current++;
				curindex++;
                readable << "current++" << std::endl;
				break;
			}
			case ctPenSize:
			{
                INT32 val = ReadInt(current, curindex, readable) / 100000.0;
                readable << "ctPenSize " << val << std::endl;
                pRenderer->put_PenSize(val);
				break;
			}
			case ctPenDashStyle:
			{

				BYTE nDashType = *current++;
                readable << "current++" << std::endl;
                readable << "ctPenDashStyle " << (int)nDashType << ' ';
				curindex++;
				switch (nDashType)
				{
				case Aggplus::DashStyleCustom:
				{
                    int nCountDash = ReadInt(current, curindex, readable);
                    readable << "custom " << nCountDash << std::endl;
					if (0 < nCountDash)
					{
						double* pDash = new double[nCountDash];
						for (int nDash = 0; nDash < nCountDash; ++nDash)
						{
                            pDash[nDash] = ReadInt(current, curindex, readable) / 100000.0;
                            readable << pDash[nDash] << ' ';
						}

						if (c_nGrRenderer == lRendererType)
						{
							for (int nDash = 0; nDash < nCountDash; ++nDash)
							{
								// в отрисовщике - баг. зачем-то умножается на коеф 25.4/dpi
								// чтобы не менять там (перед выпуском) - умножаю здесь на обратку
								double dDpiX = 0;
								pRenderer->get_DpiX(&dDpiX);
								pDash[nDash] *= (dDpiX / 25.4);
							}
						}

                        pRenderer->PenDashPattern(pDash, nCountDash);
                        delete[] pDash;
					}
				}
				default:
                    pRenderer->put_PenDashStyle(nDashType);
                    break;
				}
                readable << std::endl;

				break;
			}
			case ctPenLineJoin:
			{
                readable << "ctPenLineJoin " << (int)*current << std::endl;
				pRenderer->put_PenLineJoin(*current);
				current++;
				curindex++;
                readable << "current++" << std::endl;
				break;
			}
			case ctBrushType:
			{
                INT32 val = ReadInt(current, curindex, readable);
                readable << "ctBrushType " << val << std::endl;
                pRenderer->put_BrushType(val);
				break;
			}
			case ctBrushColor1:
			{
                INT32 val = ReadInt(current, curindex, readable);
                readable << "ctBrushColor1 " << val << std::endl;
                pRenderer->put_BrushColor1(val);
				break;
			}
			case ctBrushAlpha1:
			{
                readable << "ctBrushAlpha1 " << (int)*current << std::endl;
				pRenderer->put_BrushAlpha1(*current);
				current++;
				curindex++;
                readable << "current++" << std::endl;
				break;
			}
			case ctBrushColor2:
			{
                INT32 val = ReadInt(current, curindex, readable);
                readable << "ctBrushColor2 " << val << std::endl;
                pRenderer->put_BrushColor1(val);
				break;
			}
			case ctBrushAlpha2:
			{
                readable << "ctBrushAlpha2 " << (int)*current << std::endl;
				pRenderer->put_BrushAlpha2(*current);
				current++;
				curindex++;
                readable << "current++" << std::endl;
				break;
			}
			case ctBrushRectable:
			{
                double m1 = ReadInt(current, curindex, readable) / 100000.0;
                double m2 = ReadInt(current, curindex, readable) / 100000.0;
                double m3 = ReadInt(current, curindex, readable) / 100000.0;
                double m4 = ReadInt(current, curindex, readable) / 100000.0;
                readable << "ctBrushRectable " << m1 << ' '
                         << m2 << ' ' << m3 << ' ' << m4 << std::endl;
				pRenderer->BrushRect(0, m1, m2, m3, m4);
				break;
			}
			case ctBrushRectableEnabled:
			{
				bool bEnable = (1 == *current) ? true : false;
                readable << "ctBrushRectableEnabled " << std::boolalpha << bEnable << std::endl;
				pCorrector->EnableBrushRect(bEnable);

				current += 1;
				curindex += 1;
				break;
			}
			case ctBrushTexturePath:
			{
                readable << "ctBrushTexturePath" << std::endl;
                readable << "current: " << (int)*current << ' ' << (int)current[1] << std::endl;
                int nLen = 2 * ReadUSHORT(current, curindex, readable);
                std::wstring sTempPath = ReadString16(current, curindex, nLen, readable);

                readable
                         << "nLen = " << nLen << std::endl

                        << "temp path: " << sTempPath << std::endl;
				std::wstring sImagePath = pCorrector->GetImagePath(sTempPath);
                readable << "image path: " << sImagePath << std::endl;
                readable << "image path size: " << sImagePath.size() << std::endl;
				pRenderer->put_BrushTexturePath(sImagePath);
				break;
			}
			case ctBrushGradient:
			{
                readable << "ctBrushGradient" << std::endl;
				current++;
				curindex++;
                readable << "current++" << std::endl;
                readable << "skipped 1" << std::endl;

				while (true)
				{
					BYTE _command = *current;
					current++;
					curindex++;
                    readable << "current++" << std::endl;

                    if (251 == _command){
                        readable << "command 251: break" << std::endl;
                        break;
                    }

					switch (_command)
					{
					case 0:
					{
                        readable << "command 0" << std::endl;
						current += 5;
						curindex += 5;
                        readable << "skipped 5" << std::endl;
                        double x0 = ReadInt(current, curindex, readable) / 100000.0;
                        double y0 = ReadInt(current, curindex, readable) / 100000.0;
                        double x1 = ReadInt(current, curindex, readable) / 100000.0;
                        double y1 = ReadInt(current, curindex, readable) / 100000.0;

                        readable << "linear gradient " << x0 << ' '
                                 << y0 << ' ' << x1 << ' ' << y1 << std::endl;

                        pCorrector->SetLinearGradiant(x0, y0, x1, y1);
                        break;
                    }
                    case 1:
                    {
                        readable << "command 1" << std::endl;
                        current++;
                        curindex++;
                        readable << "current++" << std::endl;
                        readable << "skipped 1" << std::endl;
                        double x0 = ReadInt(current, curindex, readable) / 100000.0;
                        double y0 = ReadInt(current, curindex, readable) / 100000.0;
                        double x1 = ReadInt(current, curindex, readable) / 100000.0;
                        double y1 = ReadInt(current, curindex, readable) / 100000.0;
                        double r0 = ReadInt(current, curindex, readable) / 100000.0;
                        double r1 = ReadInt(current, curindex, readable) / 100000.0;
                        readable << "radial gradient " << x0 << ' ' << y0 << ' '
                                 << x1 << ' ' << y1 <<  ' ' << r0 << ' ' << r1 << std::endl;
                        pCorrector->SetRadialGradiant(x0, y0, r0, x1, y1, r1);
                        break;
                    }
                    case 2:
                    {
                        readable << "command 2: BrushGradientColors" << std::endl;
                        LONG lColorsCount = (LONG)ReadInt(current, curindex, readable);
                        if (0 >= lColorsCount)
                        {
                            readable << "no gradient colors" << std::endl;
                            pRenderer->put_BrushGradientColors(NULL, NULL, 0);
                        }
                        else
                        {
                            LONG* pColors = new LONG[lColorsCount];
                            double* pPositions = new double[lColorsCount];

                            if (!pColors)
                                break;

                            if (!pPositions)
                            {
                                delete[] pColors;
                                break;
                            }

                            for (LONG lIndex = 0; lIndex < lColorsCount; lIndex++)
                            {
                                pPositions[lIndex] = ReadInt(current, curindex, readable) / 100000.0;
                                pColors[lIndex] = ReadInt(current, curindex, readable);
                                readable << pPositions[lIndex] << ' ' << pColors[lIndex] << std::endl;
                            }

                            pRenderer->put_BrushGradientColors(pColors, pPositions, lColorsCount);

                            delete[] pColors;
                            delete[] pPositions;
                        }

						break;
					}
					default:
					{
                        readable << "unknown command" << std::endl;
						break;
					}
					};
				}

				break;
			}
			case ctBrushTextureMode:
			{
				LONG lMode = (LONG)(*current);
                readable << "ctBrushTextureMode " << lMode << std::endl;
				pRenderer->put_BrushTextureMode(lMode);

				current += 1;
				curindex += 1;
				break;
			}
			case ctBrushTextureAlpha:
			{
				LONG lAlpha = (LONG)(*current);
                readable << "ctBrushTextureAlpha " << lAlpha << std::endl;
				pRenderer->put_BrushTextureAlpha(lAlpha);

				current += 1;
				curindex += 1;
				break;
			}
			case ctSetTransform:
			{
                double m1 = ReadInt(current, curindex, readable) / 100000.0;
                double m2 = ReadInt(current, curindex, readable) / 100000.0;
                double m3 = ReadInt(current, curindex, readable) / 100000.0;
                double m4 = ReadInt(current, curindex, readable) / 100000.0;
                double m5 = ReadInt(current, curindex, readable) / 100000.0;
                double m6 = ReadInt(current, curindex, readable) / 100000.0;
				pRenderer->SetTransform(m1, m2, m3, m4, m5, m6);
                readable << "ctSetTransform " << m1 << ' ' << m2 << ' '
                         << m3 << ' ' << m4 << ' ' << m5 << ' ' << m6 << std::endl;
				break;
			}
			case ctPathCommandStart:
			{
                readable << "ctPathCommandStart " << std::endl;
				if (bIsPathOpened)
				{
					pRenderer->PathCommandEnd();
					pRenderer->EndCommand(c_nPathType);
				}

				pRenderer->BeginCommand(c_nPathType);
				pRenderer->PathCommandStart();

				bIsPathOpened = true;
				break;
			}
			case ctPathCommandMoveTo:
			{
                double m1 = ReadInt(current, curindex, readable) / 100000.0;
                double m2 = ReadInt(current, curindex, readable) / 100000.0;
                readable << "ctPathCommandMoveTo " << m1 << ' ' << m2 << std::endl;
				pRenderer->PathCommandMoveTo(m1, m2);
				break;
			}
			case ctPathCommandLineTo:
			{
                double m1 = ReadInt(current, curindex, readable) / 100000.0;
                double m2 = ReadInt(current, curindex, readable) / 100000.0;
                readable << "ctPathCommandLineTo " << m1 << ' ' << m2 << std::endl;
				pRenderer->PathCommandLineTo(m1, m2);
				break;
			}
			case ctPathCommandCurveTo:
			{
                double m1 = ReadInt(current, curindex, readable) / 100000.0;
                double m2 = ReadInt(current, curindex, readable) / 100000.0;
                double m3 = ReadInt(current, curindex, readable) / 100000.0;
                double m4 = ReadInt(current, curindex, readable) / 100000.0;
                double m5 = ReadInt(current, curindex, readable) / 100000.0;
                double m6 = ReadInt(current, curindex, readable) / 100000.0;
                readable << "ctPathCommandCurveTo " << m1 << ' ' << m2 << ' '
                         << m3 << ' ' << m4 << ' ' << m5 << ' ' << m6 << std::endl;
				pRenderer->PathCommandCurveTo(m1, m2, m3, m4, m5, m6);
				break;
			}
			case ctPathCommandClose:
			{
                readable << "ctPathCommandClose " << std::endl;
				pRenderer->PathCommandClose();
				break;
			}
			case ctPathCommandEnd:
			{
                readable << "ctPathCommandEnd " << std::endl;
				if (bIsPathOpened)
				{
					pRenderer->PathCommandEnd();
					pRenderer->EndCommand(c_nPathType);
					bIsPathOpened = false;
				}
				break;
			}
			case ctDrawPath:
			{
                INT32 val = ReadInt(current, curindex, readable);
                readable << "ctDrawPath " << val << std::endl;
                pRenderer->DrawPath(val);
				break;
			}
			case ctDrawImageFromFile:
			{
                int nLen = ReadInt(current, curindex, readable);
                std::wstring sTempPath = ReadString16(current, curindex, nLen, readable);
                readable << "ctDrawImageFromFile " << std::endl;
                readable << "temp path: " << sTempPath << std::endl;
				std::wstring sImagePath = pCorrector->GetImagePath(sTempPath);
                readable << "image path: " << sImagePath << std::endl;

                double m1 = ReadInt(current, curindex, readable) / 100000.0;
                double m2 = ReadInt(current, curindex, readable) / 100000.0;
                double m3 = ReadInt(current, curindex, readable) / 100000.0;
                double m4 = ReadInt(current, curindex, readable) / 100000.0;

				try
				{
                    readable << m1 << ' ' << m2 << ' ' << m3 << ' ' << m4 << std::endl;
					pRenderer->DrawImageFromFile(sImagePath, m1, m2, m3, m4);
				}
				catch (...)
				{
				}

				break;
			}
			case ctFontName:
            {
                int _sLen = 2 * (int)ReadUSHORT(current, curindex, readable);
                std::wstring wsTempString = ReadString16(current, curindex, _sLen, readable);
                readable << "ctFontName " << wsTempString << std::endl;
				pRenderer->put_FontName(wsTempString);
				break;
			}
			case ctFontSize:
			{
                double m1 = ReadInt(current, curindex, readable) / 100000.0;
                readable << "ctFontSize " << m1 << std::endl;
				pRenderer->put_FontSize(m1);
				break;
			}
			case ctFontStyle:
			{
                INT32 val = ReadInt(current, curindex, readable);
                readable << "ctFontStyle " << val << std::endl;
                pRenderer->put_FontStyle(val);
				break;
			}
			case ctDrawText:
			{
                int _sLen = 2 * (int)ReadUSHORT(current, curindex, readable);
                std::wstring wsTempString = ReadString16(current, curindex, _sLen, readable);

                double m1 = ReadInt(current, curindex, readable) / 100000.0;
                double m2 = ReadInt(current, curindex, readable) / 100000.0;

                readable << "ctDrawText: " << wsTempString << std::endl;
                readable << m1 << ' ' << m2 << std::endl;

                if (!pPicker)
                    pRenderer->CommandDrawText(wsTempString, m1, m2, 0, 0);
                else
                    pPicker->FillText(wsTempString, m1, m2, 0, 0);
				break;
			}
			case ctBeginCommand:
			{
				if (bIsPathOpened)
				{
					pRenderer->PathCommandEnd();
					pRenderer->EndCommand(4);
					bIsPathOpened = false;
				}
                DWORD val = (DWORD)(ReadInt(current, curindex, readable));
                readable << "ctBeginCommand " << val << std::endl;
                pRenderer->BeginCommand(val);
				break;
			}
			case ctEndCommand:
			{
				if (bIsPathOpened)
				{
					pRenderer->EndCommand(4);
					bIsPathOpened = false;
				}
                DWORD val = (DWORD)(ReadInt(current, curindex, readable));
                readable << "ctEndCommand " << val << std::endl;
                pRenderer->EndCommand(val);
				pRenderer->PathCommandEnd();
				break;
			}
			case ctGradientFill:
			{
				// TODO: Эта команда не должна приходить
                INT32 gradientType = ReadInt(current, curindex, readable);
                readable << "ctGradientFill " << gradientType << std::endl;

				std::wstring sXml, sXmlStop;
				if (0 == gradientType)	//	linearGradient
				{
                    readable << "linearGradient " << std::endl;
                    double x1 = ReadInt(current, curindex, readable) / 100000.0;
                    double x2 = ReadInt(current, curindex, readable) / 100000.0;
                    double y1 = ReadInt(current, curindex, readable) / 100000.0;
                    double y2 = ReadInt(current, curindex, readable) / 100000.0;

                    readable << x1 << ' ' << x2 << ' ' << y1 << ' ' << y2 << std::endl;
                    int stops = ReadInt(current, curindex, readable);
                    readable << "stops " << stops << std::endl;

					for (int i = 0; i < stops; ++i)
					{
						INT32 color   = static_cast<INT32>(*current);
						double opacity = static_cast<double>(static_cast<INT32>(*(current + 1))) / 255.0;
						double offset  = static_cast<double>(static_cast<INT32>(*(current + 2))) / 255.0;
                        readable << i << ": " << color << ' ' << opacity << ' ' << offset << std::endl;

						current  += 6 * 4; // 4 + 1 + 1
						curindex += 6 * 4;
					}
				}
				else if (1 == gradientType)
				{
                    readable << "some other gradient, radial i guess " << std::endl;
                    double cx = ReadInt(current, curindex, readable) / 100000.0;
                    double cy = ReadInt(current, curindex, readable) / 100000.0;
                    double r  = ReadInt(current, curindex, readable) / 100000.0;
                    double fx = ReadInt(current, curindex, readable) / 100000.0;
                    double fy = ReadInt(current, curindex, readable) / 100000.0;

                    readable << cx << ' ' << cy << ' ' << r << ' ' << fx <<  ' ' << fy << std::endl;
                    int stops = ReadInt(current, curindex, readable);
                    readable << "stops " << stops << std::endl;

					for (int i = 0; i < stops; ++i)
					{
						INT32 color   = static_cast<INT32>(*current);
						double opacity = static_cast<double>(static_cast<INT32>(*(current + 1))) / 255.0;
						double offset  = static_cast<double>(static_cast<INT32>(*(current + 2))) / 255.0;

                        readable << i << ": " << color << ' ' << opacity << ' ' << offset << std::endl;
						current  += 6 * 4;		//	4 + 1 + 1
						curindex += 6 * 4;
					}
				}
				break;
			}
			case ctGradientFillXML:
			{
				// TODO: Эта команда не должна приходить

                INT32 gradientType = ReadInt(current, curindex, readable);
                readable << "ctGradientFillXML " << gradientType << std::endl;
                int _sLen = ReadInt(current, curindex, readable);
                std::wstring wsTempString = ReadString16(current, curindex, _sLen, readable);
                readable << wsTempString << std::endl;
				break;
			}
			case ctGradientStroke:
			{
				// TODO: Эта команда не должна приходить
                INT32 gradientType = ReadInt(current, curindex, readable);
                readable << "ctGradientStroke " << gradientType << std::endl;
				if (0 == gradientType)	//	linearGradient
				{
                    readable << "linear gradient" << std::endl;
                    double x1 = ReadInt(current, curindex, readable) / 100000.0;
                    double x2 = ReadInt(current, curindex, readable) / 100000.0;
                    double y1 = ReadInt(current, curindex, readable) / 100000.0;
                    double y2 = ReadInt(current, curindex, readable) / 100000.0;
                    readable << x1 << ' ' << x2 << ' ' << y1 << ' ' << y2 << std::endl;

                    int stops = ReadInt(current, curindex, readable);
                    readable << "stops " << stops << std::endl;

					for (int i = 0; i < stops; ++i)
					{
						INT32 color   = static_cast<INT32>(*current);
						double opacity = static_cast<double>(static_cast<INT32>(*(current + 1))) / 255.0;
						double offset  = static_cast<double>(static_cast<INT32>(*(current + 2))) / 255.0;
                        readable << i << ": " << color << ' ' << opacity << ' ' << offset << std::endl;

						current  += 6 * 4; // 4 + 1 + 1
						curindex += 6 * 4;
					}
				}
				else if (1 == gradientType)
				{
                    readable << "some other gradient, radial i guess " << std::endl;
                    double cx = ReadInt(current, curindex, readable) / 100000.0;
                    double cy = ReadInt(current, curindex, readable) / 100000.0;
                    double r  = ReadInt(current, curindex, readable) / 100000.0;
                    double fx = ReadInt(current, curindex, readable) / 100000.0;
                    double fy = ReadInt(current, curindex, readable) / 100000.0;
                    readable << cx << ' ' << cy << ' ' << r << ' ' << fx <<  ' ' << fy << std::endl;
                    int stops = ReadInt(current, curindex, readable);
                    readable << "stops " << stops << std::endl;

					for (int i = 0; i < stops; ++i)
					{
						INT32 color   = static_cast<INT32>(*current);
						double opacity = static_cast<double>(static_cast<INT32>(*(current + 1))) / 255.0;
						double offset  = static_cast<double>(static_cast<INT32>(*(current + 2))) / 255.0;
                        readable << i << ": " << color << ' ' << opacity << ' ' << offset << std::endl;

						current  += 6 * 4; // 4 + 1 + 1
						curindex += 6 * 4;
					}
				}
				break;
			}
			case ctGradientStrokeXML:
			{
				// TODO: Эта команда не должна приходить
                INT32 gradientType = ReadInt(current, curindex, readable);
                readable << "ctGradientStrokeXML " << gradientType << std::endl;
                int _sLen = (int)ReadInt(current, curindex, readable);
                std::wstring wsTempString = ReadString16(current, curindex, _sLen, readable);
                readable << wsTempString << std::endl;
				break;
			}
			case ctHyperlink:
			{
                readable << "ctHyperlink " << std::endl;
                double dX = ReadDouble(current, curindex, readable);
                double dY = ReadDouble(current, curindex, readable);
                double dW = ReadDouble(current, curindex, readable);
                double dH = ReadDouble(current, curindex, readable);
                readable << dX << ' ' << dY << ' ' << dW << ' ' << dH << std::endl;

                std::wstring wsUrl     = ReadString(current, curindex, readable);
                std::wstring wsTooltip = ReadString(current, curindex, readable);
                readable << "url: " << wsUrl << std::endl;
                readable << "tooltip: " << wsTooltip << std::endl;


				pRenderer->AddHyperlink(dX, dY, dW, dH, wsUrl, wsTooltip);
				break;
			}
			case ctLink:
			{
                readable << "ctLink " << std::endl;
                double dX = ReadDouble(current, curindex, readable);
                double dY = ReadDouble(current, curindex, readable);
                double dW = ReadDouble(current, curindex, readable);
                double dH = ReadDouble(current, curindex, readable);
                readable << dX << ' ' << dY << ' ' << dW << ' ' << dH << std::endl;

                double dDestX = ReadDouble(current, curindex, readable);
                double dDestY = ReadDouble(current, curindex, readable);
                int    nPage  = ReadInt(current, curindex, readable);
                readable << dDestX << ' ' << dDestY << ' ' << nPage << std::endl;

				pRenderer->AddLink(dX, dY, dW, dH, dDestX, dDestY, nPage);
				break;
			}
			case ctFormField:
			{
                readable << "ctFormField" << std::endl;
				BYTE* nStartPos   = current;
				int   nStartIndex = curindex;

                int nLen = ReadInt(current, curindex, readable);
                readable << "nLen = " << nLen << std::endl;

                double dX = ReadDouble(current, curindex, readable);
                double dY = ReadDouble(current, curindex, readable);
                double dW = ReadDouble(current, curindex, readable);
                double dH = ReadDouble(current, curindex, readable);
                readable << "dX = " << dX << " dY = " << dY
                         << " dW = " << dW << " dH = " << dH << std::endl;

				CFormFieldInfo oInfo;
				oInfo.SetBounds(dX, dY, dW, dH);
                double baseLineOffset = ReadDouble(current, curindex, readable);
                readable << "base line offset = " << baseLineOffset << std::endl;
                oInfo.SetBaseLineOffset(baseLineOffset);

                int nFlags = ReadInt(current, curindex, readable);

                if (nFlags & 1){
                    std::wstring keyStr = ReadString(current, curindex, readable);
                    readable << "key: " << keyStr << std::endl;
                    oInfo.SetKey(keyStr);
                }

                if (nFlags & 2){
                    std::wstring helpText = ReadString(current, curindex, readable);
                    readable << "helpText: " << helpText << std::endl;
                    oInfo.SetHelpText(helpText);
                }

                readable << "required: " << std::boolalpha << (bool)(nFlags & 4) << std::endl;
				oInfo.SetRequired(nFlags & 4);
                readable << "PlaceHolder: " << std::boolalpha << (bool)(nFlags & 8) << std::endl;
				oInfo.SetPlaceHolder(nFlags & 8);

				if (nFlags & (1 << 6))
				{
                    int nBorderType = ReadInt(current, curindex, readable);
                    double dBorderSize = ReadDouble(current, curindex, readable);
                    unsigned char unR = ReadByte(current, curindex, readable);
                    unsigned char unG = ReadByte(current, curindex, readable);
                    unsigned char unB = ReadByte(current, curindex, readable);
                    unsigned char unA = ReadByte(current, curindex, readable);
                    readable << "border type: " << nBorderType << "\nborder size = " << dBorderSize
                             << "\nrgba = " << unR << unG << unB << unA << std::endl;

					oInfo.SetBorder(nBorderType, dBorderSize, unR, unG, unB, unA);
				}

				if (nFlags & (1 << 9))
				{
                    unsigned char unR = ReadByte(current, curindex, readable);
                    unsigned char unG = ReadByte(current, curindex, readable);
                    unsigned char unB = ReadByte(current, curindex, readable);
                    unsigned char unA = ReadByte(current, curindex, readable);
                    readable << "shd rgba: " << unR << unG << unB << unA << std::endl;
					oInfo.SetShd(unR, unG, unB, unA);
				}

                INT32 type = ReadInt(current, curindex, readable);
                readable << "type: " << type << std::endl;
                oInfo.SetType(type);

				if (oInfo.IsTextField())
				{
					CFormFieldInfo::CTextFormPr* pPr = oInfo.GetTextFormPr();
                    readable << std::boolalpha << "comb: " << (bool)(nFlags & (1 << 20)) << std::endl;
					pPr->SetComb(nFlags & (1 << 20));

                    if (nFlags & (1 << 21)){
                        INT32 maxCharacters = ReadInt(current, curindex, readable);
                        readable << "maxCharacters: " << maxCharacters << std::endl;
                        pPr->SetMaxCharacters(maxCharacters);
                    }

                    if (nFlags & (1 << 22)){
                        std::wstring textValue = ReadString(current, curindex, readable);
                        readable << "textValue: " << textValue << std::endl;
                        pPr->SetTextValue(textValue);
                    }

                    readable << std::boolalpha << "autofit: "
                            << (bool)(nFlags & (1 << 23)) << std::endl;
					pPr->SetAutoFit(nFlags & (1 << 23));
                    readable << std::boolalpha << "MultiLine: "
                            << (bool)(nFlags & (1 << 24)) << std::endl;
					pPr->SetMultiLine(nFlags & (1 << 24));
				}
				else if (oInfo.IsDropDownList())
				{
					CFormFieldInfo::CDropDownFormPr* pPr = oInfo.GetDropDownFormPr();
                    readable << std::boolalpha << "EditComboBox: "
                            << (bool)(nFlags & (1 << 20)) << std::endl;
					pPr->SetEditComboBox(nFlags & (1 << 20));

                    int nItemsCount = ReadInt(current, curindex, readable);
					for (int nIndex = 0; nIndex < nItemsCount; ++nIndex)
					{
                        std::wstring item = ReadString(current, curindex, readable);
                        readable << "combo box item " << nIndex << ": " << item << std::endl;
                        pPr->AddComboBoxItem(item);
					}

                    int nSelectedIndex = ReadInt(current, curindex, readable);
                    readable << "SelectedIndex: " << nSelectedIndex << std::endl;

                    if (nFlags & (1 << 22)){
                        std::wstring textValue = ReadString(current, curindex, readable);
                        readable << "textValue: " << textValue << std::endl;
                        pPr->SetTextValue(textValue);
                    }
				}
				else if (oInfo.IsCheckBox())
				{
					CFormFieldInfo::CCheckBoxFormPr* pPr = oInfo.GetCheckBoxFormPr();
                    readable << std::boolalpha << "is checked: " << (bool)(nFlags & (1 << 20)) << std::endl;
					pPr->SetChecked(nFlags & (1 << 20));
                    INT32 CheckedSymbol = ReadInt(current, curindex, readable);
                    readable << "CheckedSymbol: " << CheckedSymbol << std::endl;
                    pPr->SetCheckedSymbol(CheckedSymbol);
                    std::wstring checkedFont = ReadString(current, curindex, readable);
                    readable << "checkedFont: " << checkedFont << std::endl;
                    pPr->SetCheckedFont(checkedFont);
                    INT32 UncheckedSymbol = ReadInt(current, curindex, readable);
                    readable << "UncheckedSymbol: " << UncheckedSymbol << std::endl;
                    pPr->SetUncheckedSymbol(UncheckedSymbol);
                    std::wstring uncheckedFont = ReadString(current, curindex, readable);
                    readable << "uncheckedFont: " << uncheckedFont << std::endl;
                    pPr->SetUncheckedFont(uncheckedFont);

                    if (nFlags & (1 << 21)){
                        std::wstring GroupKey = ReadString(current, curindex, readable);
                        readable << "GroupKey: " << GroupKey << std::endl;
                        pPr->SetGroupKey(GroupKey);
                    }
				}
				else if (oInfo.IsPicture())
				{
					CFormFieldInfo::CPictureFormPr* pPr = oInfo.GetPictureFormPr();
                    readable << std::boolalpha << "ConstantProportions: "
                        << (bool)(nFlags & (1 << 20)) << std::endl;
					pPr->SetConstantProportions(nFlags & (1 << 20));
                    readable << std::boolalpha << "RespectBorders: "
                        << (bool)(nFlags & (1 << 21)) << std::endl;
					pPr->SetRespectBorders(nFlags & (1 << 21));
                    INT32 scaleType{(nFlags >> 24) & 0xF};
                    readable << "scale type: " << scaleType << std::endl;
                    pPr->SetScaleType((CFormFieldInfo::EScaleType)scaleType);
                    LONG lShiftX = ReadInt(current, curindex, readable);
                    LONG lShiftY = ReadInt(current, curindex, readable);
                    readable << "shift X = " << lShiftX << " shift Y = " << lShiftY << std::endl;
					pPr->SetShift(lShiftX, lShiftY);
				}

				if (oInfo.IsValid())
					pRenderer->AddFormField(oInfo);

				current  = nStartPos + nLen;
				curindex = nStartIndex + nLen;

				break;
			}
			default:
			{
                readable << "COMMON unknown command" << std::endl;
				break;
			}
			}; // switch (eCommand)
		} // while (curindex < len)

		return true;
	}

    void CMetafilePagesInfo::CheckBuffer(BYTE* pBuffer, LONG lBufferLen)
    {
        CommandType eCommand = ctError;

        bool bIsPathOpened = false;
        int curindex = 0;

        std::wfstream dummy;

        BYTE* current = pBuffer;
        while (curindex < lBufferLen)
        {
            eCommand = (CommandType)(*current);
            current++;
            curindex++;
            switch (eCommand)
            {
            case ctPageWidth:
            {
                arSizes[PagesCount - 1].width = (ReadInt(current, curindex, dummy) / 100000.0);
                break;
            }
            case ctPageHeight:
            {
                arSizes[PagesCount - 1].height = (ReadInt(current, curindex, dummy) / 100000.0);
                break;
            }
            case ctPageStart:
            {
                AddPage();
                arSizes[PagesCount - 1].data = current;
                break;
            }
            case ctPageEnd:
            {
                break;
            }
            case ctPenColor:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctPenAlpha:
            {
                current++;
                curindex++;
                break;
            }
            case ctPenSize:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctPenDashStyle:
            {
                BYTE nDashType = *current++;
                curindex++;
                switch (nDashType)
                {
                case Aggplus::DashStyleCustom:
                {
                    int nCountDash = ReadInt(current, curindex, dummy);
                    if (0 < nCountDash)
                    {
                        SkipInt(current, curindex, dummy, nCountDash);
                    }
                }
                default:
                    break;
                }

                break;
            }
            case ctPenLineJoin:
            {
                current++;
                curindex++;
                break;
            }
            case ctBrushType:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctBrushColor1:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctBrushAlpha1:
            {
                current++;
                curindex++;
                break;
            }
            case ctBrushColor2:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctBrushAlpha2:
            {
                current++;
                curindex++;
                break;
            }
            case ctBrushRectable:
            {
                SkipInt(current, curindex, dummy, 4);
                break;
            }
            case ctBrushRectableEnabled:
            {
                current += 1;
                curindex += 1;
                break;
            }
            case ctBrushTexturePath:
            {
                int nLen = 2 * ReadUSHORT(current, curindex, dummy);
                SkipString16(current, curindex, nLen, dummy);
                break;
            }
            case ctBrushGradient:
            {
                current++;
                curindex++;

                while (true)
                {
                    BYTE _command = *current;
                    current++;
                    curindex++;

                    if (251 == _command)
                        break;

                    switch (_command)
                    {
                    case 0:
                    {
                        current += 5;
                        curindex += 5;
                        SkipInt(current, curindex, dummy, 4);
                        break;
                    }
                    case 1:
                    {
                        current++;
                        curindex++;
                        SkipInt(current, curindex, dummy, 6);
                        break;
                    }
                    case 2:
                    {
                        LONG lColorsCount = (LONG)ReadInt(current, curindex, dummy);
                        if (0 <= lColorsCount)
                        {
                            SkipInt(current, curindex, dummy, 2 * lColorsCount);
                        }

                        break;
                    }
                    default:
                    {
                        break;
                    }
                    };
                }

                break;
            }
            case ctBrushTextureMode:
            {
                current += 1;
                curindex += 1;
                break;
            }
            case ctBrushTextureAlpha:
            {
                current += 1;
                curindex += 1;
                break;
            }
            case ctSetTransform:
            {
                SkipInt(current, curindex, dummy, 6);
                break;
            }
            case ctPathCommandStart:
            {
                break;
            }
            case ctPathCommandMoveTo:
            {
                SkipInt(current, curindex, dummy, 2);
                break;
            }
            case ctPathCommandLineTo:
            {
                SkipInt(current, curindex, dummy, 2);
                break;
            }
            case ctPathCommandCurveTo:
            {
                SkipInt(current, curindex, dummy, 6);
                break;
            }
            case ctPathCommandClose:
            {
                break;
            }
            case ctPathCommandEnd:
            {
                break;
            }
            case ctDrawPath:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctDrawImageFromFile:
            {
                int nLen = ReadInt(current, curindex, dummy);
                SkipString16(current, curindex, nLen, dummy);

                SkipInt(current, curindex, dummy, 4);
                break;
            }
            case ctFontName:
            {
                int nLen = 2 * (int)ReadUSHORT(current, curindex, dummy);
                SkipString16(current, curindex, nLen, dummy);
                break;
            }
            case ctFontSize:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctFontStyle:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctDrawText:
            {
                int nLen = 2 * (int)ReadUSHORT(current, curindex, dummy);
                SkipString16(current, curindex, nLen, dummy);

                SkipInt(current, curindex, dummy, 2);
                break;
            }
            case ctBeginCommand:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctEndCommand:
            {
                SkipInt(current, curindex, dummy);
                break;
            }
            case ctGradientFill:
            case ctGradientFillXML:
            case ctGradientStroke:
            case ctGradientStrokeXML:
            {
                // TODO: Эта команда не должна приходить
                return;
            }
			case ctHyperlink:
			{
                SkipDouble(current, curindex, dummy, 4);
                SkipString(current, curindex, dummy);
                SkipString(current, curindex, dummy);
				break;
			}
			case ctLink:
			{
                SkipDouble(current, curindex, dummy, 6);
                SkipInt(current, curindex, dummy);
				break;
			}
			case ctFormField:
			{
				BYTE* nStartPos   = current;
				int   nStartIndex = curindex;

                int nLen = ReadInt(current, curindex, dummy);

				current  = nStartPos + nLen;
				curindex = nStartIndex + nLen;
				break;
			}
            default:
            {
                break;
            }
            }; // switch (eCommand)
        } // while (curindex < len)
    }
}
