#ifndef SCLAP_H
#define SCLAP_H

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>

namespace sclap
{
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	const uint8_t UNEXISTED      = 0;
	const uint8_t ARG_STRING     = 1 << 0;
	const uint8_t ARG_NONE       = 1 << 1;
	const uint8_t ARG_INT        = 1 << 2;
	const uint8_t ARG_REAL       = 1 << 3;
	const uint8_t ARG_STRING_VEC = 1 << 4;
	const uint8_t ARG_BOOL_VEC   = 1 << 5;
	const uint8_t ARG_INT_VEC    = 1 << 6;
	const uint8_t ARG_REAL_VEC   = 1 << 7;

	const char OPT_SHORT_NONE = '\0';

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	class OptDescriptor
	{
	public:
		OptDescriptor(const char shortName, const char* longName,
			uint8_t possibleArgumentValues)
			: mShortName(shortName), mLongName(longName),
			mPossibleArgumentValues(possibleArgumentValues)
		{}

		char shortName() const { return mShortName; }
		const std::string& longName() const { return mLongName; }
		uint8_t possibleArgumentValues() const { return mPossibleArgumentValues; }

	private:
		const char mShortName;
		const std::string mLongName;
		const uint8_t mPossibleArgumentValues;
	};

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	class OptDescriptors
	{
	public:
		OptDescriptors(const std::vector<OptDescriptor>& descriptors);
		OptDescriptors() : mDescriptors(), mError(), mOk(true) {}
		OptDescriptors(const OptDescriptors& optDesc)
			: mDescriptors(optDesc.mDescriptors), mOk(optDesc.mOk), mError(optDesc.mError.str())
		{ }

		const OptDescriptor* const operator[](const std::string& opt) const;
		const OptDescriptor* const operator[](char opt) const;

		bool ok() const { return mOk; }
		std::string error() const { return mError.str(); }

		OptDescriptors& operator<<(const OptDescriptor& optDescriptor)
		{
			mDescriptors.push_back(optDescriptor);
			check();
			return *this;
		}

	private:
		std::vector<OptDescriptor> mDescriptors;

		std::stringstream mError;
		bool mOk;

		void check();
	};

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	OptDescriptors::OptDescriptors(const std::vector<OptDescriptor>& descriptors)
		: mDescriptors(descriptors), mError(), mOk(false)
	{
		check();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	void OptDescriptors::check()
	{
		std::vector<char> existingShortNames;
		std::vector<std::string> existinglongNames;
		for (size_t i = 0; i < mDescriptors.size(); ++i)
		{
			if (mDescriptors[i].longName().size() == 1)
			{
				mOk = false;
				mError << "Long option length cant be 1: "
					<< mDescriptors[i].longName()
					<< ".\n";
			}
			for (size_t j = 0; j < existingShortNames.size(); ++j)
			{
				if (mDescriptors[i].shortName() == existingShortNames[j])
				{
					mOk = false;
					mError << "Multiple option descriptors with the same short option: "
						<< existingShortNames[j]
						<< ".\n";
				}
			}
			for (size_t j = 0; j < existinglongNames.size(); ++j)
			{
				if (mDescriptors[i].longName() == existinglongNames[j])
				{
					mOk = false;
					mError << "Multiple option descriptors with the same long option: "
						<< existinglongNames[j]
						<< ".\n";
				}
			}
		}

		mOk = true;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	const OptDescriptor* const OptDescriptors::operator[](const std::string& opt) const
	{
		if (opt.empty()) { return NULL; }

		if (opt.size() == 1)
		{
			return operator[](opt[0]);
		}

		for (size_t i = 0; i < mDescriptors.size(); ++i)
		{
			if (mDescriptors[i].longName() == opt)
			{
				return &mDescriptors[i];
			}
		}

		return NULL;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	const OptDescriptor* const OptDescriptors::operator[](char opt) const
	{
		for (size_t i = 0; i < mDescriptors.size(); ++i)
		{
			const char curOpt = mDescriptors[i].shortName();
			if (curOpt == OPT_SHORT_NONE) continue;
			if (curOpt == opt)
			{
				return &mDescriptors[i];
			}
		}

		return NULL;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	class OptionValue
	{
	public:
		virtual const std::string asString() const { return ""; };
		virtual double asReal() const { return 0; };
		virtual int asInteger() const { return 0; };
		virtual bool asBool() const { return true; };

		virtual const std::vector<std::string> asStringVector() const
		{
			return std::vector<std::string>();
		}
		virtual const std::vector<double> asRealVector() const
		{
			return std::vector<double>();
		}
		virtual const std::vector<int> asIntegerVector() const
		{
			return std::vector<int>();
		}
		virtual const std::vector<bool> asBoolVector() const
		{
			return std::vector<bool>();
		}

		virtual uint8_t type() const = 0 { return 0; }

		operator bool() const { return type(); }

		virtual bool read(int& inOutCurIndex,
			char** inOutCurArgumentStr, int argc, char** inArgv) = 0;
	};

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	namespace hidden
	{
		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool toInteger(const char* s, int& val)
		{
			if (s[0] == '\0' || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

			char* p;
			val = strtol(s, &p, 10);

			return (*p == 0);
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool toDouble(const char* s, double& val)
		{
			char* end = NULL;
			val = strtod(s, &end);
			return end != s && *end == '\0' && val != HUGE_VAL;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool isSingleArgType(uint8_t type)
		{
			return (type <= ARG_REAL) && type;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool readInt(int& inOutCurIndex, char** inOutCurArgumentStr,
			char** inArgv, int &outInteger)
		{
			bool res = hidden::toInteger(*inOutCurArgumentStr, outInteger);
			if (res)
			{
				++inOutCurIndex;
				*inOutCurArgumentStr = inArgv[inOutCurIndex];
			}
			return res;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool readDouble(int& inOutCurIndex, char** inOutCurArgumentStr,
			char** inArgv, double &outDouble)
		{
			bool res = hidden::toDouble(*inOutCurArgumentStr, outDouble);
			if (res)
			{
				++inOutCurIndex;
				*inOutCurArgumentStr = inArgv[inOutCurIndex];
			}
			return res;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool readString(int& inOutCurIndex, char** inOutCurArgumentStr,
			char** inArgv, std::string &outString)
		{
			if (**inOutCurArgumentStr != '\0')
			{
				outString = std::string(*inOutCurArgumentStr);
				++inOutCurIndex;
				*inOutCurArgumentStr = inArgv[inOutCurIndex];
			}
			else
			{
				return false;
			}
			return true;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueUnexisted : public OptionValue
		{
		public:
			virtual bool asBool() const { return false; }

			virtual uint8_t type() const { return UNEXISTED; }

			virtual bool read(int& inOutCurIndex, char** inOutCurArgumentStr,
				int argc, char** inArgv) { return false; }
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueBool : public OptionValue
		{
		public:
			virtual const std::string asString() const { return "true"; };

			virtual uint8_t type() const { return ARG_NONE; }

			virtual bool read(int& inOutCurIndex, char** inOutCurArgumentStr,
				int argc, char** inArgv) { return true; }
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueInt : public OptionValue
		{
		public:
			virtual const std::string asString() const { return std::to_string(mValue); };
			virtual double asReal() const { return mValue; };
			virtual int asInteger() const { return mValue; };
			virtual bool asBool() const { return mValue; };

			virtual const std::vector<std::string> asStringVector() const
			{
				return std::vector<std::string>({ asString() });
			}
			virtual const std::vector<double> asRealVector() const
			{
				return std::vector<double>({ asReal() });
			}
			virtual const std::vector<int> asIntegerVector() const
			{
				return std::vector<int>({ asInteger() });
			}
			virtual const std::vector<bool> asBoolVector() const
			{
				return std::vector<bool>({ asBool() });
			}

			virtual uint8_t type() const { return ARG_INT; }

			virtual bool read(int& inOutCurIndex,
				char** inOutCurArgumentStr, int argc, char** inArgv)
			{
				return readInt(inOutCurIndex, inOutCurArgumentStr, inArgv, mValue);
			}

		private:
			int mValue;
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueReal : public OptionValue
		{
		public:
			virtual const std::string asString() const { return std::to_string(mValue); };
			virtual double asReal() const { return mValue; };

			virtual const std::vector<std::string> asStringVector() const
			{
				return std::vector<std::string>({ asString() });
			}
			virtual const std::vector<double> asRealVector() const
			{
				return std::vector<double>({ asReal() });
			}

			virtual uint8_t type() const { return ARG_REAL; }

			virtual bool read(int& inOutCurIndex,
				char** inOutCurArgumentStr, int argc, char** inArgv)
			{
				return readDouble(inOutCurIndex, inOutCurArgumentStr, inArgv, mValue);
			}

		private:
			double mValue;
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueString : public OptionValue
		{
		public:
			virtual const std::string asString() const { return mValue; };

			virtual const std::vector<std::string> asStringVector() const
			{
				return std::vector<std::string>({ asString() });
			}

			virtual uint8_t type() const { return ARG_STRING; }

			virtual bool read(int& inOutCurIndex,
				char** inOutCurArgumentStr, int argc, char** inArgv)
			{
				return readString(inOutCurIndex, inOutCurArgumentStr, inArgv, mValue);
			}

		private:
			std::string mValue;
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueBoolVector : public OptionValue
		{
		public:
			virtual const std::string asString() const
			{
				std::string valueStr = "";
				for (size_t i = 0; i < mValue.size(); ++i)
				{
					valueStr += mValue[i] ? "true" : "false";
					valueStr += " ";
				}

				valueStr.pop_back();
				return valueStr;
			}
			virtual const std::vector<std::string> asStringVector() const;
			virtual bool asBool() const { return mValue[0]; }
			virtual const std::vector<bool> asBoolVector() { return mValue; }
  
			virtual uint8_t type() const { return ARG_BOOL_VEC; }

			virtual bool read(int& inOutCurIndex,
				char** inOutCurArgumentStr, int argc, char** inArgv);

		private:
			std::vector<bool> mValue;
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool OptionValueBoolVector::read(int& inOutCurIndex, char** inOutCurArgumentStr,
			int argc, char** inArgv)
		{
			int startIndex = inOutCurIndex;
			char** startArgument = inOutCurArgumentStr;

			mValue.clear();

			std::string strArg;
			while (**inOutCurArgumentStr != '\0' 
				&& **inOutCurArgumentStr != '-' 
				&& inOutCurIndex < argc)
			{
				if (readString(inOutCurIndex, inOutCurArgumentStr, inArgv, strArg))
				{
					if (strArg == "True" || strArg == "true")
					{
						mValue.push_back(true);
					}
					else if (strArg == "False" || strArg == "false")
					{
						mValue.push_back(false);
					}
					else
					{
						inOutCurArgumentStr = startArgument;
						inOutCurIndex = startIndex;
						return false;
					}
				}
			}

			return mValue.size() > 0;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		const std::vector<std::string> OptionValueBoolVector::asStringVector() const
		{
			std::vector<std::string> ret(mValue.size());
			for (size_t i = 0; i < mValue.size(); ++i)
			{
				ret[i] = mValue[i] ? "true" : "false";
			}

			return ret;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueIntVector : public OptionValue
		{
		public:
			virtual const std::string asString() const { return std::to_string(mValue[0]); };
			virtual double asReal() const { return mValue[0]; };
			virtual int asInteger() const { return mValue[0]; };
			virtual bool asBool() const { return mValue[0]; };

			virtual const std::vector<std::string> asStringVector() const;
			virtual const std::vector<double> asRealVector() const
			{
				return std::vector<double>(mValue.begin(), mValue.end());
			}
			virtual const std::vector<int> asIntegerVector() const { return std::vector<int>(mValue); }
			virtual const std::vector<bool> asBoolVector() const
			{
				return std::vector<bool>(mValue.begin(), mValue.end());
			}

			virtual uint8_t type() const { return ARG_INT_VEC; }

			virtual bool read(int& inOutCurIndex,
				char** inOutCurArgumentStr, int argc, char** inArgv);

		private:
			std::vector<int> mValue;
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		const std::vector<std::string> OptionValueIntVector::asStringVector() const
		{
			std::vector<std::string> ret(mValue.size());
			for (size_t i = 0; i < mValue.size(); ++i)
			{
				ret[i] = std::to_string(mValue[i]);
			}

			return ret;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool OptionValueIntVector::read(int& inOutCurIndex, char** inOutCurArgumentStr,
			int argc, char** inArgv)
		{
			int startIndex = inOutCurIndex;
			char** startArgument = inOutCurArgumentStr;

			mValue.clear();

			int intArg;
			while (**inOutCurArgumentStr != '\0'
				&& **inOutCurArgumentStr != '-'
				&& inOutCurIndex < argc)
			{
				if (readInt(inOutCurIndex, inOutCurArgumentStr, inArgv, intArg))
				{
					mValue.push_back(intArg);
				}
			}

			return mValue.size() > 0;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueRealVector : public OptionValue
		{
		public:
			virtual const std::string asString() const { return std::to_string(mValue[0]); };
			virtual double asReal() const { return mValue[0]; };

			virtual const std::vector<std::string> asStringVector() const;
			virtual const std::vector<double> asRealVector() const { return mValue; }

			virtual uint8_t type() const { return ARG_REAL_VEC; }

			virtual bool read(int& inOutCurIndex,
				char** inOutCurArgumentStr, int argc, char** inArgv);

		private:
			std::vector<double> mValue;
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		const std::vector<std::string> OptionValueRealVector::asStringVector() const
		{
			std::vector<std::string> ret(mValue.size());
			for (size_t i = 0; i < mValue.size(); ++i)
			{
				ret[i] = std::to_string(mValue[i]);
			}

			return ret;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool OptionValueRealVector::read(int& inOutCurIndex, char** inOutCurArgumentStr,
			int argc, char** inArgv)
		{
			int startIndex = inOutCurIndex;
			char** startArgument = inOutCurArgumentStr;

			mValue.clear();

			double doubleArg;
			while (**inOutCurArgumentStr != '\0'
				&& **inOutCurArgumentStr != '-'
				&& inOutCurIndex < argc)
			{
				if (readDouble(inOutCurIndex, inOutCurArgumentStr, inArgv, doubleArg))
				{
					mValue.push_back(doubleArg);
				}
			}

			return mValue.size() > 0;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueStringVector : public OptionValue
		{
		public:
			virtual const std::string asString() const { return mValue[0]; };
			virtual const std::vector<std::string> asStringVector() const { return mValue; }

			virtual uint8_t type() const { return ARG_STRING_VEC; }

			virtual bool read(int& inOutCurIndex,
				char** inOutCurArgumentStr, int argc, char** inArgv);

		private:
			std::vector<std::string> mValue;
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool OptionValueStringVector::read(int& inOutCurIndex, char** inOutCurArgumentStr,
			int argc, char** inArgv)
		{
			int startIndex = inOutCurIndex;
			char** startArgument = inOutCurArgumentStr;

			mValue.clear();

			std::string strArg;
			while (**inOutCurArgumentStr != '\0'
				&& **inOutCurArgumentStr != '-'
				&& inOutCurIndex < argc)
			{
				if (readString(inOutCurIndex, inOutCurArgumentStr, inArgv, strArg))
				{
					mValue.push_back(strArg);
				}
			}

			return mValue.size() > 0;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/
	};

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	// Base class for options with different type of arguments defined in private namespace.
	// Provides basic functionality and interface.
	class Option
	{
	public:
		Option(const char shortName, const std::string longName, OptionValue* value)
			: mShortName(shortName), mLongName(longName), mValue(value)
		{
			if (!value)
			{
				mValue = &sOptionNone;
			}
		}

		virtual ~Option() {}

		const char shortName() const
		{
			return mShortName;
		}

		const std::string longName() const
		{
			return mLongName;
		}

		const OptionValue& value() const
		{
			return *mValue;
		}

	private:
		static hidden::OptionValueUnexisted sOptionNone;

		const char mShortName;
		const std::string mLongName;
		OptionValue* mValue;
	};

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	hidden::OptionValueUnexisted Option::sOptionNone = hidden::OptionValueUnexisted();

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	class Options
	{
	public:
		Options(OptDescriptors& descriptors, int argc, char** argv)
			: mDescriptors(descriptors)
		{
			if (mDescriptors.ok())
			{
				parse(argc, argv);
			}
		}

		~Options()
		{
			for (size_t i = 0; i < mOptions.size(); ++i)
			{
				delete mOptions[i];
			}
			for (size_t i = 0; i < mOptionValues.size(); ++i)
			{
				delete mOptionValues[i];
			}
		}

		bool ok() const { return mOk; }
		std::string error() const { return mError.str(); }

		const Option& operator[](std::string opt) const;
		const Option& operator[](char opt) const;

	private:
		static Option sOptionNone;

		OptDescriptors mDescriptors;
		
		std::vector<Option*> mOptions;
		std::vector<OptionValue*> mOptionValues;

		std::stringstream mError;
		bool mOk = true;

		void parse(int argc, char** argv);
		bool readOptionNames(int& inOutCurIndex, char** inOutCurArgumentStr,
			char** inArgv, std::set<std::string>& outOptions);
	};

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	Option Options::sOptionNone = Option(OPT_SHORT_NONE, "", NULL);

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	// extract long option (--long --> { "long" }) 
	// or set (or single) of short options (-short --> { "s", "h", "o", "r", "t" })
	bool Options::readOptionNames(int& inOutCurIndex, char** inOutCurArgumentStr,
		char** inArgv, std::set<std::string>& outOptionNames)
	{
		if (**inOutCurArgumentStr == '-')
		{
			++* inOutCurArgumentStr;
			// -- (long option)
			if (**inOutCurArgumentStr == '-')
			{
				++* inOutCurArgumentStr;
				std::string optName;

				while ((**inOutCurArgumentStr != '=') && (**inOutCurArgumentStr != '\0'))
				{
					optName.push_back(*(*inOutCurArgumentStr)++);
				}

				if (**inOutCurArgumentStr == '\0')
				{
					++inOutCurIndex;
					*inOutCurArgumentStr = inArgv[inOutCurIndex];
				}
				else if (**inOutCurArgumentStr == '=')
				{
					++* inOutCurArgumentStr;
				}

				if (optName.size())
				{
					outOptionNames.insert(optName);
				}
				else
				{
					mOk = false;
					mError << "Error: " << inArgv[inOutCurIndex] << ". Zero length option.\n";
					return false;
				}
			}
			// - (short option(s))
			else
			{
				while ((**inOutCurArgumentStr != '=') && (**inOutCurArgumentStr != '\0'))
				{
					std::string opt(1, **inOutCurArgumentStr);
					if (outOptionNames.find(opt) == outOptionNames.end())
					{
						outOptionNames.insert(opt);
						++(*inOutCurArgumentStr);
					}
					else
					{
						mOk = false;
						mError << "Error: " << opt << ". Same option multiple times.\n";
						return false;
					}
				}

				if (**inOutCurArgumentStr == '\0')
				{
					++inOutCurIndex;
					*inOutCurArgumentStr = inArgv[inOutCurIndex];
				}
				else if (**inOutCurArgumentStr == '=')
				{
					++* inOutCurArgumentStr;
				}
			}
		}
		// not option
		else
		{
			mOk = false;
			mError << "Error: " << *inOutCurArgumentStr << ". Option expected.\n";
			return false;
		}

		return true;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	const Option& Options::operator[](std::string opt) const
	{
		if (opt.size() == 1) return operator[](opt[0]);

		for (size_t i = 0; i < mOptions.size(); ++i)
		{
			if (mOptions[i]->longName() == opt)
			{
				return *mOptions[i];
			}
		}

		return sOptionNone;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	const Option& Options::operator[](char opt) const
	{
		for (size_t i = 0; i < mOptions.size(); ++i)
		{
			if (mOptions[i]->shortName() == opt)
			{
				return *mOptions[i];
			}
		}

		return sOptionNone;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	void Options::parse(int argc, char** argv)
	{
		int curIndex = 1;
		char* curArgumentStr = argv[curIndex];

		while (curIndex < argc)
		{
			std::set<std::string> optNames;
			if (!readOptionNames(curIndex, &curArgumentStr, argv, optNames))
			{
				break;
			}

			bool optionsExist = true;
			bool onlyFlags = true;
			uint8_t typeBase = 0;
			uint8_t typeShift = 0;
			for (std::set<std::string>::const_iterator it = optNames.begin();
				it != optNames.end();
				++it)
			{
				const OptDescriptor* desc = mDescriptors[*it];
				if (desc == NULL)
				{
					optionsExist = false;
					mOk = false;
					mError << "Undefined option: " << *it << '\n';
					break;
				}

				const uint8_t curPossibleArgumentValues = desc->possibleArgumentValues();

				onlyFlags = onlyFlags && (curPossibleArgumentValues == ARG_NONE);

				if (hidden::isSingleArgType(curPossibleArgumentValues))
				{
					if (curPossibleArgumentValues > typeShift)
					{
						typeShift = curPossibleArgumentValues;
					}
				}
				else if (typeBase < curPossibleArgumentValues)
				{
					typeBase = curPossibleArgumentValues;
				}
			}

			if (!optionsExist) break;

			if (onlyFlags)
			{
				mOptionValues.push_back(new hidden::OptionValueBool);
			}
			else
			{
				uint8_t argumentValues;
				if (!typeBase)
				{
					argumentValues = typeShift;
				}
				else
				{
					argumentValues = typeBase * typeShift;
				}

				switch (argumentValues)
				{
				case ARG_STRING:
					mOptionValues.push_back(new hidden::OptionValueString); break;
				case ARG_NONE:
					mOptionValues.push_back(new hidden::OptionValueBool); break;
				case ARG_INT:
					mOptionValues.push_back(new hidden::OptionValueInt); break;
				case ARG_REAL:
					mOptionValues.push_back(new hidden::OptionValueReal); break;
				case ARG_STRING_VEC:
					mOptionValues.push_back(new hidden::OptionValueStringVector); break;
				case ARG_BOOL_VEC:
					mOptionValues.push_back(new hidden::OptionValueBoolVector); break;
				case ARG_INT_VEC:
					mOptionValues.push_back(new hidden::OptionValueIntVector); break;
				case ARG_REAL_VEC:
					mOptionValues.push_back(new hidden::OptionValueRealVector); break;
				}
			}

			if (!mOptionValues.back()->read(curIndex, &curArgumentStr, argc, argv))
			{
				mOk = false;
				mError << "Failed to read argument.\n";
			}
			for (std::set<std::string>::const_iterator it = optNames.begin();
				it != optNames.end(); ++it)
			{
				mOptions.push_back(new Option(mDescriptors[*it]->shortName(),
					mDescriptors[*it]->longName(),
					mOptionValues.back()));
			}
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

} // end of sclap

#endif // !SCLAP_H
