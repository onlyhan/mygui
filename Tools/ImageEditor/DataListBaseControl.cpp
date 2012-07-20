/*!
	@file
	@author		Albert Semenov
	@date		07/2012
*/

#include "Precompiled.h"
#include "DataListBaseControl.h"
#include "FactoryManager.h"
#include "CommandManager.h"
#include "DialogManager.h"
#include "MessageBoxManager.h"
#include "DataManager.h"
#include "ActionManager.h"
#include "ActionCreateData.h"
#include "ActionCloneData.h"
#include "ActionDestroyData.h"
#include "ActionRenameData.h"
#include "ActionChangePositionData.h"
#include "PropertyUtility.h"
#include "DataUtility.h"

namespace tools
{

	FACTORY_ITEM_ATTRIBUTE(DataListBaseControl)

	DataListBaseControl::DataListBaseControl() :
		mListBoxControl(nullptr)
	{
	}

	DataListBaseControl::~DataListBaseControl()
	{
	}

	void DataListBaseControl::OnInitialise(Control* _parent, MyGUI::Widget* _place, const std::string& _layoutName)
	{
		Control::OnInitialise(_parent, _place, _layoutName);

		const VectorControl& childs = getChilds();
		for (VectorControl::const_iterator child = childs.begin(); child != childs.end(); child ++)
		{
			ListBoxDataControl* list = dynamic_cast<ListBoxDataControl*>(*child);
			if (list != nullptr)
			{
				mListBoxControl = list;
				break;
			}
		}

		if (mListBoxControl != nullptr)
		{
			mListBoxControl->setEnableChangePosition(true);
			mListBoxControl->eventChangePosition.connect(this, &DataListBaseControl::notifyChangePosition);
			mListBoxControl->eventChangeName.connect(this, &DataListBaseControl::notifyChangeName);
		}
	}

	bool DataListBaseControl::checkCommand(bool _result)
	{
		if (_result)
			return false;

		if (DialogManager::getInstance().getAnyDialog())
			return false;

		if (MessageBoxManager::getInstance().hasAny())
			return false;

		return true;
	}

	void DataListBaseControl::commandCreateImageData(const MyGUI::UString& _commandName, bool& _result)
	{
		if (!checkCommand(_result))
			return;

		Data* data = DataUtility::getSelectedDataByType(mParentType);
		if (data != nullptr)
		{
			ActionCreateData* command = new ActionCreateData();
			command->setType(mCurrentType);
			command->setParent(data);
			command->setUniqueProperty(mPropertyForUnique);

			ActionManager::getInstance().doAction(command);
		}

		_result = true;
	}

	void DataListBaseControl::commandCloneImageData(const MyGUI::UString& _commandName, bool& _result)
	{
		if (!checkCommand(_result))
			return;

		Data* data = DataUtility::getSelectedDataByType(mCurrentType);
		if (data != nullptr)
		{
			ActionCloneData* command = new ActionCloneData();
			command->setPrototype(data);
			command->setUniqueProperty(mPropertyForUnique);

			ActionManager::getInstance().doAction(command);
		}

		_result = true;
	}

	void DataListBaseControl::commandDestroyImageData(const MyGUI::UString& _commandName, bool& _result)
	{
		if (!checkCommand(_result))
			return;

		Data* data = DataUtility::getSelectedDataByType(mCurrentType);
		if (data != nullptr)
		{
			ActionDestroyData* command = new ActionDestroyData();
			command->setData(data);
			command->setUniqueProperty(mPropertyForUnique);

			ActionManager::getInstance().doAction(command);
		}

		_result = true;
	}

	void DataListBaseControl::commandRenameImageData(const MyGUI::UString& _commandName, bool& _result)
	{
		if (!checkCommand(_result))
			return;

		if (mListBoxControl != nullptr)
			mListBoxControl->OnRenameData();

		_result = true;
	}

	void DataListBaseControl::setDataInfo(const std::string& _parentType, const std::string& _currentType, const std::string& _propertyName, const std::string& _propertyUnique)
	{
		mParentType = _parentType;
		mCurrentType = _currentType;
		mPropertyForName = _propertyName;
		mPropertyForUnique = _propertyUnique;

		if (mListBoxControl != nullptr)
			mListBoxControl->setDataInfo(mParentType, mPropertyForName, mPropertyForUnique);
	}

	void DataListBaseControl::notifyChangePosition(Data* _data1, Data* _data2)
	{
		ActionChangePositionData* command = new ActionChangePositionData();
		command->setData1(_data1);
		command->setData2(_data2);

		ActionManager::getInstance().doAction(command);
	}

	void DataListBaseControl::notifyChangeName(Data* _data, const std::string& _name)
	{
		PropertyUtility::executeAction(_data->getProperty(mPropertyForName), _name);
	}

}