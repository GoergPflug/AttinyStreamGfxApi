#define _CRT_RAND_S
#define _CRT_SECURE_NO_WARNINGS
int main_encoder(char *name);

/******************************************************************************
* Filename: MFMP4ToYUVWithoutMFT.cpp
*
* Description:
* The sample is based on the MFMP4ToYUVWithMFT one. The sample gets YUV
* samples out of the Source Reader WITHOUT having to explicitly wire up
* the H264 decoder transform.
*
* This file contains a C++ console application that reads H264 encoded video
* frames from an mp4 file and decodes them to a YUV pixel format and dumps them
* to an output file stream.
*
* Note: If using the big_buck_bunny.mp4 file as the source the frame size changes
* from 640x360 to 640x368 on the 25th sample. This program deals with it by throwing
* overwriting the captured file whenever the source stream changes.
*
* To convert the raw yuv data dumped at the end of this sample use the ffmpeg command below:
* ffmpeg -vcodec rawvideo -s 640x368 -pix_fmt yuv420p -i rawframes.yuv -vframes 1 output.jpeg
* ffmpeg -vcodec rawvideo -s 640x368 -pix_fmt yuv420p -i rawframes.yuv out.avi
*
* Author:
* Aaron Clauson (aaron@sipsorcery.com)
*
* History:
* 23 Jan 2020	  Aaron Clauson	  Created, Dublin, Ireland. Based on MFMP4ToYUVWithMFT sample.
*
* License: Public Domain (no warranty, use at own risk)
/******************************************************************************/
/******************************************************************************
* Filename: MFUtility.h
*
* Description:
* This header file contains common macros and functions that are used in the
* Media Foundation sample applications.
*
* Author:
* Aaron Clauson (aaron@sipsorcery.com)
*
* History:
* 07 Mar 2015	  Aaron Clauson	  Created, Hobart, Australia.
* 03 Jan 2019   Aaron Clauson   Removed managed C++ references.
*
* License: Public Domain (no warranty, use at own risk)
/******************************************************************************/

#include <stdio.h>
#include <tchar.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <mmdeviceapi.h>
#include <wmcodecdsp.h>
#include <wmsdkidl.h>

#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>

#define CHECK_HR(hr, msg) if (hr != S_OK) { printf(msg); printf(" Error: %.2X.\n", hr); goto done; }

#define CHECKHR_GOTO(x, y) if(FAILED(x)) goto y

#define INTERNAL_GUID_TO_STRING( _Attribute, _skip ) \
if (Attr == _Attribute) \
{ \
	pAttrStr = #_Attribute; \
	C_ASSERT((sizeof(#_Attribute) / sizeof(#_Attribute[0])) > _skip); \
	pAttrStr += _skip; \
	goto done; \
} \

template <class T> void SAFE_RELEASE(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

template <class T> inline void SAFE_RELEASE(T*& pT)
{
	if (pT != NULL)
	{
		pT->Release();
		pT = NULL;
	}
}

enum class DeviceType { Audio, Video };

#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return #val
#endif

LPCSTR GetGUIDNameConst(const GUID& guid)
{
	IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
	IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
	IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
	IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
	IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
	IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
	IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
	IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
	IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
	IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
	IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
	IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
	IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
	IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
	IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
	IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
	IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
	IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
	IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
	IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
	IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
	IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
	IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
	IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
	IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
	IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
	IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
	IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
	IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
	IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
	IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
	IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
	IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
	IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
	IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
	IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
	IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
	IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
	IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
	IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
	IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
	IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
	IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
	IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
	IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
	IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC);
	IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);

	// Media types

	IF_EQUAL_RETURN(guid, MFMediaType_Audio);
	IF_EQUAL_RETURN(guid, MFMediaType_Video);
	IF_EQUAL_RETURN(guid, MFMediaType_Protected);
	IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
	IF_EQUAL_RETURN(guid, MFMediaType_Script);
	IF_EQUAL_RETURN(guid, MFMediaType_Image);
	IF_EQUAL_RETURN(guid, MFMediaType_HTML);
	IF_EQUAL_RETURN(guid, MFMediaType_Binary);
	IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

	IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
	IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8 
	IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
	IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
	IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
	IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
	IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
	IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
	IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
	IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
	IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
	IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
	IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
	IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
	IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8 
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8 
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5 
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5 
	IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
	IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
	IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
	IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
	IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
	IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
	IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
	IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
	IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
	IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
	IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);

	IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM 
	IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT 
	IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS 
	IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF 
	IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM 
	IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2 
	IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3 
	IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS 
	IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF 
	IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9 
	IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3 
	IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG 
	IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC 
	IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC 

	return NULL;
}

/**
* Helper function to get a user friendly description for a media type.
* Note that there may be properties missing or incorrectly described.
* @param[in] pMediaType: pointer to the media type to get a description for.
* @@Returns A string describing the media type.
*
* Potential improvements https://docs.microsoft.com/en-us/windows/win32/medfound/media-type-debugging-code.
*/
std::string GetMediaTypeDescription(IMFMediaType* pMediaType)
{
	HRESULT hr = S_OK;
	GUID MajorType;
	UINT32 cAttrCount;
	LPCSTR pszGuidStr;
	std::string description;
	WCHAR TempBuf[200];

	if (pMediaType == NULL)
	{
		description = "<NULL>";
		goto done;
	}

	hr = pMediaType->GetMajorType(&MajorType);
	CHECKHR_GOTO(hr, done);

	//pszGuidStr = STRING_FROM_GUID(MajorType);
	pszGuidStr = GetGUIDNameConst(MajorType);
	if (pszGuidStr != NULL)
	{
		description += pszGuidStr;
		description += ": ";
	}
	else
	{
		description += "Other: ";
	}

	hr = pMediaType->GetCount(&cAttrCount);
	CHECKHR_GOTO(hr, done);

	for (UINT32 i = 0; i < cAttrCount; i++)
	{
		GUID guidId;
		MF_ATTRIBUTE_TYPE attrType;

		hr = pMediaType->GetItemByIndex(i, &guidId, NULL);
		CHECKHR_GOTO(hr, done);

		hr = pMediaType->GetItemType(guidId, &attrType);
		CHECKHR_GOTO(hr, done);

		//pszGuidStr = STRING_FROM_GUID(guidId);
		pszGuidStr = GetGUIDNameConst(guidId);
		if (pszGuidStr != NULL)
		{
			description += pszGuidStr;
		}
		else
		{
			LPOLESTR guidStr = NULL;

			CHECKHR_GOTO(StringFromCLSID(guidId, &guidStr), done);
			auto wGuidStr = std::wstring(guidStr);
			description += std::string(wGuidStr.begin(), wGuidStr.end()); // GUID's won't have wide chars.

			CoTaskMemFree(guidStr);
		}

		description += "=";

		switch (attrType)
		{
		case MF_ATTRIBUTE_UINT32:
		{
			UINT32 Val;
			hr = pMediaType->GetUINT32(guidId, &Val);
			CHECKHR_GOTO(hr, done);

			description += std::to_string(Val);
			break;
		}
		case MF_ATTRIBUTE_UINT64:
		{
			UINT64 Val;
			hr = pMediaType->GetUINT64(guidId, &Val);
			CHECKHR_GOTO(hr, done);

			if (guidId == MF_MT_FRAME_SIZE)
			{
				description += "W:" + std::to_string(HI32(Val)) + " H: " + std::to_string(LO32(Val));
			}
			else if (guidId == MF_MT_FRAME_RATE)
			{
				// Frame rate is numerator/denominator.
				description += std::to_string(HI32(Val)) + "/" + std::to_string(LO32(Val));
			}
			else if (guidId == MF_MT_PIXEL_ASPECT_RATIO)
			{
				description += std::to_string(HI32(Val)) + ":" + std::to_string(LO32(Val));
			}
			else
			{
				//tempStr.Format("%ld", Val);
				description += std::to_string(Val);
			}

			//description += tempStr;

			break;
		}
		case MF_ATTRIBUTE_DOUBLE:
		{
			DOUBLE Val;
			hr = pMediaType->GetDouble(guidId, &Val);
			CHECKHR_GOTO(hr, done);

			//tempStr.Format("%f", Val);
			description += std::to_string(Val);
			break;
		}
		case MF_ATTRIBUTE_GUID:
		{
			GUID Val;
			const char* pValStr;

			hr = pMediaType->GetGUID(guidId, &Val);
			CHECKHR_GOTO(hr, done);

			//pValStr = STRING_FROM_GUID(Val);
			pValStr = GetGUIDNameConst(Val);
			if (pValStr != NULL)
			{
				description += pValStr;
			}
			else
			{
				LPOLESTR guidStr = NULL;
				CHECKHR_GOTO(StringFromCLSID(Val, &guidStr), done);
				auto wGuidStr = std::wstring(guidStr);
				description += std::string(wGuidStr.begin(), wGuidStr.end()); // GUID's won't have wide chars.

				CoTaskMemFree(guidStr);
			}

			break;
		}
		case MF_ATTRIBUTE_STRING:
		{
			hr = pMediaType->GetString(guidId, TempBuf, sizeof(TempBuf) / sizeof(TempBuf[0]), NULL);
			if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
			{
				description += "<Too Long>";
				break;
			}
			CHECKHR_GOTO(hr, done);
			auto wstr = std::wstring(TempBuf);
			description += std::string(wstr.begin(), wstr.end()); // It's unlikely the attribute descriptions will contain multi byte chars.

			break;
		}
		case MF_ATTRIBUTE_BLOB:
		{
			description += "<BLOB>";
			break;
		}
		case MF_ATTRIBUTE_IUNKNOWN:
		{
			description += "<UNK>";
			break;
		}
		}

		description += ", ";
	}

done:

	return description;
}

/**
* Helper function to get a user friendly description for a media type.
* Note that there may be properties missing or incorrectly described.
* @param[in] pMediaType: pointer to the video media type to get a description for.
* @@Returns A string describing the media type.
*
* Potential improvements https://docs.microsoft.com/en-us/windows/win32/medfound/media-type-debugging-code.
*/
std::string GetVideoTypeDescriptionBrief(IMFMediaType* pMediaType)
{
	std::string description = " ";
	GUID subType;
	UINT32 width = 0, height = 0, fpsNum = 0, fpsDen = 0;

	if (pMediaType == NULL) {
		description = " <NULL>";
	}
	else {
		CHECK_HR(pMediaType->GetGUID(MF_MT_SUBTYPE, &subType), "Failed to get video sub type.");
		CHECK_HR(MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &width, &height), "Failed to get MF_MT_FRAME_SIZE attribute.");
		CHECK_HR(MFGetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, &fpsNum, &fpsDen), "Failed to get MF_MT_FRAME_RATE attribute.");

		description += GetGUIDNameConst(subType);
		description += ", " + std::to_string(width) + "x" + std::to_string(height) + ", " + std::to_string(fpsNum) + "/" + std::to_string(fpsDen) + "fps";
	}

