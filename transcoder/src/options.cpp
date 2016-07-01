/*
 * options.cpp
 *
 *  Created on: Jun 30, 2016
 *      Author: Jason
 */

#include "options.h"
#include "media_codec.h"

Options::Options()
{
	buildoptionset();
	for(unsigned int i = 0; i < MAX_OPTION; ++i)
	{
		m_selectedoption[i] = ORIGINAL_FEATURE;
	}
}
Options::~Options()
{
	for(unsigned int i = 0; i < MAX_OPTION; ++i)
	{
		m_optionsets[i].clear();
	}
}

void Options::ResetOptions()
{
	for(unsigned int i = 0; i < MAX_OPTION; ++i)
	{
		m_selectedoption[i] = ORIGINAL_FEATURE;
	}
}

std::vector<TranscodingOption>& Options::GetOptionSet(TranscodingOptionType type)
{
	return m_optionsets[type];
}


void Options::SelectOption(TranscodingOptionType type, unsigned int option)
{
	m_selectedoption[type] = option;
}
unsigned int Options::GetSelectedOption(TranscodingOptionType type)
{
	return m_selectedoption[type];
}

const char* Options::GetOptionName(TranscodingOptionType type, unsigned int id)
{
	for(unsigned int i = 0; i < m_optionsets[type].size(); ++i)
	{
		if(m_optionsets[type][i].id == id)
		{
			return m_optionsets[type][i].name.c_str();
		}
	}
	return "invalid selection";
}

void Options::buildoptionset()
{
	buildoptionset_videocodec(m_optionsets[VIDEO_CODEC_OPTION]);
	buildoptionset_audiocodec(m_optionsets[AUDIO_CODEC_OPTION]);
	buildoptionset_resolution(m_optionsets[RESOLUTION_OPTION]);
}

void Options::buildoptionset_videocodec(std::vector<TranscodingOption>& optionset)
{
	optionset.push_back(TranscodingOption("Original", ORIGINAL_FEATURE, "images/video.png"));
	optionset.push_back(TranscodingOption("H.264", MEDIACODEC_H264, "images/video.png"));
	optionset.push_back(TranscodingOption("MPEG4", MEDIACODEC_MPEG4, "images/video.png"));
	optionset.push_back(TranscodingOption("HEVC", MEDIACODEC_HEVC, "images/video.png"));
	optionset.push_back(TranscodingOption("VC1", MEDIACODEC_VC1, "images/video.png"));
}

void Options::buildoptionset_audiocodec(std::vector<TranscodingOption>& optionset)
{
	optionset.push_back(TranscodingOption("Original", ORIGINAL_FEATURE, "images/audio.png"));
	optionset.push_back(TranscodingOption("AAC-LC", MEDIACODEC_AAC_LC, "images/audio.png"));
	optionset.push_back(TranscodingOption("MP3", MEDIACODEC_MP3, "images/audio.png"));
	optionset.push_back(TranscodingOption("FLAC", MEDIACODEC_FLAC, "images/audio.png"));
	optionset.push_back(TranscodingOption("WMA Professional", MEDIACODEC_WMAPRO, "images/audio.png"));
}



void Options::buildoptionset_resolution(std::vector<TranscodingOption>& optionset)
{
	optionset.push_back(TranscodingOption("Original", ORIGINAL_FEATURE, "images/resolution.png"));
	optionset.push_back(TranscodingOption("1280 X 720", RATIO_16_9|MULTI_080, "images/resolution.png"));
	optionset.push_back(TranscodingOption("960 X 480", RATIO_16_9|MULTI_060, "images/resolution.png"));
	optionset.push_back(TranscodingOption("320 X 160", RATIO_16_9|MULTI_020, "images/resolution.png"));
	optionset.push_back(TranscodingOption("1280 X 960", RATIO_4_3|MULTI_320, "images/resolution.png"));
	optionset.push_back(TranscodingOption("640 X 480", RATIO_4_3|MULTI_160, "images/resolution.png"));
	optionset.push_back(TranscodingOption("320 X 240", RATIO_4_3|MULTI_080, "images/resolution.png"));
}


