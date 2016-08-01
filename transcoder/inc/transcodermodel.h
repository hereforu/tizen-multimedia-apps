/*
 * transcodermodel.h
 *
 *  Created on: Jun 29, 2016
 *      Author: Jason
 */

#ifndef TRANSCODERMODEL_H_
#define TRANSCODERMODEL_H_

#include <string>
#include "common/model.h"
#include "common/mediacontentcontroller.h"
#include "options.h"



class TranscoderModel : public Model
{
public:
	TranscoderModel();
	~TranscoderModel();

	void RebuildContentsList();
	const std::vector<MediaContentItem>& GetContentsList();
	void SetSelectedContentIndex(int index);
	const MediaContentItem& GetSelectedContent();

	void SelectTypeofOptions(TranscodingOptionType type);
	TranscodingOptionType GetSelectedTypeofOptions();
	std::vector<TranscodingOption>& GetOptionSet(TranscodingOptionType type);
	void SelectOption(TranscodingOptionType type, unsigned int option);
	unsigned int GetSelectedOption(TranscodingOptionType type);
	const char* GetOptionName(TranscodingOptionType type, unsigned int id);
	void ResetOptions();

protected:
	void creatspecifics();
	void destroyspecifics();
	void buildcontentslist();

private:
	int m_selectedcontentindex;
	std::vector<MediaContentItem> m_contents;
	Options m_options;
	TranscodingOptionType m_selectedoptiontype;
};




#endif /* TRANSCODERMODEL_H_ */