done:

	return description;
}

HRESULT FindMatchingVideoType(IMFMediaTypeHandler* pMediaTypeHandler, const GUID& pixelFormat, uint32_t width, uint32_t height, uint32_t fps, IMFMediaType* pOutMediaType)
{
	HRESULT hr = S_FALSE;
	DWORD mediaTypeCount = 0;
	GUID subType;
	UINT32 w = 0, h = 0, fpsNum = 0, fpsDen = 0;

	CHECK_HR(pMediaTypeHandler->GetMediaTypeCount(&mediaTypeCount),
		"Failed to get sink media type count.");

	for (int i = 0; i < mediaTypeCount; i++) {
		IMFMediaType* pMediaType = NULL;
		CHECK_HR(pMediaTypeHandler->GetMediaTypeByIndex(i, &pMediaType), "Failed to get media type.");

		CHECK_HR(pMediaType->GetGUID(MF_MT_SUBTYPE, &subType), "Failed to get video sub type.");
		CHECK_HR(MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &w, &h), "Failed to get MF_MT_FRAME_SIZE attribute.");
		CHECK_HR(MFGetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, &fpsNum, &fpsDen), "Failed to get MF_MT_FRAME_RATE attribute.");

		if (IsEqualGUID(pixelFormat, subType) && w == width && h == height && fps == fpsNum && fpsDen == 1) {
			CHECK_HR(pMediaType->CopyAllItems(pOutMediaType), "Error copying media type attributes.");
			SAFE_RELEASE(pMediaType);
			hr = S_OK;
			break;
		}
		else {
			SAFE_RELEASE(pMediaType);
		}
	}

done:
	return hr;
}

/*
Lists all the available media types attached to a media type handler.
The media type handler can be acquired from a source reader or sink writer.
* @param[in] pMediaTypeHandler: pointer to the media type handler to list
*  the types for.
* @@Returns S_OK if successful or an error code if not.
*/
HRESULT ListMediaTypes(IMFMediaTypeHandler* pMediaTypeHandler)
{
	HRESULT hr = S_OK;
	DWORD mediaTypeCount = 0;

	hr = pMediaTypeHandler->GetMediaTypeCount(&mediaTypeCount);
	CHECK_HR(hr, "Failed to get sink media type count.");

	std::cout << "Sink media type count: " << mediaTypeCount << "." << std::endl;

	for (int i = 0; i < mediaTypeCount; i++) {
		IMFMediaType* pMediaType = NULL;
		hr = pMediaTypeHandler->GetMediaTypeByIndex(i, &pMediaType);
		CHECK_HR(hr, "Failed to get media type.");

		std::cout << "Media type " << i << ": " << std::endl;
		std::cout << GetMediaTypeDescription(pMediaType) << std::endl;

		SAFE_RELEASE(pMediaType);
	}

done:
	return hr;
}

/*
* List all the media modes available on a media source.
* @param[in] pReader: pointer to the media source reader to list the media types for.
*/
void ListModes(IMFSourceReader* pReader, bool brief = false)
{
	HRESULT hr = NULL;
	DWORD dwMediaTypeIndex = 0;

	while (SUCCEEDED(hr))
	{
		IMFMediaType* pType = NULL;
		hr = pReader->GetNativeMediaType(0, dwMediaTypeIndex, &pType);
		if (hr == MF_E_NO_MORE_TYPES)
		{
			hr = S_OK;
			break;
		}
		else if (SUCCEEDED(hr))
		{
			if (!brief) {
				std::cout << GetMediaTypeDescription(pType) << std::endl;
			}
			else {
				std::cout << GetVideoTypeDescriptionBrief(pType) << std::endl;
			}

			pType->Release();
		}
		++dwMediaTypeIndex;
	}
}

/**
* Prints out a list of the audio or video capture devices available.
* @param[in] deviceType: whether to list audio or video capture devices.
* @@Returns S_OK if successful or an error code if not.
*
* Remarks:
* See https://docs.microsoft.com/en-us/windows/win32/coreaudio/device-properties.
*/
HRESULT ListCaptureDevices(DeviceType deviceType)
{
	IMFAttributes* pDeviceAttributes = NULL;
	IMFActivate** ppDevices = NULL;
	UINT32 deviceCount = 0;

	HRESULT hr = S_OK;

	hr = MFCreateAttributes(&pDeviceAttributes, 1);
	CHECK_HR(hr, "Error creating device attributes.");

	if (deviceType == DeviceType::Audio) {
		// Request audio capture devices.
		hr = pDeviceAttributes->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);
		CHECK_HR(hr, "Error initialising audio configuration object.");
	}
	else {
		// Request video capture devices.
		hr = pDeviceAttributes->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
		CHECK_HR(hr, "Error initialising video configuration object.");
	}

	hr = MFEnumDeviceSources(pDeviceAttributes, &ppDevices, &deviceCount);
	CHECK_HR(hr, "Error enumerating devices.");

	wprintf(L"Device count %d.\n", deviceCount);

	for (UINT i = 0; i < deviceCount; i++) {

		LPWSTR friendlyName = NULL;
		UINT32 friendlyNameLength = 0;
		IMFMediaSource* pMediaSource = NULL;
		IMFSourceReader* pSourceReader = NULL;

		hr = ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &friendlyName, &friendlyNameLength);
		CHECK_HR(hr, "Error retrieving device friendly name.");

		wprintf(L"Device name: %s\n", friendlyName);

		hr = ppDevices[i]->ActivateObject(IID_PPV_ARGS(&pMediaSource));
		CHECK_HR(hr, "Error activating device media source.");

		hr = MFCreateSourceReaderFromMediaSource(
			pMediaSource,
			NULL,
			&pSourceReader);
		CHECK_HR(hr, "Error creating device source reader.");

		ListModes(pSourceReader);

		CoTaskMemFree(friendlyName);
		SAFE_RELEASE(pMediaSource);
		SAFE_RELEASE(pSourceReader);
	}

