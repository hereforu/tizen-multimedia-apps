/*
 * options.h
 *
 *  Created on: Jun 30, 2016
 *      Author: Jason
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <vector>
#include <string>

typedef enum
{
	RATIO_16_9 = 0x1000,
	RATIO_4_3 = 0x2000,

	MULTI_020 = 0x0001,
	MULTI_060 = 0x0002,
	MULTI_080 = 0x0004,
	MULTI_160 = 0x0008,
	MULTI_320 = 0x0010
}ResolutionType;

typedef struct _TranscodingOption
{
	_TranscodingOption(const char* _name, unsigned int _id, const char* _icon)
	:name(_name), id(_id)
	{
		if(_icon)
			icon = _icon;
	}
	std::string name;
	unsigned int id;
	std::string icon;
}TranscodingOption;
typedef enum _TranscodingOptionType
{
	VIDEO_CODEC_OPTION = 0,
	AUDIO_CODEC_OPTION,
	RESOLUTION_OPTION,
	MAX_OPTION
}TranscodingOptionType;

#define ORIGINAL_FEATURE 0

class Options
{
public:

	Options();
	~Options();

	std::vector<TranscodingOption>& GetOptionSet(TranscodingOptionType type);
	void SelectOption(TranscodingOptionType type, unsigned int option);
	unsigned int GetSelectedOption(TranscodingOptionType type);
	void ResetOptions();
	const char* GetOptionName(TranscodingOptionType type, unsigned int id);

private:
	void buildoptionset();
	void buildoptionset_videocodec(std::vector<TranscodingOption>& optionset);
	void buildoptionset_audiocodec(std::vector<TranscodingOption>& optionset);
	void buildoptionset_resolution(std::vector<TranscodingOption>& optionset);

private:
	std::vector<TranscodingOption> m_optionsets[MAX_OPTION];
	unsigned int m_selectedoption[MAX_OPTION];
};


#endif /* OPTIONS_H_ */
