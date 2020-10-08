#ifndef SCLAP_H
#define SCLAP_H

#include <stdint.h>

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

	class OptionDescriptor
	{
	public:
		OptionDescriptor(const char shortName, const char* longName,
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

	class OptionDescriptors
	{
	public:
		OptionDescriptors(const std::vector<OptionDescriptor>& descriptors);
		OptionDescriptors() : mDescriptors(), mError(), mOk(true) {}
		OptionDescriptors(const OptionDescriptors& optDesc)
			: mDescriptors(optDesc.mDescriptors), mOk(optDesc.mOk), mError(optDesc.mError.str())
		{ }

		const OptionDescriptor* const operator[](const std::string& opt) const;
		const OptionDescriptor* const operator[](char opt) const;

		bool valid() const { return mOk; }
		std::string error() const { return mError.str(); }

		OptionDescriptors& operator<<(const OptionDescriptor& OptionDescriptor)
		{
			mDescriptors.push_back(OptionDescriptor);
			check();
			return *this;
		}

	private:
		std::vector<OptionDescriptor> mDescriptors;

		std::stringstream mError;
		bool mOk;

		void check();
	};

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	OptionDescriptors::OptionDescriptors(const std::vector<OptionDescriptor>& descriptors)
		: mDescriptors(descriptors), mError(), mOk(false)
	{
		check();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	void OptionDescriptors::check()
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

	const OptionDescriptor* const OptionDescriptors::operator[](const std::string& opt) const
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

	const OptionDescriptor* const OptionDescriptors::operator[](char opt) const
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

		virtual uint8_t type() const = 0;

		operator bool() const { return type(); }

		virtual bool read(int& inOutCurIndex,
			char** inOutCurArgumentStr, int argc, char** inArgv) = 0;
	};

	uint8_t OptionValue::type() const {
		return 0;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	/*------------------------------------------------------------------------------------------------*/
	/*////////////////////////////////////////////////////////////////////////////////////////////////*/

	namespace hidden
	{
		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		
		template <typename T>
		std::string numberToString(T Number)
		{
			std::ostringstream ss;
			ss << Number;
			return ss.str();
		}

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
			return end != s && *end == '\0';
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

		uint8_t getTypeToRead(uint8_t single, uint8_t vector)
		{
			if (vector == ARG_REAL_VEC) return ARG_REAL_VEC;
			else if (vector == ARG_INT_VEC) {
				  if (single == ARG_REAL) return ARG_REAL_VEC;
				  else return ARG_INT_VEC;
			}
			else if (vector == ARG_BOOL_VEC)
			{
				if (single == ARG_REAL) return ARG_REAL_VEC;
				else if (single == ARG_INT) return ARG_INT_VEC;
				else return ARG_BOOL_VEC;
			}
			else if (vector == ARG_STRING_VEC) {
				if (single == ARG_REAL) return ARG_REAL_VEC;
				else if (single == ARG_INT) return ARG_INT_VEC;
				else return ARG_STRING_VEC;
			}
			else return single;
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
			virtual const std::string asString() const { return mValue ? "true" : "false"; };

			virtual uint8_t type() const { return ARG_NONE; }

			virtual bool read(int& inOutCurIndex, char** inOutCurArgumentStr,
				int argc, char** inArgv);

			virtual bool asBool() const { return mValue; };

		private:
			bool mValue;
		};

		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		bool OptionValueBool::read(int& inOutCurIndex, char** inOutCurArgumentStr,
			int argc, char** inArgv)
		{
			if (inOutCurIndex < argc) 
			{	
				if (!strcmp(*inOutCurArgumentStr, "true"))
				{
					++inOutCurIndex;
					*inOutCurArgumentStr = inArgv[inOutCurIndex];
					mValue = true;
					return true;
				}
				else if (!strcmp(*inOutCurArgumentStr, "false"))
				{
					++inOutCurIndex;
					*inOutCurArgumentStr = inArgv[inOutCurIndex];
					mValue = false;
					return true;
				}	
			}

			mValue = true;
			return true;
		}

		/*////////////////////////////////////////////////////////////////////////////////////////////*/
		/*--------------------------------------------------------------------------------------------*/
		/*////////////////////////////////////////////////////////////////////////////////////////////*/

		class OptionValueInt : public OptionValue
		{
		public:
			virtual const std::string asString() const { return hidden::numberToString(mValue); };
			virtual double asReal() const { return mValue; };
			virtual int asInteger() const { return mValue; };
			virtual bool asBool() const { return mValue; };

			virtual const std::vector<std::string> asStringVector() const
			{
				return std::vector<std::string>(1, asString());
			}
			virtual const std::vector<double> asRealVector() const
			{
				return std::vector<double>(1, asReal());
			}
			virtual const std::vector<int> asIntegerVector() const
			{
				return std::vector<int>(1, asInteger());
			}
			virtual const std::vector<bool> asBoolVector() const
			{
				return std::vector<bool>(1, asBool());
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
			virtual const std::string asString() const { return hidden::numberToString(mValue); };
			virtual double asReal() const { return mValue; };
			virtual int asInteger() const { return (int)mValue; };

			virtual const std::vector<std::string> asStringVector() const
			{
				return std::vector<std::string>(1, asString());
			}
			virtual const std::vector<double> asRealVector() const
			{
				return std::vector<double>(1, asReal());
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
				return std::vector<std::string>(1, asString());
			}

			virtual uint8_t type() const { return ARG_STRING; }

			virtual bool read(int& inOutCurIndex,
				char** inOutCurArgumentStr, int argc, char** inArgv)
			{
				if (inOutCurIndex >= argc)
				{
					return false;
				}
				else
				{
					return readString(inOutCurIndex, inOutCurArgumentStr, inArgv, mValue);
				}
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

				valueStr = valueStr.substr(1, valueStr.size() - 1);
				return valueStr;
			}
			virtual const std::vector<std::string> asStringVector() const;
			virtual bool asBool() const { return mValue[0]; }
			virtual const std::vector<bool> asBoolVector() const { return mValue; }
  
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
			while (inOutCurIndex < argc 
				&& **inOutCurArgumentStr != '-' 
				&& **inOutCurArgumentStr != '\0')
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
			virtual const std::string asString() const { return hidden::numberToString(mValue[0]); };
			virtual double asReal() const { return mValue[0]; };
			virtual int asInteger() const { return mValue[0]; };
			virtual bool asBool() const { return mValue[0]; };

			virtual const std::vector<std::string> asStringVector() const;
			virtual const std::vector<double> asRealVector() const
			{
				return std::vector<double>(mValue.begin(), mValue.end());
			}
			virtual const std::vector<int> asIntegerVector() const
			{ 
				return mValue;
			}
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
				ret[i] = hidden::numberToString(mValue[i]);
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
			while (inOutCurIndex < argc
				&& **inOutCurArgumentStr != '-'
				&& **inOutCurArgumentStr != '\0')
			{
				if (readInt(inOutCurIndex, inOutCurArgumentStr, inArgv, intArg))
				{
					mValue.push_back(intArg);
				} 
				else
				{
					inOutCurIndex = startIndex;
					inOutCurArgumentStr = startArgument;
					return false;
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
			virtual const std::string asString() const { return hidden::numberToString(mValue[0]); };
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
				ret[i] = hidden::numberToString(mValue[i]);
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
			while (inOutCurIndex < argc
				&& **inOutCurArgumentStr != '-'
				&& **inOutCurArgumentStr != '\0')
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
			while (inOutCurIndex < argc
				&& **inOutCurArgumentStr != '-'
				&& **inOutCurArgumentStr != '\0')
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
	}

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

		uint8_t type() const { return mValue->type(); }
		bool asBool() const { return mValue->asBool(); }
		const std::vector<bool> asBoolVector() const { return mValue->asBoolVector(); }
		int asInteger() const { return mValue->asInteger(); }
		const std::vector<int> asIntegerVector() const { return mValue->asIntegerVector(); }
		double asDouble() const { return mValue->asReal(); }
		const std::vector<double> asRealVector() const { return mValue->asRealVector(); }
		const std::string asString() const { return mValue->asString(); }
		const std::vector<std::string> asStringVector() const { return mValue->asStringVector(); }
		operator bool() const { return asBool(); }

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
		Options(OptionDescriptors& descriptors, int argc, char** argv)
			: mDescriptors(descriptors)
		{
			if (mDescriptors.valid())
			{
				mOk = true;
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

		bool valid() const { return mOk; }
		std::string error() const { return mError.str(); }

		const Option& operator[](std::string option) const;
		const Option& operator[](char option) const;

	private:
		static Option sOptionNone;

		OptionDescriptors mDescriptors;
		
		std::vector<Option*> mOptions;
		std::vector<OptionValue*> mOptionValues;

		std::stringstream mError;
		bool mOk;

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
				std::string optionName;

				while ((**inOutCurArgumentStr != '=') && (**inOutCurArgumentStr != '\0'))
				{
					optionName.push_back(*(*inOutCurArgumentStr)++);
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

				if (optionName.size())
				{
					if (!mDescriptors[optionName])
					{
						mOk = false;
						mError << "Error: " << inArgv[inOutCurIndex] << " is not existing option.\n";
						return false;
					}
					outOptionNames.insert(optionName);
				}
				else
				{
					mOk = false;
					mError << "Error: " << optionName << ". Zero length option.\n";
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
						if (!mDescriptors[opt])
						{
							mOk = false;
							mError << "Error: " << opt << " is not existing option.\n";
							return false;
						}
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
			std::set<std::string> optionNames;
			if (!readOptionNames(curIndex, &curArgumentStr, argv, optionNames))
			{
				mOk = false;
				mError << mDescriptors.error();
				break;
			}

			bool optionsExist = true;
			bool onlyFlags = true;
			uint8_t typeVector = 0;
			uint8_t typeSingle = 1;
			for (std::set<std::string>::const_iterator it = optionNames.begin();
				it != optionNames.end();
				++it)
			{
				const OptionDescriptor* desc = mDescriptors[*it];
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
					if (curPossibleArgumentValues > typeSingle)
					{
						typeSingle = curPossibleArgumentValues;
					}
				}
				else if (typeVector < curPossibleArgumentValues)
				{
					typeVector = curPossibleArgumentValues;
				}
			}

			if (!optionsExist) break;

			if (onlyFlags)
			{
				mOptionValues.push_back(new hidden::OptionValueBool);
			}
			else
			{
				uint8_t argumentValues = hidden::getTypeToRead(typeSingle, typeVector);

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
				default:
					mError << "Read error.\n";
					exit(-1);
				}
			}

			if (!mOptionValues.back()->read(curIndex, &curArgumentStr, argc, argv))
			{
				mOk = false;
				mError << "Failed to read argument.\n";
				return;
			}

			for (std::set<std::string>::const_iterator it = optionNames.begin();
				it != optionNames.end(); ++it)
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