done:

	SAFE_RELEASE(pDeviceAttributes);
	CoTaskMemFree(ppDevices);

	return hr;
}

/**
* Prints out a list of the audio or video capture devices available along with
* a brief description of the most important format properties.
* @param[in] deviceType: whether to list audio or video capture devices.
* @@Returns S_OK if successful or an error code if not.
*
* Remarks:
* See https://docs.microsoft.com/en-us/windows/win32/coreaudio/device-properties.
*/
HRESULT ListVideoDevicesWithBriefFormat()
{
	IMFAttributes* pDeviceAttributes = NULL;
	IMFActivate** ppDevices = NULL;
	UINT32 deviceCount = 0;

	HRESULT hr = S_OK;

	hr = MFCreateAttributes(&pDeviceAttributes, 1);
	CHECK_HR(hr, "Error creating device attributes.");

	// Request video capture devices.
	hr = pDeviceAttributes->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	CHECK_HR(hr, "Error initialising video configuration object.");

	hr = MFEnumDeviceSources(pDeviceAttributes, &ppDevices, &deviceCount);
	CHECK_HR(hr, "Error enumerating devices.");

	wprintf(L"Device count %d.\n", deviceCount);

	for (UINT i = 0; i < deviceCount; i++) {

		LPWSTR friendlyName = NULL;
		UINT32 friendlyNameLength = 0;
		IMFMediaSource* pMediaSource = NULL;
		IMFSourceReader* pSourceReader = NULL;

		hr = ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &friendlyName, &friendlyNameLength);
		CHECK_HR(hr, "Error retrieving device friendly name.");

		wprintf(L"Device name: %s\n", friendlyName);

		hr = ppDevices[i]->ActivateObject(IID_PPV_ARGS(&pMediaSource));
		CHECK_HR(hr, "Error activating device media source.");

		hr = MFCreateSourceReaderFromMediaSource(
			pMediaSource,
			NULL,
			&pSourceReader);
		CHECK_HR(hr, "Error creating device source reader.");

		ListModes(pSourceReader, true);

		CoTaskMemFree(friendlyName);
		SAFE_RELEASE(pMediaSource);
		SAFE_RELEASE(pSourceReader);
	}

done:

	SAFE_RELEASE(pDeviceAttributes);
	CoTaskMemFree(ppDevices);

	return hr;
}

/**
* Attempts to print out a list of all the audio output devices
* available on the system.
* @@Returns S_OK if successful or an error code if not.
*
* Remarks:
* See https://docs.microsoft.com/en-us/windows/win32/medfound/streaming-audio-renderer.
*/
HRESULT ListAudioOutputDevices()
{
	HRESULT hr = S_OK;

	IMMDeviceEnumerator* pEnum = NULL;      // Audio device enumerator.
	IMMDeviceCollection* pDevices = NULL;   // Audio device collection.
	IMMDevice* pDevice = NULL;              // An audio device.
	UINT deviceCount = 0;

	// Create the device enumerator.
	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL,
		CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pEnum
	);

	// Enumerate the rendering devices.
	hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
	CHECK_HR(hr, "Failed to enumerate audio end points.");

	hr = pDevices->GetCount(&deviceCount);
	CHECK_HR(hr, "Failed to get audio end points count.");

	std::cout << "Audio output device count " << deviceCount << "." << std::endl;

	for (int i = 0; i < deviceCount; i++) {
		LPWSTR wstrID = NULL;                   // Device ID.

		hr = pDevices->Item(i, &pDevice);
		CHECK_HR(hr, "Failed to get device for ID.");

		hr = pDevice->GetId(&wstrID);
		CHECK_HR(hr, "Failed to get name for device.");

		std::wcout << "Audio output device " << i << ": " << wstrID << "." << std::endl;

		CoTaskMemFree(wstrID);
	}

done:

	SAFE_RELEASE(pEnum);
	SAFE_RELEASE(pDevices);
	SAFE_RELEASE(pDevice);

	return hr;
}

/*
* Attempts to get an audio output sink for the specified device index.
* @param[in] deviceIndex: the audio output device to get the sink for.
* @param[out] ppAudioSink: if successful this parameter will be set with
*  the output sink.
* @@Returns S_OK if successful or an error code if not.
*/
HRESULT GetAudioOutputDevice(UINT deviceIndex, IMFMediaSink** ppAudioSink)
{
	HRESULT hr = S_OK;

	IMMDeviceEnumerator* pEnum = NULL;      // Audio device enumerator.
	IMMDeviceCollection* pDevices = NULL;   // Audio device collection.
	IMMDevice* pDevice = NULL;              // An audio device.
	IMFAttributes* pAttributes = NULL;      // Attribute store.
	LPWSTR wstrID = NULL;                   // Device ID.
	UINT deviceCount = 0;

	// Create the device enumerator.
	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL,
		CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pEnum
	);

	// Enumerate the rendering devices.
	hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
	CHECK_HR(hr, "Failed to enumerate audio end points.");

	hr = pDevices->GetCount(&deviceCount);
	CHECK_HR(hr, "Failed to get audio end points count.");

	if (deviceIndex >= deviceCount) {
		printf("The audio output device index was invalid.\n");
		hr = E_INVALIDARG;
	}
	else {
		hr = pDevices->Item(deviceIndex, &pDevice);
		CHECK_HR(hr, "Failed to get device for ID.");

		hr = pDevice->GetId(&wstrID);
		CHECK_HR(hr, "Failed to get name for device.");

		std::wcout << "Audio output device for index " << deviceIndex << ": " << wstrID << "." << std::endl;

		// Create an attribute store and set the device ID attribute.
		hr = MFCreateAttributes(&pAttributes, 1);
		CHECK_HR(hr, "Failed to create attribute store.");

		hr = pAttributes->SetString(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, wstrID);
		CHECK_HR(hr, "Failed to set endpoint ID attribute.");

		// Create the audio renderer.
		hr = MFCreateAudioRenderer(pAttributes, ppAudioSink);
		CHECK_HR(hr, "Failed to create the audio output sink.");
	}

done:

	CoTaskMemFree(wstrID);
	SAFE_RELEASE(pEnum);
	SAFE_RELEASE(pDevices);
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pAttributes);

	return hr;
}

/**
* Gets a video source reader from a device such as a webcam.
* @param[in] nDevice: the video device index to attempt to get the source reader for.
* @param[out] ppVideoSource: will be set with the source for the reader if successful.
* @param[out] ppVideoReader: will be set with the reader if successful. Set this parameter
*  to nullptr if no reader is required and only the source is needed.
* @@Returns S_OK if successful or an error code if not.
*/
HRESULT GetVideoSourceFromDevice(UINT nDevice, IMFMediaSource** ppVideoSource, IMFSourceReader** ppVideoReader)
{
	UINT32 videoDeviceCount = 0;
	IMFAttributes* videoConfig = NULL;
	IMFActivate** videoDevices = NULL;
	WCHAR* webcamFriendlyName;
	UINT nameLength = 0;
	IMFAttributes* pAttributes = NULL;

	HRESULT hr = S_OK;

	// Get the first available webcam.
	hr = MFCreateAttributes(&videoConfig, 1);
	CHECK_HR(hr, "Error creating video configuration.");

	// Request video capture devices.
	hr = videoConfig->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	CHECK_HR(hr, "Error initialising video configuration object.");

	hr = MFEnumDeviceSources(videoConfig, &videoDevices, &videoDeviceCount);
	CHECK_HR(hr, "Error enumerating video devices.");

	if (nDevice >= videoDeviceCount) {
		printf("The device index of %d was invalid for available device count of %d.\n", nDevice, videoDeviceCount);
		hr = E_INVALIDARG;
	}
	else {
		hr = videoDevices[nDevice]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &webcamFriendlyName, &nameLength);
		CHECK_HR(hr, "Error retrieving video device friendly name.\n");

		wprintf(L"Using webcam: %s\n", webcamFriendlyName);

		hr = videoDevices[nDevice]->ActivateObject(IID_PPV_ARGS(ppVideoSource));
		CHECK_HR(hr, "Error activating video device.");

		CHECK_HR(MFCreateAttributes(&pAttributes, 1),
			"Failed to create attributes.");

		if (ppVideoReader != nullptr) {
			// Adding this attribute creates a video source reader that will handle
			// colour conversion and avoid the need to manually convert between RGB24 and RGB32 etc.
			CHECK_HR(pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1),
				"Failed to set enable video processing attribute.");

			// Create a source reader.
			hr = MFCreateSourceReaderFromMediaSource(
				*ppVideoSource,
				pAttributes,
				ppVideoReader);
			CHECK_HR(hr, "Error creating video source reader.");
		}
	}

done:

	SAFE_RELEASE(videoConfig);
	SAFE_RELEASE(videoDevices);
	SAFE_RELEASE(pAttributes);

	return hr;
}

/**
* Gets an audio or video source reader from a capture device such as a webcam or microphone.
* @param[in] deviceType: the type of capture device to get a source reader for.
* @param[in] nDevice: the capture device index to attempt to get the source reader for.
* @param[out] ppMediaSource: will be set with the source for the reader if successful.
* @param[out] ppVMediaReader: will be set with the reader if successful. Set this parameter
*  to nullptr if no reader is required and only the source is needed.
* @@Returns S_OK if successful or an error code if not.
*/
HRESULT GetSourceFromCaptureDevice(DeviceType deviceType, UINT nDevice, IMFMediaSource** ppMediaSource, IMFSourceReader** ppMediaReader)
{
	UINT32 captureDeviceCount = 0;
	IMFAttributes* pDeviceConfig = NULL;
	IMFActivate** ppCaptureDevices = NULL;
	WCHAR* deviceFriendlyName;
	UINT nameLength = 0;
	IMFAttributes* pAttributes = NULL;

	HRESULT hr = S_OK;

	hr = MFCreateAttributes(&pDeviceConfig, 1);
	CHECK_HR(hr, "Error creating capture device configuration.");

	GUID captureType = (deviceType == DeviceType::Audio) ?
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID :
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID;

	// Request video capture devices.
	hr = pDeviceConfig->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		captureType);
	CHECK_HR(hr, "Error initialising capture device configuration object.");

	hr = MFEnumDeviceSources(pDeviceConfig, &ppCaptureDevices, &captureDeviceCount);
	CHECK_HR(hr, "Error enumerating capture devices.");

	if (nDevice >= captureDeviceCount) {
		printf("The device index of %d was invalid for available device count of %d.\n", nDevice, captureDeviceCount);
		hr = E_INVALIDARG;
	}
	else {
		hr = ppCaptureDevices[nDevice]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &deviceFriendlyName, &nameLength);
		CHECK_HR(hr, "Error retrieving video device friendly name.\n");

		wprintf(L"Capture device friendly name: %s\n", deviceFriendlyName);

		hr = ppCaptureDevices[nDevice]->ActivateObject(IID_PPV_ARGS(ppMediaSource));
		CHECK_HR(hr, "Error activating capture device.");

		// Is a reader required or does the caller only want the source?
		if (ppMediaReader != nullptr) {
			CHECK_HR(MFCreateAttributes(&pAttributes, 1),
				"Failed to create attributes.");

			if (deviceType == DeviceType::Video) {
				// Adding this attribute creates a video source reader that will handle
				// colour conversion and avoid the need to manually convert between RGB24 and RGB32 etc.
				CHECK_HR(pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1),
					"Failed to set enable video processing attribute.");
			}

			// Create a source reader.
			hr = MFCreateSourceReaderFromMediaSource(
				*ppMediaSource,
				pAttributes,
				ppMediaReader);
			CHECK_HR(hr, "Error creating media source reader.");
		}
	}

done:

	SAFE_RELEASE(pDeviceConfig);
	SAFE_RELEASE(ppCaptureDevices);
	SAFE_RELEASE(pAttributes);

	return hr;
}

/**
* Copies a media type attribute from an input media type to an output media type. Useful when setting
* up the video sink and where a number of the video sink input attributes need to be duplicated on the
* video writer attributes.
* @param[in] pSrc: the media attribute the copy of the key is being made from.
* @param[in] pDest: the media attribute the copy of the key is being made to.
* @param[in] key: the media attribute key to copy.
*/
HRESULT CopyAttribute(IMFAttributes* pSrc, IMFAttributes* pDest, const GUID& key)
{
	PROPVARIANT var;
	PropVariantInit(&var);

	HRESULT hr = S_OK;

	hr = pSrc->GetItem(key, &var);
	if (SUCCEEDED(hr))
	{
		hr = pDest->SetItem(key, var);
	}

	PropVariantClear(&var);
	return hr;
}



/**
* Creates a bitmap file and writes to disk.
* @param[in] fileName: the path to save the file at.
* @param[in] width: the width of the bitmap.
* @param[in] height: the height of the bitmap.
* @param[in] bitsPerPixel: colour depth of the bitmap pixels (typically 24 or 32).
* @param[in] bitmapData: a pointer to the bytes containing the bitmap data.
* @param[in] bitmapDataLength: the number of pixels in the bitmap.
*/
unsigned char * buf_out = (unsigned char *)malloc(0);
int frames_stored = 0;


/**
* Calculate the minimum stride from the media type.
* From:
* https://docs.microsoft.com/en-us/windows/win32/medfound/uncompressed-video-buffers
*/
HRESULT GetDefaultStride(IMFMediaType* pType, LONG* plStride)
{
	LONG lStride = 0;

	// Try to get the default stride from the media type.
	HRESULT hr = pType->GetUINT32(MF_MT_DEFAULT_STRIDE, (UINT32*)&lStride);
	if (FAILED(hr))
	{
		// Attribute not set. Try to calculate the default stride.

		GUID subtype = GUID_NULL;

		UINT32 width = 0;
		UINT32 height = 0;

		// Get the subtype and the image size.
		hr = pType->GetGUID(MF_MT_SUBTYPE, &subtype);
		if (FAILED(hr))
		{
			goto done;
		}

		hr = MFGetAttributeSize(pType, MF_MT_FRAME_SIZE, &width, &height);
		if (FAILED(hr))
		{
			goto done;
		}

		hr = MFGetStrideForBitmapInfoHeader(subtype.Data1, width, &lStride);
		if (FAILED(hr))
		{
			goto done;
		}

		// Set the attribute for later reference.
		(void)pType->SetUINT32(MF_MT_DEFAULT_STRIDE, UINT32(lStride));
	}

	if (SUCCEEDED(hr))
	{
		*plStride = lStride;
	}

done:
	return hr;
}

/**
* Dumps the media buffer contents of an IMF sample to a file stream.
* @param[in] pSample: pointer to the media sample to dump the contents from.
* @param[in] pFileStream: pointer to the file stream to write to.
* @@Returns S_OK if successful or an error code if not.
*/
#include <math.h>
float Convert_sRGB_ToLinear(float thesRGBValue) {
	return pow(thesRGBValue, 0.89);
}

int skip_frame=2;
int skip_frames=2;
HRESULT store_frame(IMFSample* pSample, int xres, int yres, int stride)
{
	skip_frame--;
	if (skip_frame)return S_OK;
	skip_frame = skip_frames;
	IMFMediaBuffer* buf = NULL;
	DWORD bufLength;

	HRESULT hr = S_OK;

	hr = pSample->ConvertToContiguousBuffer(&buf);
	CHECK_HR(hr, "ConvertToContiguousBuffer failed.");

	hr = buf->GetCurrentLength(&bufLength);
	CHECK_HR(hr, "Get buffer length failed.");

	printf("Writing sample to capture file sample size %i.\n", bufLength);

	static byte* byteBuffer = NULL;
	static  DWORD buffMaxLen = 0, buffCurrLen = 0;
	buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);
	frames_stored++;
	buf_out = (unsigned char*)realloc(buf_out, frames_stored * 128 * 64);
	if (!buf_out)abort();
	int x, y;
	for (y = 0; y < 64; y++)
		for (x = 0; x < 128; x++)
		{
			int px = (x / 128.0)*(float)xres;
			int py = (y / 64.0)*(float)yres;
			int p = (stride / 4) * py + px;
			p *= 4;

			float c = byteBuffer[p] * 0.1 + byteBuffer[p + 1] * 0.6 + byteBuffer[p] * 0.3;
			c = Convert_sRGB_ToLinear(c/255.0)*255.0;
			if (c > 255)c = 255;
			buf_out[x + y * 128 + 128 * 64 * (frames_stored - 1)] = c;
		}
done:

	SAFE_RELEASE(buf);

	return hr;
}

/**
* Creates a new single buffer media sample.
* @param[in] bufferSize: size of the media buffer to set on the create media sample.
* @param[out] pSample: pointer to the create single buffer media sample.
* @@Returns S_OK if successful or an error code if not.
*/
HRESULT CreateSingleBufferIMFSample(DWORD bufferSize, IMFSample** pSample)
{
	IMFMediaBuffer* pBuffer = NULL;

	HRESULT hr = S_OK;

	hr = MFCreateSample(pSample);
	CHECK_HR(hr, "Failed to create MF sample.");

	// Adds a ref count to the pBuffer object.
	hr = MFCreateMemoryBuffer(bufferSize, &pBuffer);
	CHECK_HR(hr, "Failed to create memory buffer.");

	// Adds another ref count to the pBuffer object.
	hr = (*pSample)->AddBuffer(pBuffer);
	CHECK_HR(hr, "Failed to add sample to buffer.");

done:
	// Leave the single ref count that will be removed when the pSample is released.
	SAFE_RELEASE(pBuffer);
	return hr;
}

/**
* Creates a new media sample and copies the first media buffer from the source to it.
* @param[in] pSrcSample: size of the media buffer to set on the create media sample.
* @param[out] pDstSample: pointer to the media sample created.
* @@Returns S_OK if successful or an error code if not.
*/
HRESULT CreateAndCopySingleBufferIMFSample(IMFSample* pSrcSample, IMFSample** pDstSample)
{
	IMFMediaBuffer* pDstBuffer = NULL;
	DWORD srcBufLength;

	HRESULT hr = S_OK;

	// Gets total length of ALL media buffer samples. We can use here because it's only a
	// single buffer sample copy.
	hr = pSrcSample->GetTotalLength(&srcBufLength);
	CHECK_HR(hr, "Failed to get total length from source buffer.");

	hr = CreateSingleBufferIMFSample(srcBufLength, pDstSample);
	CHECK_HR(hr, "Failed to create new single buffer IMF sample.");

	hr = pSrcSample->CopyAllItems(*pDstSample);
	CHECK_HR(hr, "Failed to copy IMFSample items from src to dst.");

	hr = (*pDstSample)->GetBufferByIndex(0, &pDstBuffer);
	CHECK_HR(hr, "Failed to get buffer from sample.");

	hr = pSrcSample->CopyToBuffer(pDstBuffer);
	CHECK_HR(hr, "Failed to copy IMF media buffer.");

done:
	SAFE_RELEASE(pDstBuffer);
	return hr;
}

/**
* Attempts to get an output sample from an MFT transform.
* @param[in] pTransform: pointer to the media transform to apply.
* @param[out] pOutSample: pointer to the media sample output by the transform. Can be NULL
*  if the transform did not produce one.
* @param[out] transformFlushed: if set to true means the transform format changed and the
*  contents were flushed. Output format of sample most likely changed.
* @@Returns S_OK if successful or an error code if not.
*/
HRESULT GetTransformOutput(IMFTransform* pTransform, IMFSample** pOutSample, BOOL* transformFlushed)
{
	MFT_OUTPUT_STREAM_INFO StreamInfo = { 0 };
	MFT_OUTPUT_DATA_BUFFER outputDataBuffer = { 0 };
	DWORD processOutputStatus = 0;
	IMFMediaType* pChangedOutMediaType = NULL;

	HRESULT hr = S_OK;
	*transformFlushed = FALSE;

	hr = pTransform->GetOutputStreamInfo(0, &StreamInfo);
	CHECK_HR(hr, "Failed to get output stream info from MFT.");

	outputDataBuffer.dwStreamID = 0;
	outputDataBuffer.dwStatus = 0;
	outputDataBuffer.pEvents = NULL;

	if ((StreamInfo.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES) == 0) {
		hr = CreateSingleBufferIMFSample(StreamInfo.cbSize, pOutSample);
		CHECK_HR(hr, "Failed to create new single buffer IMF sample.");
		outputDataBuffer.pSample = *pOutSample;
	}
	{
	auto mftProcessOutput = pTransform->ProcessOutput(0, 1, &outputDataBuffer, &processOutputStatus);

	//printf("Process output result %.2X, MFT status %.2X.\n", mftProcessOutput, processOutputStatus);

	if (mftProcessOutput == S_OK) {
		// Sample is ready and allocated on the transform output buffer.
		*pOutSample = outputDataBuffer.pSample;
	}
	else if (mftProcessOutput == MF_E_TRANSFORM_STREAM_CHANGE) {
		// Format of the input stream has changed. https://docs.microsoft.com/en-us/windows/win32/medfound/handling-stream-changes
		if (outputDataBuffer.dwStatus == MFT_OUTPUT_DATA_BUFFER_FORMAT_CHANGE) {
			printf("MFT stream changed.\n");

			hr = pTransform->GetOutputAvailableType(0, 0, &pChangedOutMediaType);
			CHECK_HR(hr, "Failed to get the MFT output media type after a stream change.");

			std::cout << "MFT output media type: " << GetMediaTypeDescription(pChangedOutMediaType) << std::endl << std::endl;

			hr = pChangedOutMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV);
			CHECK_HR(hr, "Failed to set media sub type.");

			hr = pTransform->SetOutputType(0, pChangedOutMediaType, 0);
			CHECK_HR(hr, "Failed to set new output media type on MFT.");

			hr = pTransform->ProcessMessage(MFT_MESSAGE_COMMAND_FLUSH, NULL);
			CHECK_HR(hr, "Failed to process FLUSH command on MFT.");

			*transformFlushed = TRUE;
		}
		else {
			printf("MFT stream changed but didn't have the data format change flag set. Don't know what to do.\n");
			hr = E_NOTIMPL;
		}

		SAFE_RELEASE(pOutSample);
		*pOutSample = NULL;
	}
	else if (mftProcessOutput == MF_E_TRANSFORM_NEED_MORE_INPUT) {
		// More input is not an error condition but it means the allocated output sample is empty.
		SAFE_RELEASE(pOutSample);
		*pOutSample = NULL;
		hr = MF_E_TRANSFORM_NEED_MORE_INPUT;
	}
	else {
		printf("MFT ProcessOutput error result %.2X, MFT status %.2X.\n", mftProcessOutput, processOutputStatus);
		hr = mftProcessOutput;
		SAFE_RELEASE(pOutSample);
		*pOutSample = NULL;
	}
}
done:

	SAFE_RELEASE(pChangedOutMediaType);

	return hr;
}

/**
* Gets the hex string representation of a byte array.
* @param[in] start: pointer to the start of the byte array.
* @param[in] length: length of the byte array.
* @@Returns a null terminated char array.
*/
unsigned char* HexStr(const uint8_t* start, size_t length)
{
	// Each byte requires 2 characters. Add one additional byte to hold the null termination char.
	unsigned char* hexStr = (unsigned char*)calloc((size_t)(length * 2 + 1), 1);

	static const char hexmap[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
	  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	int posn = 0;
	for (int i = 0; i < length; i++)
	{
		unsigned char val = (unsigned char)(*(start + i));
		hexStr[posn] = (hexmap[val >> 4]);
		hexStr[posn + 1] = (hexmap[val & 15]);
		posn += 2;
	}

	return hexStr;
}

class MediaEventHandler : IMFAsyncCallback
{
public:

	HRESULT STDMETHODCALLTYPE Invoke(IMFAsyncResult* pAsyncResult)
	{
		HRESULT hr = S_OK;
		IMFMediaEvent* pEvent = NULL;
		MediaEventType meType = MEUnknown;
		BOOL fGetAnotherEvent = TRUE;
		HRESULT hrStatus = S_OK;
		IMFMediaEventGenerator* pEventGenerator = NULL;

		hr = pAsyncResult->GetState((IUnknown**)&pEventGenerator);
		if (!SUCCEEDED(hr))
		{
			printf("Failed to get media event generator from async state.\n");
		}

		// Get the event from the event queue.
		// Assume that m_pEventGenerator is a valid pointer to the
		// event generator's IMFMediaEventGenerator interface.
		hr = pEventGenerator->EndGetEvent(pAsyncResult, &pEvent);

		// Get the event type.
		if (SUCCEEDED(hr))
		{
			hr = pEvent->GetType(&meType);
			//printf("Media event type %d.\n", meType);
		}

		// Get the event status. If the operation that triggered the event 
		// did not succeed, the status is a failure code.
		if (SUCCEEDED(hr))
		{
			hr = pEvent->GetStatus(&hrStatus);
		}

		if (SUCCEEDED(hr))
		{
			// TODO: Handle the event.
		}

		// If not finished, request another event.
		// Pass in a pointer to this instance of the application's
		// CEventHandler class, which implements the callback.
		if (fGetAnotherEvent)
		{
			hr = pEventGenerator->BeginGetEvent(this, pEventGenerator);
		}

		SAFE_RELEASE(pEvent);
		return hr;
	}

	HRESULT STDMETHODCALLTYPE GetParameters(
		DWORD* pdwFlags,
		DWORD* pdwQueue
	)
	{
		pdwFlags = 0;
		pdwQueue = 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject)
	{
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return 0;
	}

	ULONG STDMETHODCALLTYPE Release(void)
	{
		return 0;
	}
};

#include <stdio.h>
#include <tchar.h>
#include <evr.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <wmcodecdsp.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

//#define SOURCE_FILENAME L"../MediaFiles/big_buck_bunny.mp4"
//#define CAPTURE_FILENAME "c:/dev/rawframes.bin"
int _tmain(int argc, _TCHAR* argv[])


{
	//  std::ofstream outputBuffer(CAPTURE_FILENAME, std::ios::out | std::ios::binary);
	CoInitialize(0);
//	CHECK_HR(CoInitializeEx(NULL, COINIT_MULTITHREADED),
	//	"COM initialisation failed.");

	CHECK_HR(MFStartup(MF_VERSION),
		"Media Foundation initialisation failed.");

	static 	IMFSourceResolver* pSourceResolver = NULL;
	static IUnknown* uSource = NULL;
	static IMFMediaSource* mediaFileSource = NULL;
	static IMFAttributes* pVideoReaderAttributes = NULL;
	static IMFSourceReader* pSourceReader = NULL;
	static IMFMediaType* pReaderOutputType = NULL, *pFirstOutputType = NULL;
	static MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;
	static DWORD mftStatus = 0;

	// Need the color converter DSP for conversions between YUV, RGB etc.
	// Lots of examples use this explicit colour converter load. Seems
	// like most cases it gets loaded automatically.
	CHECK_HR(MFTRegisterLocalByCLSID(
		CLSID_CResizerDMO

		,
		MFT_CATEGORY_VIDEO_PROCESSOR,
		L"",
		MFT_ENUM_FLAG_SYNCMFT,
		0,
		NULL,
		0,
		NULL),
		"Error registering colour converter DSP.");

	// Set up the reader for the file.
	CHECK_HR(MFCreateSourceResolver(&pSourceResolver),
		"MFCreateSourceResolver failed.");
	wchar_t wtext[256];
	//mbstowcs(wtext, argv[1], strlen(argv[1]) + 1);//Plus null
	//LPWSTR ptr = wtext;
	//printf("blup!");
	CHECK_HR(pSourceResolver->CreateObjectFromURL(
		argv[1],		        // URL of the source.
		MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
		NULL,                       // Optional property store.
		&ObjectType,				        // Receives the created object type. 
		&uSource					          // Receives a pointer to the media source.
	),
		"Failed to create media source resolver for file.");

	CHECK_HR(uSource->QueryInterface(IID_PPV_ARGS(&mediaFileSource)),
		"Failed to create media file source.");

	CHECK_HR(MFCreateAttributes(&pVideoReaderAttributes, 2),
		"Failed to create attributes object for video reader.");

	CHECK_HR(pVideoReaderAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID),
		"Failed to set dev source attribute type for reader config.");

	CHECK_HR(pVideoReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1),
		"Failed to set enable video processing attribute type for reader config.");

	CHECK_HR(pVideoReaderAttributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV),
		"Failed to set media sub type on source reader output media type.");

	CHECK_HR(MFCreateSourceReaderFromMediaSource(mediaFileSource, pVideoReaderAttributes, &pSourceReader),
		"Error creating media source reader.");

	CHECK_HR(MFCreateMediaType(&pReaderOutputType), "Failed to create source reader output media type.");

	CHECK_HR(pReaderOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video), "Failed to set major type on source reader output media type.");
	CHECK_HR(pReaderOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32), "Failed to set media sub type on source reader output media type.");

	CHECK_HR(pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pReaderOutputType),
		"Failed to set output media type on source reader.");

	CHECK_HR(pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pFirstOutputType),
		"Error retrieving current media type from first video stream.");

	//  MFSetAttributeSize(pReaderOutputType, MF_MT_FRAME_SIZE, 128, 64);
	//  MFSetAttributeSize(pFirstOutputType, MF_MT_FRAME_SIZE, 128, 64);

	 // std::cout << "Source reader output media type: " << GetMediaTypeDescription(pFirstOutputType) << std::endl << std::endl;
	 // std::cout << "Sourcexxx reader output media type: " << GetMediaTypeDescription(pReaderOutputType) << std::endl << std::endl;

	  //getchar();
	  // Start processing frames.
	static 	IMFSample* pVideoSample = NULL;
	static DWORD streamIndex, flags;
	static  LONGLONG llVideoTimeStamp = 0, llSampleDuration = 0;
	static  int sampleCount = 0;
	static  DWORD sampleFlags = 0;

	while (1)
	{
		CHECK_HR(pSourceReader->ReadSample(
			MF_SOURCE_READER_FIRST_VIDEO_STREAM,
			0,                              // Flags.
			&streamIndex,                   // Receives the actual stream index. 
			&flags,                         // Receives status flags.
			&llVideoTimeStamp,              // Receives the time stamp.
			&pVideoSample                    // Receives the sample or NULL.
		), "Error reading video sample.");
		//	pVideoSample->ConvertToContiguousBuffer();
		if (flags & MF_SOURCE_READERF_STREAMTICK)
		{
			//printf("\tStream tick.\n");
		}
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			//printf("\tEnd of stream.\n");
			break;
		}
		if (flags & MF_SOURCE_READERF_NEWSTREAM)
		{
			//printf("\tNew stream.\n");
			break;
		}
		if (flags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
		{
			//printf("\tNative type changed.\n");
			break;
		}
		if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
		{
			//printf("\tCurrent type changed.\n");
			//break;

		}

		if (pVideoSample)
		{
			//printf("Processing sample %i.\n", sampleCount);

			CHECK_HR(pVideoSample->SetSampleTime(llVideoTimeStamp), "Error setting the video sample time.");
			CHECK_HR(pVideoSample->GetSampleDuration(&llSampleDuration), "Error getting video sample duration.");
			CHECK_HR(pVideoSample->GetSampleFlags(&sampleFlags), "Error getting sample flags.");

			UINT64 videoResolution;
			UINT32 stride;

			CHECK_HR(pFirstOutputType->GetUINT64(MF_MT_FRAME_SIZE, &videoResolution), "Failed to get new video resolution.");
			CHECK_HR(pFirstOutputType->GetUINT32(MF_MT_DEFAULT_STRIDE, &stride), "Failed to get the new stride.");

			int yres = (int)(videoResolution & 0xffffffff), xres = (videoResolution >> 32);
			//printf("yres: %d xres: %d\r\n", yres, xres);
			//printf("stride:%d", stride);
			//printf("Sample count %d, Sample flags %d, sample duration %I64d, sample time %I64d\n", sampleCount, sampleFlags, llSampleDuration, llVideoTimeStamp);

			store_frame(pVideoSample, xres, yres, stride);

			sampleCount++;
		}

		SAFE_RELEASE(pVideoSample);
	}


done:

	SAFE_RELEASE(pSourceResolver);
	SAFE_RELEASE(uSource);
	SAFE_RELEASE(mediaFileSource);
	SAFE_RELEASE(pVideoReaderAttributes);
	SAFE_RELEASE(pSourceReader);
	SAFE_RELEASE(pReaderOutputType);
	SAFE_RELEASE(pFirstOutputType);
	main_encoder((char*)"encoded_video");
	return 0;
}


#define MAX_TILES 128


#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <Vfw.h>
#include <mmsystem.h>
#include <atlimage.h>
CImage *img;
#pragma comment(lib, "vfw32.lib")
FILE *file_charsets = 0;
FILE *file_chunks = 0;

unsigned char GetGrayscale(COLORREF cr)
{
	unsigned char byColor;

	byColor =
		(GetRValue(cr) * 0.30) +
		(GetGValue(cr) * 0.59) +
		(GetBValue(cr) * 0.11);
	return byColor;
}

int img_xres = 128;
int img_yres = 64 * 1900;

static unsigned char blue_noise[512 * 512];

unsigned short rnd()
{
	unsigned int r = 0;
	rand_s(&r);
	return r;
}


typedef struct vq
{
	int a[8 * 8];
	double sum[8 * 8];
	double error_cum;
	int usage;


	double distance(unsigned char *block)
	{
		double d = 0;
		int i;
		for (i = 0; i < 8 * 8; i++)d += (a[i] - block[i])*(a[i] - block[i]);
		return d;
	}
	void put(unsigned char *img, int xp, int yp, int xres, int idx)
	{
		int		map2[8 * 8] = {
			50,10,56,0,48,8,58,3,29,34,25,37,30,33,24,38,5,52,13,61,4,54,12,62,42,20,45,21,41,17,47,16,9,57,1,49,7,59,27,51,39,26,35,28,32,36,2,31,18,44,22,40,19,46,23,43,63,6,53,11,60,15,55,14,
		}
		;
		int map[8 * 8] = {
0, 32, 8, 40, 2, 34, 10, 42, /* 8x8 Bayer ordered dithering */
48, 16, 56, 24, 50, 18, 58, 26, /* pattern. Each input pixel */
12, 44, 4, 36, 14, 46, 6, 38, /* is scaled to the 0..63 range */
60, 28, 52, 20, 62, 30, 54, 22, /* before looking in this table */
 3, 35, 11, 43, 1, 33, 9, 41, /* to determine the action. */
51, 19, 59, 27, 49, 17, 57, 25,
15, 47, 7, 39, 13, 45, 5, 37,
63, 31, 55, 23, 61, 29, 53, 21
		};
		int i = 0;
		int x, y;
		double err = 0;
		int r;
		int idx_x = idx & 0xf;
		int idx_y = idx >> 4;
		idx_x *= 8;
		idx_y *= 8;
		err = 0;
		for (i = 0; i < 64; i++)err += a[i];
		err /= 64;
		err = 0;
		int yy;
		if (0)
			for (yy = 0; yy < 8; yy += 2)
			{
				err = 0;
				for (int t = 0; t < 4; t++)
					for (y = yy; y < yy + 2; y++)

					{
						int xs = 0;
						int xe = 8;
						int xd = 1;
						if (y & 1)xs = 7, xe = -1, xd = -1;
						for (x = xs; x != xe; x += xd)
						{

							int c = a[x + y * 8];
							err += c;

							if (err > 127)err -= 255, c = 255; else c = 0;
							c = a[x + y * 8];

							img[(x + xp) * 3 + (y + yp) * xres * 3] = c,// a[i],
								img[(x + xp) * 3 + (y + yp) * xres * 3 + 1] = c,// a[i],
								img[(x + xp) * 3 + (y + yp) * xres * 3 + 2] = c;// a[i], i++;

						}
					}
			}
		i = 0;

		if (1)
			for (y = 0; y < 8; y++)
				for (x = 0; x < 8; x++)
				{
					int c = a[i]>>2;
					//if (c > blue_noise[x + idx_x + 512 * (y + idx_y)])c = 255; else c = 0;
					//				c = a[i]/4;
								//	c = a[i];

					if (c > map2[x+8*y])c = 255; else c = 0;
									//if (c > (rand() & 0xff))err=255-c,c = 255; else err=c, c = 0;
					img[(x + xp) * 3 + (y + yp) * xres * 3] = c,// a[i],
						img[(x + xp) * 3 + (y + yp) * xres * 3 + 1] = c,// a[i],
						img[(x + xp) * 3 + (y + yp) * xres * 3 + 2] = c, i++;// a[i], i++;

				}

	}

	void dump(int idx, FILE *f, int use_map2)
	{
		int i = 0;
		int x, y;
		double err = 0;
		int r;
		err = 0;
		for (i = 0; i < 64; i++)err += a[i];
		err /= 64;
		err = 0;
		int yy;

		int		map[8 * 8] = {

	36,22,51,63,15,28,40,5,46,9,31,6,37,8,18,62,16,1,41,24,42,58,11,50,39,20,44,60,26,55,45,7,48,64,54,30,13,38,21,29,25,57,17,47,23,32,43,3,14,35,52,2,49,59,10,53,27,56,12,34,4,19,33,61,
//			60,7,41,16,46,11,50,29,17,47,25,55,27,62,20,44,31,10,64,35,6,38,54,5,56,37,51,14,48,24,12,42,18,3,21,33,1,59,32,63,49,28,58,45,19,40,8,26,13,39,9,30,52,15,57,43,34,53,22,61,4,36,23,2,
			/*

			62,35,2,42,4,46,34,3,
12,21,56,22,53,29,16,47,
51,31,43,11,36,7,58,23,
41,8,25,61,20,49,33,1,
19,55,38,0,45,9,27,60,
44,5,28,52,17,57,37,13,
18,59,32,10,40,30,6,50,
39,14,48,24,63,15,54,26,
*/
		}
		;
		int map2[8 * 8] = {
	19,48,2,50,20,40,22,53,36,5,35,12,62,9,58,26,60,28,59,39,18,45,32,7,13,52,31,3,51,30,1,43,49,21,25,55,14,24,42,17,4,56,46,6,38,63,8,33,27,37,11,57,16,34,29,61,44,10,41,23,47,0,54,15,
		};


		if (1)
			for (x = 0; x < 8; x++)
			{
				int d = 0;
				for (y = 0; y < 8; y++)
				{
					int c = a[x + y * 8];
					if (!use_map2)
					{
						if ((c/3.8) > map[x + y * 8])c = 255; else c = 0;
					}
					else
					{
						if ((c/3.8) >( map2[x + y * 8]))c = 255; else c = 0;
					}
					//				c = a[i]/4;
								//	c = a[i];

						//			if (c > map2[x+8*y])c = 255; else c = 0;
									//if (c > (rand() & 0xff))err=255-c,c = 255; else err=c, c = 0;

					if (c)d |= 1 << y;
				}
				fprintf(f, "%d,", d);
			}
	}

	void add(unsigned char *block)
	{
		int i;
		for (i = 0; i < 8 * 8; i++)sum[i] += block[i];
		usage++;

	}
	void update()
	{
		if (!usage)
		{
			clear();
			return;
		}
		for (int i = 0; i < 8 * 8; i++)a[i] = 0.5 + sum[i] / usage, sum[i] = 0;
		usage = 0;
	}
	void clear()
	{
		for (int i = 0; i < 8 * 8; i++)sum[i] = 0;
		usage = 0;
	}
	void randomize()
	{
		for (int i = 0; i < 8 * 8; i++)a[i] += rnd() % 3 - 1;
	}
};
vq v[256];

void get_block(unsigned char *img, int xres, int xp, int yp, unsigned char *out)
{
	int x, y;
	for (y = 0; y < 8; y++)
		for (x = 0; x < 8; x++)
		{
			*out = img[x + xp + (y + yp)*xres];
			out++;
		}
}


int vq_nr = 1;

int find_best(unsigned char *block, double &e)
{
	double err = v[0].distance(block);
	int best = 0;
	int i;

	//	#pragma loop(hint_parallel(4))

	for (i = 0; i < vq_nr; i++)
	{

		double err2 = v[i].distance(block);
		if (err2 < err)err = err2, best = i;
	}
	e = err;
	return best;
}
void
display_img(unsigned char *img, int xr, int yr)
{
	HDC dc = GetDC(GetConsoleWindow());

	BITMAPINFO info;
	ZeroMemory(&info, sizeof(BITMAPINFO));
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biWidth = xr;
	info.bmiHeader.biHeight = -yr;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biSizeImage = 0;
	info.bmiHeader.biCompression = BI_RGB;

	StretchDIBits(dc, 512, 0, xr, yr, 0, 0, xr, yr, img, &info, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(GetConsoleWindow(), dc);

}

int dump = 0;
static unsigned char * data;// [2024 * 1024 * 8];
static unsigned char v_out[1024 * 1024];
int v_out_pos = 0;

void write_bytes(FILE *f, unsigned char *b, int l)
{
	int i;
	for (i = 0; i < l; i++)fprintf(f, "%d,", b[i]);
}

void compress_video(int frames_chunk)
{
	int b = 128;
	FILE *f = file_chunks;// fopen("/dev/full_apple_vid.h", "wb");
	write_bytes(f, v_out, 128);
	for (int frame = 1; frame < frames_chunk; frame++)
	{
		fprintf(f, "\r\n//Frame:%d bytes before:%d\r\n", frame, b);
		int i;
		int eq = 0;

//		fprintf(f, "\r\n//");

//		for (i = 0; i < 128; i++)fprintf(f, "%d ", v_out[i + frame * 128]);
//		fprintf(f, "\r\n");

		for (i = 0; i < 128; i++)
		{
			if (v_out[frame * 128 + i] == v_out[(frame - 1) * 128 + i])
			{
				//fprintf(f, "128,");
				eq++;
			}
			else
			{
				if (eq)
				{
					fprintf(f, "%d,", eq + 127);
					eq = 0;
					b++;
				}
				fprintf(f, "%d,", v_out[frame * 128 + i]);
				b++;
			}
		}
		if (eq)
		{
			fprintf(f, "\r\n%d,/*skip:%d last*/\r\n", eq + 127, eq);
			eq = 0;
			b++;
		}
	}
	fprintf(f, "//b:%d\r\n};\r\n", b);

}

static unsigned char *d_out;// [1024 * 3 * 1024 * 8];
int main_encoder(char *fname)
{
	/*
		if (argc != 2)
		{
			printf("Video_Encode file.png");
			exit(0);
		}
		img = new CImage();
		int res = img->Load(argv[1]);
	/*	if (!res)
		{
			printf("file not found\r\n");
			exit(0);
		}*/
		//int depth = img->GetBPP();
	//	img_xres = img->GetWidth();
		//img_yres = img->GetHeight();
	//	printf("xres (should be 128)  %d yres %d", img_xres, img_yres);
	printf("main encoding step\r\n");
	file_charsets = fopen("video_charsets.h", "wb");
	file_chunks = fopen("video_chunks.h", "wb");

	int chunk = 0;
	int frames_chunk = 200;

	int frame_start = 0;

	data = buf_out;// (unsigned char *)malloc(img_xres*img_yres);

	img_xres = 128;
	img_yres = frames_stored * 64;

	d_out = (unsigned char *)malloc(img_xres*img_yres * 3);

/*
	FILE*	f = fopen("/dev/blue_noise512x512.raw", "rb");
	if (!f)
	{
		printf("blue noise texture missing\r\n");
		exit(0);
	}
	fread(blue_noise, 512, 512, f);
	*/
	//	FILE *f1=0,*f2=0;
	int frames = frames_stored;
	//int chunk = 0;

anfang:

	static int ccc = 100;
	v_out_pos = 0;
	int x, y, i, c = 4;
	dump = 0;

	int full_c = 0;
	fprintf(file_chunks, "static const u8 chunk_%0d[] __attribute__((progmem))={", chunk);
	chunk++;
	vq_nr = 1;
	memset(v, 0, sizeof(v));
	printf("Chunk!\r\n!");
	ccc = 100;
	for (;;)
	{
		int frame_end = (frames_chunk + frame_start);
		if (frame_end > frames)frame_end = frames;
		char tmp[256];

		double e_a = 0;
		for (y = 64 * frame_start; y < 64 * (frame_end); y += 8)
			for (x = 0; x < img_xres; x += 8)
			{
				double e = 0;
				unsigned char block[8 * 8];
				get_block(data, img_xres, x, y, block);
				int b = find_best(block, e);
				v[b].error_cum += e;
				v[b].add(block);
				e_a += e;
				if (dump)
				{
					v_out[v_out_pos] = b;
					v_out_pos++;
				}
				v[b].put(d_out, x, y, img_xres, b);
			}

		if (dump)
		{
			fprintf(file_charsets, "\r\n// charset A:\r\n");
			for (i = 0; i < MAX_TILES; i++)v[i].dump(i, file_charsets, 0);
			/*
			fprintf(file_charsets, "\r\n// charset B:\r\n");
			for (i = 0; i < MAX_TILES; i++)v[i].dump(i, file_charsets, 1);
			*/
			// temporal dithering:
//			for (i = 0; i < MAX_TILES; i++)v[i].dump(i, file_charsets, 0xf);

		}

		double e_max = v[0].error_cum / v[0].usage;
		int worst = 0;

		for (i = 0; i < vq_nr; i++)if (v[i].usage)
			if (v[i].error_cum / v[i].usage > e_max)
			{
				e_max = v[i].error_cum / v[i].usage;
				worst = i;
			}
		//		display_img(d_out, img_xres, img_yres);
		if (vq_nr < MAX_TILES / 2 - 1)
			for (i = vq_nr; i < vq_nr * 2; i++)v[i] = v[i - vq_nr];
		int unused = 0;
		for (i = 0; i < vq_nr; i++)
		{
			if ((!unused) && (v[i].usage < 1))//&&(!(rnd()&0xff)))
			{
				ccc--;
				if (ccc > 0)
					c = 25;
				unused++;
				if ((rnd() & 0xf) == 0)worst = rnd() % vq_nr;
				v[i].clear();
				v[i] = v[worst];

				v[i].randomize();
				//	printf("%d,", v[i].usage);
			}
			else
				v[i].update();

		}
		printf("%d   %f         \r", unused, e_a);

		c--;
		if (dump)
		{
			fprintf(file_charsets, "\r\n\r\n//-----------\r\n");
			compress_video(frames_chunk);
			frame_start += frames_chunk;
			if (frame_start > frames)
			{
				fclose(file_charsets);
				fclose(file_chunks);
				exit(0);
			}
			dump = 0;
			goto anfang;
		}
		if (!c)
		{
			//if(vq_nr==1) 
			if (vq_nr != MAX_TILES)
				for (i = vq_nr; i < vq_nr * 2; i++)v[i].randomize();// = v[i - vq_nr];

			c = 25;
			printf("\r\n%d %d\r\n", vq_nr, unused);
			if (vq_nr == MAX_TILES)full_c++;
			if (full_c == 10)dump = 1;
			if (vq_nr < MAX_TILES - 1)vq_nr *= 2;
		}
	}
}
